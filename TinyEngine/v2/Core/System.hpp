//
//  System.hpp
//  TinyEngine
//
//  Created by Federico Saldarini on 10/31/19.
//  Copyright © 2019 0xfede. All rights reserved.
//

#ifndef System_hpp
#define System_hpp

class World;

// Basically an Interface to follow:
struct System {
  // Called by world: (optional override)
  void init(World* world) {}
  
  // Called by world: (optional override)
  void shutDown(World* world) {}
  
  // Called by world: (optional override)
  void query(World* world) {}
  
  // Called by world: (required.)
  void tick(World* world);
};

#endif /* System_hpp */
