#include <iostream>
#include "SDL/SDL.h"
#include "world.hpp"
namespace Polarity {
  World *world = nullptr;

  World::World():physics(b2Vec2(0.0f, -10.0f)),keyState(SDLK_LAST) {
    std::cerr << "World has started"<<std::endl;
    for (int i=0; i< SDLK_LAST; ++i) {
      keyState[i] = false;
    }
  }
  void World::init() {
      world = new World();
  }
  GameObject* World::addObject(const b2BodyDef&bdef) {
      GameObject * object = new GameObject(&physics, bdef);
      objects.emplace_back(object);
      return objects.back().get();
  }
  void World::keyEvent(int keyCode, bool pressed) {
    if (keyCode < SDLK_LAST) {
      keyState[keyCode] = pressed;
    }else {
      std::cerr << "Key code out of range "<<keyCode<<"\n";
    }
  }
  void World::tick() {
    for (auto &obj : objects) {
      obj->tick(this);
    }
    // Gets called every frame
  }
}
