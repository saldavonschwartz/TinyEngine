//
//  RenderSystem.hpp
//  TinyEngine
//
//  Created by Federico Saldarini on 11/1/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#ifndef RenderSystem_hpp
#define RenderSystem_hpp

#include "../../Core/System.hpp"
#include "../Components/Transform.hpp"
#include "../Components/SpriteRenderer.hpp"

#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using std::vector;
using std::pair;

struct GLFWwindow;

class RenderSystem : public System {

public:
  size_t batchSize = 50;
  
  void init();
  void query(World* world);
  void tick(World* world);
  
  size_t createAtlas(uint32_t size, uint32_t cellSize);
  size_t loadTexture(const string& path, size_t atlas);
  size_t loadShaderProgram(const string& path);
  
protected:
  enum {
    AttrPositions = 0,
    AttrTXCoords = 1,
    AttrTXOffset = 2,
    AttrWTransform = 3
  };
  
  struct TXAtlas {
    uint32_t id;
    uint32_t dim = 0, cellSize = 0;
    vector<uint8_t> grid;
  };
  
  struct Texture {
    size_t atlas;
    vec2 origin;
  };
  
  GLFWwindow* win = nullptr;
  unordered_map<string, uint32_t> programStages;
  vector<pair<size_t, size_t>> sortedSprites;
  unordered_map<size_t, uint32_t> programs;
  unordered_map<size_t, Texture> textures;
  unordered_map<size_t, TXAtlas> atlases;
  uint32_t BuffWTransform = 0;
  uint32_t BuffTXOffset = 0;
  size_t programCounter = 0;
  size_t textureCounter = 0;
  size_t atlasCounter = 0;
  uint32_t quadVao = 0;
  
  void initVertexBuffers();
  void submitParams(uint32_t pid, const unordered_map<string, vector<ShaderParam>>& params);
};


#endif /* RenderSystem_hpp */
