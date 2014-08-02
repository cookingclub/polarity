#include <Box2D/Box2D.h>
#include "physics/vector.hpp"
#include "physics/physics.hpp"
#include <vector>
namespace Polarity {
class World {
  b2World physics;
  std::vector<GameObject*> objects;
public:
  World():physics(b2Vec2(0.0f, -10.0f)) {
  }
};
extern World *world;
}
