//
//  SpriteRenderer.h
//  TinyEngine
//
//  Created by Federico Saldarini on 11/4/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#ifndef SpriteRenderer_h
#define SpriteRenderer_h

#include "../../Core/Utils.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <variant>
#include <unordered_map>
#include <string>

using glm::ivec2;
using glm::ivec4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;
using std::variant;
using std::unordered_map;
using std::string;

class World;

template <class... Tn>
struct TinyVariant : public variant<Tn...> {
  using variant<Tn...>::variant;
  
  template <class T> T& get() {
    return *std::get_if<T>(this);
  }
  
  template <class T> const T& get() const {
    return *std::get_if<T>(this);
  }
  
  template <class T> bool holds() {
    return std::holds_alternative<T>(*this);
  }
  
  template <class T> bool holds() const {
    return std::holds_alternative<T>(*this);
  }
};

struct size2 : public ivec2 {
  int& w = ivec2::x;
  int& h = ivec2::y;
};

struct rect4 : public ivec4 {
  int& w = ivec4::z;
  int& h = ivec4::w;
};

using ShaderParam = TinyVariant<size_t, float, vec2, vec3, vec4, mat3, mat4>;

struct Shader {
  size_t program = id_none;
  unordered_map<string, ShaderParam> params;
};

//struct Logic {
//  function<void(World*)> onTick;
//};

#endif /* SpriteRenderer_h */
