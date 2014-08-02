#include <vector>
#include <Box2D/Box2D.h>
namespace Polarity {
  class GameObject {
  public:
     b2Body*groundBody;
     GameObject();
  };
  class Player: public GameObject {
  };
}
