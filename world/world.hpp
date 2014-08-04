#ifndef POLARITY_WORLD_HPP__
#define POLARITY_WORLD_HPP__

#include <Box2D/Box2D.h>
#include "physics/vector.hpp"
#include "physics/physics.hpp"
#include "physics/contact.hpp"
#include "graphics/graphics.hpp"
#include "audio/audio.hpp"
#include <memory>
#include <vector>

using std::shared_ptr;

namespace Polarity {
class World {
    b2World physics;
    b2Vec2 graphicsScale;
    b2Vec2 camera;
    ContactListener contactListener;
    std::vector<bool> keyState;
    std::vector<std::unique_ptr<GameObject> > objects;
    std::unique_ptr<LayerCollection> layers;
    std::shared_ptr<AudioChannelPlayer> player;

    void load(const std::string& tmxFile);
public:
    b2Vec2 getCamera()const {
        return camera;
    }
    static void init(shared_ptr<AudioChannelPlayer> audioPlayer);
    World(const std::string& tmxFile, std::shared_ptr<AudioChannelPlayer> audioPlayer);
    GameObject* addObject(Behavior*behavior, const b2BodyDef&, const b2FixtureDef&fixture, const std::string &name, GameObject::Type type, const PropertyMap &properties);
    void tick();
    void draw(SDL_Surface *screen);
    void keyEvent(int keyCode, bool pressed);
    bool isKeyDown(int keyCode);
    shared_ptr<AudioChannelPlayer> audio() {
        return player;
    }
    b2Vec2 graphicsToPhysics(const b2Vec2 &vec, float isBounds=-1) {
        return b2Vec2(vec.x / graphicsScale.x, vec.y / graphicsScale.y * isBounds);
    }
    b2Vec2 physicsToGraphics(const b2Vec2 &vec, float isBounds=-1) {
        return b2Vec2(vec.x * graphicsScale.x, vec.y * graphicsScale.y * isBounds);
    }
};
extern World *world;
}
#endif
