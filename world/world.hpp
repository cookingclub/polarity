#ifndef POLARITY_WORLD_HPP__
#define POLARITY_WORLD_HPP__

#include <Box2D/Box2D.h>
#include "physics/vector.hpp"
#include "physics/physics.hpp"
#include "physics/contact.hpp"
#include "graphics/graphics.hpp"
#include "audio/audio.hpp"
#include "player.hpp"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::unique_ptr;
using std::vector;

namespace Polarity {
class World {
    b2World physics;
    b2Vec2 graphicsScale;
    b2Vec2 camera;
    ContactListener contactListener;
    b2Vec2 screenDimensions;
    b2Vec2 mapDimensions;
    vector<bool> keyState;
    vector<bool> keyPressedThisTick;
    vector< unique_ptr<GameObject> > objects;
    unique_ptr<LayerCollection> layers;
    shared_ptr<AudioChannelPlayer> fAudioPlayer;
    shared_ptr<PlayerState> fPlayerState;
    shared_ptr<GameState> fGameState;

    void load(const std::string& tmxFile);
public:
    b2Vec2 getCamera()const {
        return camera;
    }
    void updateCamera(GameObject *obj, b2Vec2 player);
    static void init(shared_ptr<AudioChannelPlayer> audioPlayer, shared_ptr<PlayerState> playerState, shared_ptr<GameState> gameState);
    World(const std::string& tmxFile, std::shared_ptr<AudioChannelPlayer> audioPlayer, shared_ptr<PlayerState> _playerState, shared_ptr<GameState> _gameState);
    GameObject* addObject(Behavior*behavior, const b2BodyDef&, const std::vector<b2FixtureDef>&fixture, const std::string &name, GameObject::Type type, const PropertyMap &properties);
    void tick();
    void draw(SDL_Surface *screen);
    void keyEvent(int keyCode, bool pressed);
    void findKeysJustPressed(const vector<bool> &prevStates);
    void clearJustPressedStates();
    bool isKeyDown(int keyCode);
    bool wasKeyJustPressed(int keyCode);

    vector<bool> getKeyState() const {
        return keyState;
    }

    shared_ptr<AudioChannelPlayer> audio() {
        return fAudioPlayer;
    }

    shared_ptr<GameState> gameState() {
        return fGameState;
    }

    shared_ptr<PlayerState> playerState() {
        return fPlayerState;
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
