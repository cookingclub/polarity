#include <Box2D/Box2D.h>
#include "physics/vector.hpp"
#include "physics/physics.hpp"
#include "graphics/graphics.hpp"
#include "audio/audio.hpp"
#include "player.hpp"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::unique_ptr;

namespace Polarity {
class World {
    b2World physics;
    b2Vec2 graphicsScale;
    b2Vec2 camera;
    std::vector<bool> keyState;
    std::vector<bool> keyPressedState;
    std::vector<std::unique_ptr<GameObject> > objects;
    unique_ptr<LayerCollection> layers;
    shared_ptr<AudioChannelPlayer> fAudioPlayer;
    shared_ptr<PlayerState> fPlayerState;
    shared_ptr<GameState> fGameState;

    void load(const std::string& tmxFile);
public:
    b2Vec2 getCamera()const {
        return camera;
    }
    static void init(shared_ptr<AudioChannelPlayer> audioPlayer, shared_ptr<PlayerState> playerState, shared_ptr<GameState> gameState);
    World(const std::string& tmxFile, std::shared_ptr<AudioChannelPlayer> audioPlayer, shared_ptr<PlayerState> _playerState, shared_ptr<GameState> _gameState);
    GameObject* addObject(Behavior*behavior, const b2BodyDef&, const b2FixtureDef&fixture, const std::string &name, GameObject::Type type, const PropertyMap &properties);
    void tick();
    void draw(SDL_Surface *screen);
    void keyEvent(int keyCode, bool pressed);
    void keyPressedEvent(int keyCode, bool isPressed);
    bool isKeyDown(int keyCode);
    bool isKeyPressed(int keyCode);
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
