#include <Box2D/Box2D.h>
#include "physics/vector.hpp"
#include "physics/physics.hpp"
#include "graphics/graphics.hpp"
#include <memory>
#include <vector>
namespace Polarity {
class World {
    b2World physics;
    b2Vec2 graphicsScale;
    b2Vec2 camera;
    std::vector<bool> keyState;
    std::vector<std::unique_ptr<GameObject> > objects;
    std::unique_ptr<LayerCollection> layers;

    void load(const std::string& tmxFile);
public:
    b2Vec2 getCamera()const {
        return camera;
    }
    static void init();
    World(const std::string& tmxFile);
    GameObject* addObject(Behavior*behavior, const b2BodyDef&, const b2FixtureDef&fixture, const std::string &name, GameObject::Type type, const PropertyMap &properties);
    void tick();
    void draw(SDL_Surface *screen);
    void keyEvent(int keyCode, bool pressed);
    bool isKeyDown(int keyCode);
    b2Vec2 graphicsToPhysics(const b2Vec2 &vec, float isBounds=-1) {
        return b2Vec2(vec.x / graphicsScale.x, vec.y / graphicsScale.y * isBounds);
    }
    b2Vec2 physicsToGraphics(const b2Vec2 &vec, float isBounds=-1) {
        return b2Vec2(vec.x * graphicsScale.x, vec.y * graphicsScale.y * isBounds);
    }
};
extern World *world;
}
