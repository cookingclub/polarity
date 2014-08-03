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

  void GameObject::draw(SDL_Surface* screen) {
    SDL_Rect rect;
    rect.x = this->groundBody->GetPosition().x;
    rect.y = this->groundBody->GetPosition().y;
    // TODO: set weight and height
    rect.w = 10;
    rect.h = 10;
    // TODO: set the right color
    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 255, 0, 0));
  }

  float GameObject::printPosition(){
    return groundBody->GetPosition().y;
  }
}
