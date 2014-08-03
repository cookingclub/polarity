#include "physics.hpp"

using namespace std;

namespace Polarity {
  GameObject::GameObject(b2World *world, Behavior *behavior, const b2BodyDef &bdef, const b2FixtureDef &fixture) {
    this->behavior = behavior;
    groundBody = world->CreateBody(&bdef);
    groundBody->CreateFixture(&fixture);
  }

  void GameObject::tick(World *world) {
    behavior->tick(world, this);
  }
  float GameObject::printPosition(){
    return groundBody->GetPosition().y;

  }
}
