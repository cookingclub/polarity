#include <vector>
#include <Box2D/Box2D.h>
#include "behavior.hpp"

#include "SDL/SDL.h"



namespace Polarity {
  class Behavior;
  class World;

  class GameObject {
    Behavior * behavior;
  public:
    void tick(World*world);
    void draw(SDL_Surface* surface);
    b2Body*groundBody;
    float printPosition();
    GameObject(b2World *world, Behavior * behavior, const b2BodyDef &bdef, const b2FixtureDef &fixture);
  };
  class Player: public GameObject {
  };
}
