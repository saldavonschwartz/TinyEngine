//
//  World.hpp
//  TinyEngine
//
//  Created by Federico Saldarini on 11/1/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#ifndef World_hpp
#define World_hpp

#include "../Core/ComponentPool.hpp"
#include "Components/Components.hpp"
#include "Systems/Systems.hpp"
#include "../Core/Utils.hpp"

#include <tuple>

using std::tuple;

struct World {
  struct Systems {
    TransformSystem transform;
    RenderSystem render;
  } sys;
  
  struct Comps {
    ComponentPool<Transform> transform;
    ComponentPool<Shader> shader;
  } comp;
  
  bool ticking = false;
  
  void init() {
    Task::init();
    comp.transform.reserve(10);
    comp.shader.reserve(10);
    sys.render.init();
  }
  
  void shutDown() {
    Task::shutDown();
  }
  
  void tick() {
    ticking = true;
    
    while (ticking) {
      Task::submit(
         [this](){
           sys.transform.query(this);
         },
         [this](){
           sys.render.query(this);
      }).wait();

      sys.transform.tick(this);
      sys.render.tick(this);
      
      comp.transform.hasChanges = false;
      comp.shader.hasChanges = false;
    }
    
    shutDown();
  }
};

#endif /* World_hpp */
