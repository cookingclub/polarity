#ifndef POLARITY_WORLD_HPP__
#define POLARITY_WORLD_HPP__

#include <Box2D/Box2D.h>
#include "physics/vector.hpp"
#include "physics/game_object.hpp"
#include "physics/contact.hpp"
#include "graphics/layer_collection.hpp"
#include "audio/audio.hpp"
#include "player.hpp"
#include <memory>
#include <vector>
#include <set>

using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::set;

namespace Polarity {
class Canvas;

class World {
    bool loaded;
    b2World physics;
    b2Vec2 graphicsScale;
    b2Vec2 camera;
    ContactListener contactListener;
    b2Vec2 screenDimensions;
    b2Vec2 mapDimensions;
    std::set<int> keyState;
    std::set<int> keyPressedThisTick;
    vector< unique_ptr<GameObject> > objects;
    unique_ptr<LayerCollection> layers;
    shared_ptr<AudioChannelPlayer> fAudioPlayer;
    shared_ptr<PlayerState> fPlayerState;
    shared_ptr<GameState> fGameState;

    shared_ptr<Canvas> graphicsContext;

    std::shared_ptr<World> backgroundWorld;
    std::weak_ptr<World> wthis;
public:
    virtual ~World();
    //int pn = 1; //player polarity, this should prob be set in player state
    //int EMoff = 1; //EM force ON or OFF
    GameObject*player;
    b2Vec2 getCamera()const {
        return camera;
    }
    void updateCamera(GameObject *obj, b2Vec2 player);
    static void init(const shared_ptr<Canvas> &canvas, shared_ptr<AudioChannelPlayer> audioPlayer, shared_ptr<PlayerState> playerState, shared_ptr<GameState> gameState, const char *tmxFile=nullptr);
    shared_ptr<World> loadNewWorld(const std::string& tmxFile);
    void setBackgroundWorld(shared_ptr<World> newWorld);
    void switchToBackgroundWorld();
    World(const shared_ptr<Canvas> &canvas, std::shared_ptr<AudioChannelPlayer> audioPlayer, shared_ptr<PlayerState> _playerState, shared_ptr<GameState> _gameState);
    void addObject(Behavior*behavior, const b2BodyDef&, const std::vector<b2FixtureDef>&fixture, const std::string &name, GameObject::Type type, const PropertyMap &properties);
    void tick();
    void draw(Canvas *screen);
    void keyEvent(int keyCode, bool pressed);
    bool isKeyDown(int keyCode);
    bool wasKeyJustPressed(int keyCode);
    std::weak_ptr<World> weak_ptr()const {return wthis;}
    const shared_ptr<Canvas> &getGraphicsContext() const {
        return graphicsContext;
    }
    class TmxMapWrapper;

    bool isLoaded() const {
        return loaded;
    }

    const std::set<int> &getKeyState() const {
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
    b2World *getPhysicsWorld() {
        return &physics;
    }

private:
    void load(const std::string& tmxFile);
    static void load_async(const std::weak_ptr<World> &weakThis, const std::string &tmxFile,
                           const char * data, int size);
    static void finalizeLoad(const std::weak_ptr<World> &weakThis,
                             const std::string &tmxFile,
                             TmxMapWrapper *data);
};
extern std::shared_ptr<World> world;
}
#endif
