//
//  RenderSystem.cpp
//  TinyEngine
//
//  Created by Federico Saldarini on 11/4/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#include "RenderSystem.hpp"
#include "../World.hpp"
#include "../../Core/Utils.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <regex>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"


using std::regex;
using std::smatch;
using std::ifstream;


//static void checkGLError() {
//  uint32_t e;
//
//  while(!(e = glGetError()) ){
//    std::cout << "[Render System]: Error (GL): " << e << std::endl;
//  }
//}

//static mat4 P = mat4{1};
static mat4 P = mat4{1};

template <class T>
static vector<T> toValVec(const vector<ShaderParam>& pVals) {
  vector<T> v;
  
  for (auto& pVal : pVals) {
    v.emplace_back(pVal.get<T>());
  }
  
  return v;
}

static void checkShaderStatus(GLuint id) {
  GLint ok;
  GLchar info[1024];
  glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
  
  if (!ok) {
    glGetShaderInfoLog(id, 1024, NULL, info);
    std::cout << "[Render System]: Error: " << info << std::endl;
    assert(ok);
  }
}

static void checkProgramStatus(GLuint id) {
  GLint ok;
  GLchar info[1024];
  glGetProgramiv(id, GL_LINK_STATUS, &ok);
    
  if (!ok) {
    glGetProgramInfoLog(id, 1024, NULL, info);
    std::cout << "[Render System]: Error: " << info << std::endl;
    assert(ok);
  }
}

static void onGLFWError(int error, const char* description) {
  std::cout << "[Render System]: Error " << error << " : " << description << std::endl;
}

static void onGLFWBufferResize(GLFWwindow* window, int w, int h) {
  glViewport(0, 0, w, h);
  P = mat4{
    2./w, 0., 0., 0.,
    0., 2./h, 0., 0.,
    0., 0., 2., 0.,
    -1., -1., -1, 1.
  };

}

void RenderSystem::initVertexBuffers() {
  float v[] = {
    -1, -1, 0, 0, 0,
    +1, -1, 0, 1, 0,
    +1, +1, 0, 1, 1,
    -1, +1, 0, 0, 1,
  };

  uint32_t i[] = {
    0, 1, 2,  // tri 1
    2, 3, 0   // tri 2
  };
  
  uint32_t BuffAttributes, BuffIndices;
  glGenVertexArrays(1, &quadVao);
  glGenBuffers(1, &BuffAttributes);
  glGenBuffers(1, &BuffIndices);
  glGenBuffers(1, &BuffWTransform);
  glGenBuffers(1, &BuffTXOffset);
  
  glBindVertexArray(quadVao);
  
  glBindBuffer(GL_ARRAY_BUFFER, BuffAttributes);
  glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(AttrPositions);
  glVertexAttribPointer(AttrPositions, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
  
  glEnableVertexAttribArray(AttrTXCoords);
  glVertexAttribPointer(AttrTXCoords, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float)*3));
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BuffIndices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(i), i, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, BuffTXOffset);
  glEnableVertexAttribArray(AttrTXOffset);
  glVertexAttribPointer(AttrTXOffset, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), nullptr);
  glVertexAttribDivisor(AttrTXOffset, 1);

  glBindBuffer(GL_ARRAY_BUFFER, BuffWTransform);

  for (int i = 0; i < 4 ; i++) {
    glEnableVertexAttribArray(AttrWTransform + i);
    glVertexAttribPointer(AttrWTransform + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float)*i*4));
    glVertexAttribDivisor(AttrWTransform + i, 1);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

size_t RenderSystem::loadShaderProgram(const string& path) {
  unordered_map<string, string> source;
  std::ifstream f(path);
  assert(!f.fail());
  
  string regexStr;
  
  for (auto& [stageName, ignore] : programStages) {
    regexStr += !regexStr.size() ? "(" + stageName : "|" + stageName;
  }
  
  regexStr += ")";
  
  regex start(regexStr);
  string activeStage;
  smatch result;
  string temp;
  
  while(f.good()) {
    getline(f, temp);
    
    if (regex_search(temp, result, start)) {
      activeStage = result[0].str();
      continue;
    }
    
    source[activeStage] += temp += '\n';
  }
  
  uint32_t pid = glCreateProgram();
  vector<uint32_t> sids;
  uint32_t sid;
  
  for (auto& [stageName, stageSource] : source) {
    if (stageSource.size()) {
      sids.push_back(glCreateShader(programStages[stageName]));
      sid = sids.back();
      const char* const src = stageSource.c_str();
      glShaderSource(sid, 1, &src, nullptr);
      glCompileShader(sid);
      checkShaderStatus(sid);
      glAttachShader(pid, sid);
    }
  }
  
  glLinkProgram(pid);
  checkProgramStatus(pid);
  
  for (auto sid : sids) {
    glDetachShader(pid, sid);
    glDeleteShader(sid);
  }
  
  programs[programCounter] = pid;
  return programCounter++;
}

