#include <vector>
#include <Box2D/Box2D.h>
#include "behavior.hpp"




namespace Polarity {
  class Behavior;
  class World;


  class GameObject {
    Behavior * behavior;
  public:
    void tick(World*world);
    b2Body*groundBody;
    float printPosition();
    GameObject(b2World *world, Behavior * behavior, const b2BodyDef &bdef);
  };
  class Player: public GameObject {
  };
}
