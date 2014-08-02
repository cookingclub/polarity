#include "physics.hpp"

namespace Polarity {
  GameObject::GameObject(b2World *world, const b2BodyDef &bdef) {
    groundBody = world->CreateBody(&bdef);
  }
}
