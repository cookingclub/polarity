#include <iostream>
#include <iostream>
#include <libgen.h>
#include "SDL/SDL.h"
#include "world.hpp"
#include "physics/player_behavior.hpp"
#include "physics/magnetic_behavior.hpp"
#include "graphics/canvas.hpp"
#include "physics/jumping_behavior.hpp"
#include "world/door_behavior.hpp"
#include "main/main.hpp"
#include "tmxparser.h"

using std::shared_ptr;

namespace Polarity {
std::shared_ptr<World> world;

World::World(const shared_ptr<Canvas> &canvas, std::shared_ptr<AudioChannelPlayer> _audioPlayer,
            shared_ptr<PlayerState> _playerState, shared_ptr<GameState> _gameState)
        : loaded(false),
        physics(b2Vec2(0.0f, -10.0f)),
        graphicsScale(b2Vec2(96, 96)),
        camera(0, 300), //FIXME hard coded
        contactListener(this),
        screenDimensions(canvas->width(), canvas->height()),
        keyState(SDLK_LAST),
        keyPressedThisTick(SDLK_LAST, false),
        layers(nullptr),
        fAudioPlayer(_audioPlayer),
        fPlayerState(_playerState),
        fGameState(_gameState),
        graphicsContext(canvas) {
    std::cerr << "World has started"<<std::endl;
    for (int i=0; i< SDLK_LAST; ++i) {
        keyState[i] = false;
    }
    physics.SetContactListener(&contactListener);
}

void World::init(const shared_ptr<Canvas> &canvas, shared_ptr<AudioChannelPlayer> audioPlayer, shared_ptr<PlayerState> playerState, shared_ptr<GameState> gameState) {
    const char* tmxFile = "assets/levels/level3a.tmx";
    World *w = new World(canvas, audioPlayer, playerState, gameState);
    std::shared_ptr<World> sworld(w);
    w->wthis = sworld;
    w->load(tmxFile);
    world = sworld;
}

std::shared_ptr<World> World::loadNewWorld(const std::string& tmxFile) {
    World *w = new World(graphicsContext, fAudioPlayer, fPlayerState, fGameState);
    std::shared_ptr<World> sworld(w);
    w->wthis = sworld;
    w->load(tmxFile);
    return sworld;
}

void World::setBackgroundWorld(shared_ptr<World> newWorld) {
    backgroundWorld = newWorld;
}

void World::switchToBackgroundWorld() {
    if (backgroundWorld) {
        world = backgroundWorld;
        backgroundWorld.reset();
    }
}

void World::addObject(Behavior *behavior, const b2BodyDef&bdef, const std::vector<b2FixtureDef>&fixture, const std::string &name, GameObject::Type type, const PropertyMap &properties) {
    //std::weak_ptr<World> wthis(this->shared_from_this());
    auto it=properties.find("color");
    if (it != properties.end() && type == GameObject::PLATFORM) {
        Behavior* magnetism = new MagneticBehavior();
        behavior = behavior ? new CombiningBehavior(behavior, magnetism) : magnetism;
    }
    if (behavior == nullptr) {
        behavior = new Behavior();
    }
    GameObject * object = new GameObject(getGraphicsContext(), this, behavior, bdef, fixture, name, type, properties);
    objects.emplace_back(object);
    behavior->addedToWorld(this, object);

    if (type == GameObject::PLAYER){
        player = object;
    }
}

bool World::wasKeyJustPressed(int keyCode) {
    return keyPressedThisTick[keyCode];
}

bool World::isKeyDown(int keyCode) {
    return keyState[keyCode];
}

void World::keyEvent(int keyCode, bool pressed) {
    if (keyCode < SDLK_LAST) {
        keyState[keyCode] = pressed;
    }else {
        std::cerr << "Key code out of range "<<keyCode<<"\n";
    }
}

void World::findKeysJustPressed(const vector<bool> &prevStates) {
    assert(prevStates.size() == keyState.size());
    for (size_t k = 0; k < prevStates.size(); k++) {
        keyPressedThisTick[k] = (prevStates[k] != keyState[k]) && keyState[k];
    }
}

void World::clearJustPressedStates() {
    keyPressedThisTick = vector<bool>(keyState.size(), false);
}

void World::updateCamera(GameObject *obj, b2Vec2 player) {
    b2Vec2 delta = physicsToGraphics(player) - camera - 0.5f * screenDimensions;
    float lenY = delta.y > 0.0 ? delta.y : -delta.y;
    float lenX = delta.x > 0.0 ? delta.x : -delta.x;
    float maxMovementX = 64.0;
    float maxMovementY = 32.0;
    if (lenY > maxMovementY) {
        delta.y *= maxMovementY / lenY;
    }
    if (lenX > maxMovementX) {
        delta.x *= maxMovementX / lenX;
    }
    camera += delta;
    if (camera.x + screenDimensions.x > mapDimensions.x) {
        camera.x = mapDimensions.x - screenDimensions.x;
    }
    if (camera.y + screenDimensions.y > mapDimensions.y) {
        camera.y = mapDimensions.y - screenDimensions.y;
    }
    if (camera.y < 0) {
        camera.y = 0;
    }
    if (camera.x < 0) {
        camera.x = 0;
    }
}

void World::tick() {

    // timeStep, velocityIterations, positionIterations
    physics.Step(0.0166666, 8, 4);
    for (auto &obj : objects) {
        obj->tick(this);
        //std::cerr << obj->printPosition()<<std::endl;
    }
}

void World::draw(Canvas *screen) {
    if (backgroundWorld) {
        backgroundWorld->draw(screen);
    }
    if (layers) {
        for (auto& layer : layers->layers) {
            layer->draw(screen, -camera.x, -camera.y);
        }
    }
    for (auto& object : objects) {
        object->draw(this, screen);
    }
    screenDimensions.x = screen->width();
    screenDimensions.y = screen->height();
}
class World::TmxMapWrapper{
public:
    tmxparser::TmxMap map;
    ~TmxMapWrapper(){}
};

void World::load_async(const std::weak_ptr<World>&weakThis, const std::string &tmxFile,
                       const char * data, int size) {
    std::cerr << "load async " << tmxFile << std::endl;
    std::shared_ptr<World> world (weakThis.lock());
    if (world) {
        TmxMapWrapper *map = new TmxMapWrapper();
        tmxparser::TmxReturn error = tmxparser::parseFromMemory(const_cast<char*>(data), size,
                                                                &map->map);
        world->mapDimensions.x = map->map.width * map->map.tileWidth;
        world->mapDimensions.y = map->map.height * map->map.tileHeight;
        std::cerr << "done load async"<<std::endl;
        Polarity::mainThreadCallback(std::bind(&World::finalizeLoad, weakThis, tmxFile, map));
    }
}

void World::finalizeLoad(const std::weak_ptr<World> &weakThis,
                         const std::string &tmxFile,
                         TmxMapWrapper *data) {
    std::cerr << "finalize load " << tmxFile << std::endl;
    std::unique_ptr<TmxMapWrapper> destroyOnFunctionEnd(data);
    std::shared_ptr<World> world (weakThis.lock());
    if (world) {
        char *filecpy = new char[tmxFile.length() + 1];
        strncpy(filecpy, tmxFile.c_str(), tmxFile.length() + 1);
        std::string dir (dirname(filecpy));
        delete []filecpy;
        std::cerr << "Loading: " << tmxFile << " from directory " << dir << std::endl;
        world->layers = std::unique_ptr<LayerCollection>(new LayerCollection(world->getGraphicsContext().get(), dir, data->map));
        for (auto &it : data->map.tilesetCollection) {
            std::cerr << "Found tileset: " << it.name << std::endl;
        }
        for (auto &it : data->map.layerCollection) {
            std::cerr << "Found layer: " << it.name << std::endl;
        }
        for (auto &it : data->map.objectGroupCollection) {
            std::cerr << "Found object group: " << it.name << std::endl;
            for (auto &oit : it.objects) {
                b2BodyDef body_def;
                GameObject::Type type = GameObject::parseTypeStr(oit.type);
                Behavior *behavior = nullptr;
                std::vector<b2FixtureDef> fixtures;
                if (type == GameObject::PLAYER) {
                    body_def.type = b2_dynamicBody;
                } else {
                    body_def.type = b2_staticBody;
                }
                int y = oit.y;
                body_def.angle = oit.rotation;
                body_def.fixedRotation = true;
                if (oit.referenceGid) {
                    // Possibly bug in the map editor:
                    // Objects with a gid are positioned from bottom left.
                    y -= oit.height / 2;
                } else {
                    // Normal objects are positioned from top left.
                    y += oit.height / 2;
                }
                body_def.position = world->graphicsToPhysics(b2Vec2(oit.x + oit.width / 2, y));
                b2PolygonShape polygon_box; // this needs to be in scope long enough for it to be passed to the addObject function
                b2PolygonShape dynamic_box;
                b2Vec2 wh = world->graphicsToPhysics(0.5 * b2Vec2(oit.width, oit.height), 1);
                dynamic_box.SetAsBox(wh.x, wh.y);
                fixtures.push_back(b2FixtureDef());
                fixtures.back().shape = &dynamic_box;
                fixtures.back().density = 1.0f;
                fixtures.back().friction = 1.0f;
                if (type == GameObject::TRIGGER) {
                    fixtures.back().isSensor = true;
                } else if (type == GameObject::DOOR) {
                    fixtures.back().isSensor = true;
                    behavior = new Polarity::DoorBehavior();
                } else if (type == GameObject::PLAYER) {
                    behavior = new Polarity::CombiningBehavior(
                            new Polarity::PlayerBehavior(),
                            new Polarity::JumpingBehavior());
                    polygon_box.SetAsBox(0.25, 0.25, b2Vec2(0, -wh.y + 0.125), 0);
                    fixtures.push_back(b2FixtureDef());
                    fixtures.back().isSensor = true;
                    fixtures.back().userData = (void*)JumpingBehavior::FEET_FIXTURE;
                    fixtures.back().shape = &polygon_box;
                }

                world->addObject(behavior, body_def, fixtures, oit.name, type, oit.propertyMap);

                std::cerr << "object name = " << oit.name << std::endl;
            }
        }
        world->loaded = true;
    }

}
void World::load(const std::string &tmxFile) {
    std::cerr << "start async load " << tmxFile << std::endl;
    Polarity::asyncFileLoad(tmxFile, std::bind(&World::load_async, wthis, tmxFile, std::placeholders::_1, std::placeholders::_2));
}

World::~World() {
    std::cerr << "destructing world" << std::endl;
}
}
