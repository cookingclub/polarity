#include <Box2D/Box2D.h>
#include "physics/vector.hpp"
#include "physics/physics.hpp"
#include <memory>
#include <vector>
namespace Polarity {
class World {
  b2World physics;
  std::vector<std::unique_ptr<GameObject> > objects;
public:
  static void init();
  World();
  void addObject(const b2BodyDef&);
  void tick();
};
extern World *world;
}
