#include <Box2D/Box2D.h>
#include "physics/vector.hpp"
#include "physics/physics.hpp"
#include <memory>
#include <vector>
namespace Polarity {
class World {
  b2World physics;
  std::vector<bool> keyState;
  std::vector<std::unique_ptr<GameObject> > objects;
public:
  static void init();
  World();
  GameObject* addObject(Behavior*behavior, const b2BodyDef&);
  void tick();
  void keyEvent(int keyCode, bool pressed);
  bool isKeyDown(int keyCode);
};
extern World *world;
}
