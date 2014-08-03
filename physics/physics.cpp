#include "physics.hpp"

using namespace std;

namespace Polarity {
  GameObject::GameObject(b2World *world, const b2BodyDef &bdef) {
    groundBody = world->CreateBody(&bdef);
  }

  void GameObject::tick(World *world) {
    behavior->tick(world, this);
  }
  float GameObject::printPosition(){
    return groundBody->GetPosition().y;

  }
}
