//
//  Input.hpp
//  TinyEngine
//
//  Created by Federico Saldarini on 11/20/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#ifndef Input_hpp
#define Input_hpp

#include <unordered_map>

using std::unordered_map;

extern unordered_map<size_t, uint8_t> inputSystemKeys;

struct Input {
  static constexpr unordered_map<size_t, uint8_t>& keys = inputSystemKeys;
};

#endif /* Input_hpp */
