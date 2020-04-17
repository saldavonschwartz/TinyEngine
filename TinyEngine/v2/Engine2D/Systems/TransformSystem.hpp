//
//  TransformSystem.hpp
//  TinyEngine
//
//  Created by Federico Saldarini on 11/10/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#ifndef TransformSystem_hpp
#define TransformSystem_hpp

#include "../Components/Transform.hpp"

#include <glm/mat4x4.hpp>
#include <unordered_map>
#include <vector>

using std::unordered_map;
using std::vector;
using glm::mat4;
using std::pair;

struct World;

class TransformSystem {
public:
  void query(World* world);
  void tick(World* world);
  
protected:
  bool shouldTick = false;
  unordered_map<size_t, vector<pair<size_t, size_t>>> levels;
  
  void tickOne(World* world, const mat4& P, const pair<size_t, size_t>& eidcid);
};

#endif /* TransformSystem_hpp */
