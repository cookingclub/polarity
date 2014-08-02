#include <vector>
#include <Box2D/Box2D.h>
namespace Polarity {
  class GameObject {
  public:
    b2Body*groundBody;
    GameObject(b2World *world, const b2BodyDef &bdef);
  };
  class Player: public GameObject {
  };
}
