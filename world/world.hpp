#include <Box2D/Box2D.h>
#include "physics/vector.hpp"
#include "physics/physics.hpp"
#include "graphics/graphics.hpp"
#include <memory>
#include <vector>
namespace Polarity {
class World {
  b2World physics;
  std::vector<bool> keyState;
  std::vector<std::unique_ptr<GameObject> > objects;
  std::unique_ptr<LayerCollection> layers;

  void load(const std::string& tmxFile);
public:
  static void init();
  World(const std::string& tmxFile);
  GameObject* addObject(Behavior*behavior, const b2BodyDef&, const b2FixtureDef&fixture);
  void tick();
  void draw(SDL_Surface *screen);
  void keyEvent(int keyCode, bool pressed);
  bool isKeyDown(int keyCode);
};
extern World *world;
}
