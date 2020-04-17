//
//  Types.hpp
//  TinyEngine
//
//  Created by Federico Saldarini on 11/6/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#ifndef Types_hpp
#define Types_hpp

#include "Task.hpp"

#include <cstdint>
#include <stdio.h>
#include <tuple>
#include <cassert>
#include <chrono>
#include <iostream>

#define TINY_DEBUG 0

#if(TINY_DEBUG==1)
#define ASSERT_DEBUG(x) assert(x)
#else
#define ASSERT_DEBUG(x)
#endif

using std::apply;

static const size_t id_none = SIZE_MAX;

struct Profiler {
  std::string name;
  std::chrono::high_resolution_clock::time_point p;
  
  Profiler(std::string const& n) :
  name(n), p(std::chrono::high_resolution_clock::now()) { }
  
  ~Profiler() {
    using duration = std::chrono::duration<double>;
    auto d = std::chrono::high_resolution_clock::now() - p;
    std::cout << "\n" << name << ": "
    << std::chrono::duration_cast<duration>(d).count()
    << " sec."
    << std::endl;
  }
};


#endif /* Types_hpp */