size_t RenderSystem::createAtlas(uint32_t size, uint32_t cellSize) {
  TXAtlas a {.dim = size/cellSize, .cellSize = cellSize};
  a.grid = vector<uint8_t>((size*size)/cellSize);
  
  glGenTextures(1, &a.id);
  glBindTexture(GL_TEXTURE_2D, a.id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
  atlases.emplace(atlasCounter, a);
  return atlasCounter++;
}

size_t RenderSystem::loadTexture(const string& path, size_t atlas) {
  int w, h, channels;
  
  stbi_set_flip_vertically_on_load(true);
  uint8_t* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
  
  auto& a = atlases[atlas];
//  w = ceil((float)w/a.cellSize);
//  h = ceil((float)h/a.cellSize);
  int i_ = 0, j_ = 0;
  bool done = false;
  
  for (int i = 0; i < a.dim; i++) {
    if (done) {
      break;
    }
    
    for (int j = 0; j < a.dim; j++) {
      if (a.grid[i*a.dim+j] == 0) {
        a.grid[i*a.dim+j] = 1;
        i_ = i;
        j_ = j;
        done = true;
        break;
      }
    }
  }
  
  glBindTexture(GL_TEXTURE_2D, a.id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, i_ * w, j_ * h, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  free(data);
  
  textures.emplace(textureCounter, Texture{atlas, {i_, j_}});
  return textureCounter++;
}

void RenderSystem::init() {
  programStages = {
    {"VERTEX_STAGE", GL_VERTEX_SHADER},
    {"FRAGMENT_STAGE", GL_FRAGMENT_SHADER},
  };
  
  glfwSetErrorCallback(onGLFWError);
  glfwInit();
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  
  win = glfwCreateWindow(640, 480, "", NULL, NULL);
  glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetFramebufferSizeCallback(win, onGLFWBufferResize);
  glfwMakeContextCurrent(win);
  printf("[Render System]: GL Version: %s\n", glGetString(GL_VERSION));
  glfwSwapInterval(0);
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);
  
  glewExperimental = true;
  glewInit();
  initVertexBuffers();
  
  int w, h;
  glfwGetFramebufferSize(win, &w, &h);
  glViewport(0, 0, w, h);
  P = mat4{
    2./w, 0., 0., 0.,
    0., 2./h, 0., 0.,
    0., 0., 2., 0.,
    -1., -1., -1, 1.
  };
}

void RenderSystem::query(World *world) {
  auto& S = world->comp.shader;
  auto& T = world->comp.transform;

  if (!S.hasChanges && !T.hasChanges) {
    return;
  }

  sortedSprites.clear();
  
  for (auto& [eid, cid] : S.eid2cidMap()) {
    auto found = T.eid2cidMap().find(eid);
    
    if (found != T.eid2cidMap().end()) {
      sortedSprites.emplace_back(cid, found->second);
    }
  }
  
  std::sort(sortedSprites.begin(), sortedSprites.end(), [&S](auto&& s1, auto&& s2) {
    return S[s1.first].program < S[s2.first].program;
  });
}

void RenderSystem::submitParams(uint32_t pid, const unordered_map<string, vector<ShaderParam>>& params) {
  for (auto& [pName, pVals] : params) {
    int32_t uid = glGetUniformLocation(pid, pName.c_str());

    if (uid == -1) {
      continue;
    }

    if (pVals[0].holds<size_t>()) {
      glActiveTexture(GL_TEXTURE0);
      size_t tid = pVals[0].get<size_t>();
      auto a = atlases[textures[tid].atlas];
      glBindTexture(GL_TEXTURE_2D, a.id);
      glUniform1i(uid, 0);
    }
    else if (pVals[0].holds<float>()) {
      auto vals = toValVec<float>(pVals);
      glUniform1fv(uid, vals.size(), (GLfloat*)&vals[0]);
    }
    else if (pVals[0].holds<vec2>()) {
      auto vals = toValVec<vec2>(pVals);
      glUniform2fv(uid, vals.size(), (GLfloat*)&vals[0]);
    }
    else if (pVals[0].holds<vec3>()) {
      auto vals = toValVec<vec3>(pVals);
      glUniform3fv(uid, vals.size(), (GLfloat*)&vals[0]);
    }
    else if (pVals[0].holds<vec4>()) {
      auto vals = toValVec<vec4>(pVals);
      glUniform4fv(uid, vals.size(), (GLfloat*)&vals[0]);
    }
    else if (pVals[0].holds<mat3>()) {
      auto vals = toValVec<mat3>(pVals);
      glUniformMatrix3fv(uid, vals.size(), GL_FALSE, (GLfloat*)&vals[0]);
    }
    else if (pVals[0].holds<mat4>()) {
      auto vals = toValVec<mat4>(pVals);
      glUniformMatrix4fv(uid, vals.size(), GL_FALSE, (GLfloat*)&vals[0]);
    }
  }
}

void RenderSystem::tick(World* world) {
  auto& S = world->comp.shader;
  auto& T = world->comp.transform;
  uint32_t currentProgram = -1;
  vector<mat4> M;
  vector<vec2> O;
  unordered_map<string, vector<ShaderParam>> params;
  size_t batchCount = 0;
  
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glBindVertexArray(quadVao);
  
  for (auto& [sid, tid] : sortedSprites) {
    M.emplace_back(P*T[tid].M);
    auto& shader = S[sid];
    auto t = shader.params["sprite"].get<size_t>();
    O.emplace_back(textures[t].origin);
    
    for (auto& [name, val] : shader.params) {
      params[name].push_back(val);
    }
    
    batchCount += 1;
    
    if (batchCount == batchSize ||
        batchCount == sortedSprites.size()) {
      glBindBuffer(GL_ARRAY_BUFFER, BuffWTransform);
      glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * M.size(), &M[0][0], GL_DYNAMIC_DRAW);
      
      glBindBuffer(GL_ARRAY_BUFFER, BuffTXOffset);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * O.size(), &O[0][0], GL_DYNAMIC_DRAW);
      
      currentProgram = currentProgram == -1 ? programs[shader.program] : currentProgram;
      glUseProgram(currentProgram);
      submitParams(currentProgram, params);
      glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, batchCount);
      
      batchCount = 0;
      currentProgram = programs[shader.program];
    }
  }
  
  glUseProgram(0);
  glBindVertexArray(0);
  glfwSwapBuffers(win);
  glfwPollEvents();
}

