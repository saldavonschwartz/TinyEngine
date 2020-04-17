//
//  main.cpp
//  TinyEngine
//
//  Created by Federico Saldarini on 10/20/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#include "scratch/PathTracer.hpp"

int main(int argc, const char * argv[]) {
  runPathTracer("", 320, 200, 100, 50);
  return 0;
}

//#include "v2/Engine2D/World.hpp"
//
//using namespace std;
//
//int main(int argc, const char * argv[]) {
//  string path = "/Users/saldavonschwartz/workspace/TinyEngine/TinyEngine/v2/Assets/";
//
//  World w;
//  w.init();
//
//  auto atlas = w.sys.render.createAtlas(1024, 512);
//  auto img1 = w.sys.render.loadTexture(path + "Textures/tx1.jpg", atlas);
//  auto img2 = w.sys.render.loadTexture(path + "Textures/rocketAlpha.png", atlas);
//  auto program = w.sys.render.loadShaderProgram(path + "Shaders/sprite.glsl");
//
//  w.comp.shader.set(0, {
//    .program = program,
//    .params = {{"tint", vec3{1.,1.,1.}}, {"sprite", img1}}
//  });
//
//  w.comp.transform.set(0, {
//    .position = vec3{128., 128., 0.1},
//    .scale = vec2{128.,128.}
//  });
//
//  w.comp.shader.set(1, {
//    .program = program,
//    .params = {{"tint", vec3{1.,1.,1.}}, {"sprite", img2}}
//  });
//
//  w.comp.transform.set(1, {
//    .position = vec3{100., 0., 0.},
//    .scale = vec2{256.,256.}
//  });
//
//
////  auto atlas = w.renderSystem.createAtlas(1024, 256);
////  auto texture = w.renderSystem.loadTexture("", atlas);
////  auto program = w.renderSystem.loadShaderProgram("");
////
////  auto& shader = w.shaders.set(0, {.program = program});
////  shader.setParam("sprite", texture);
////  shader.setParam("tint", vec3{0.,1.,0.});
////
////  w.transforms.set(0, {
////    .position = vec3{20., 20., 0.}
////  });
////
//  w.tick();
//  return 0;
//}
