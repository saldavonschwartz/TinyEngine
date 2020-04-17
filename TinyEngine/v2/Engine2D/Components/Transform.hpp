//
//  Transform.hpp
//  TinyEngine
//
//  Created by Federico Saldarini on 11/1/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#ifndef Transform_hpp
#define Transform_hpp

#include "../../Core/Utils.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using glm::mat4;
using glm::vec2;
using glm::vec3;

struct Transform {
  size_t parent = id_none;
  vec3 position{0};
  vec2 scale{1};
  float rotation = 0;
  mat4 M{1};
};

#endif /* Transform_hpp */
