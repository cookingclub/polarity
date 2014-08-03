#include <Box2D/Box2D.h>
#include "physics/vector.hpp"
#include "physics/physics.hpp"
#include "graphics/graphics.hpp"
#include "audio/audio.hpp"
#include <memory>
#include <vector>

namespace Polarity {
class World {
    b2World physics;
    b2Vec2 camera;
    std::vector<bool> keyState;
    std::vector<std::unique_ptr<GameObject> > objects;
    std::unique_ptr<LayerCollection> layers;
    std::shared_ptr<AudioChannelPlayer> player;

    void load(const std::string& tmxFile);
public:
    b2Vec2 getCamera()const {
        return camera;
    }
    static void init();
    World(const std::string& tmxFile, std::shared_ptr<AudioChannelPlayer> audioPlayer);
    GameObject* addObject(Behavior*behavior, const b2BodyDef&, const b2FixtureDef&fixture, const std::string &name, GameObject::Type type, const PropertyMap &properties);
    void tick();
    void draw(SDL_Surface *screen);
    void keyEvent(int keyCode, bool pressed);
    bool isKeyDown(int keyCode);
};
extern World *world;
}
