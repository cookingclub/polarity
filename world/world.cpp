#include "world.hpp"
#include <iostream>
namespace Polarity {
  World *world = nullptr;

  World::World():physics(b2Vec2(0.0f, -10.0f)) {
    std::cerr << "World has started"<<std::endl;
  }
  void World::init() {
      world = new World();
  }
  void World::addObject(const b2BodyDef&bdef) {
      GameObject * object = new GameObject(&physics, bdef);
      objects.emplace_back(object);
  }
  void World::tick() {
    // Gets called every frame
  }
}
