//
//  InputSystem.cpp
//  TinyEngine
//
//  Created by Federico Saldarini on 11/20/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#include "InputSystem.hpp"

#include <unordered_map>

using std::unordered_map;
unordered_map<size_t, uint8_t> inputSystemKeys;

void InputSystem::set() {
  inputSystemKeys[2] = 66;
  inputSystemKeys[1] = 22;
}
