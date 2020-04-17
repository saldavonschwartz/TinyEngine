//
//  ComponentPool.hpp
//  TinyEngine
//
//  Created by Federico Saldarini on 11/6/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#ifndef ComponentPool_hpp
#define ComponentPool_hpp

#include "Utils.hpp"

#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <array>

using std::unordered_set;
using std::unordered_map;
using std::deque;
using std::vector;
using std::array;

template <class T>
class ComponentPool {
public:
  // Set by world:
  bool hasChanges = false;
  
  void reserve(size_t capacity) {
    data.reserve(capacity);
  }
  
  const unordered_map<size_t, size_t>& eid2cidMap() {
    return eid2cid;
  }
  
  T& operator[](const size_t& cid) {
    return data[cid];
  }
  
  T& set(size_t eid, const T& component) {
    const auto& found = eid2cid.find(eid);
    size_t cid = id_none;
    
    if (found != eid2cid.end()) {
      cid = found->second;
    }
    else if (!cidNext.empty()) {
      cid = cidNext.front();
      cidNext.pop_front();
    }
    
    if (cid != id_none) {
      data[cid] = component;
    }
    else {
      size_t cap = data.capacity();
      data.emplace_back(component);
      cid = data.size() - 1;
      
      if (cap < data.capacity()) {
        fprintf(stderr, "[Pool Warning:] increased capacity: %zu", data.capacity());
      }
    }
    
    eid2cid[eid] = cid;
    hasChanges = true;
    return data[cid];
  }
  
  T get(size_t eid) {
    ASSERT_DEBUG(eid2cid.find(eid) != eid2cid.end());
    return data[eid2cid[eid]];
  }
  
  bool has(size_t eid) {
    return eid2cid.find(eid) != eid2cid.end();
  }
  
  void del(size_t eid) {
    auto found = eid2cid[eid];
    ASSERT_DEBUG(found != eid2cid.end());
    cidNext.push_back(found.second);
    eid2cid.erase(eid);
    hasChanges = true;
  }
  
protected:
  // Map from eid to cid (index into pool):
  unordered_map<size_t, size_t> eid2cid;
  
  // For idx reuse in pool:
  deque<size_t> cidNext;
  
  vector<T> data;
};

#endif /* ComponentPool_hpp */
