//
//  TransformSystem.cpp
//  TinyEngine
//
//  Created by Federico Saldarini on 11/10/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#include "TransformSystem.hpp"
#include "../World.hpp"
#include "../../Core/Task.hpp"

void TransformSystem::query(World* world) {
  auto& T = world->comp.transform;
  shouldTick = T.hasChanges;
  
  if (!shouldTick) {
    return;
  }
  
  levels.clear();

  for (auto& [eid, cid] : T.eid2cidMap()) {
    levels[T[cid].parent].emplace_back(eid, cid);
  }
}

void TransformSystem::tick(World* world) {
  if (!shouldTick) {
    return;
  }
  
  size_t taskCount = Task::threadCount;
  vector<pair<size_t,size_t>> batch;
  size_t batchSize = 0;
  Task::Group tasks;
  size_t i = 0;
  
  while ((batchSize = levels[-1].size() / taskCount) < 5 && taskCount > 1) {
    taskCount--;
  }

  for (auto& eidcid : levels[id_none]) {
    batch.emplace_back(eidcid);
    i += 1;

    if ((!(i % batchSize) && taskCount > 1) || (i == levels[-1].size())) {
      Task::submit(tasks, [this, batch, world](){
        for (auto& eidcid : batch) {
          tickOne(world, mat4{1}, eidcid);
        }
      });

      taskCount--;
      batch.clear();
    }
  }

  tasks.wait();
}

void TransformSystem::tickOne(World* world, const mat4& P, const pair<size_t, size_t>& eidcid) {
  auto& t = world->comp.transform[eidcid.second];
  
  float sina = sin(t.rotation);
  float cosa = cos(t.rotation);
  
  t.M = P * mat4{
    t.scale.x * cosa,   t.scale.x * sina, 0,              0,
    t.scale.y * -sina,  t.scale.y * cosa, 0,              0,
    0,                  0,                1,              0,
    t.position.x,       t.position.y,     t.position.z,   1
  };
  
  for (auto& child : levels[eidcid.first]) {
    tickOne(world, t.M, child);
  }
}
