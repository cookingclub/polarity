#include <iostream>
#include <iostream>
#include <libgen.h>
#include "SDL/SDL.h"
#include "world.hpp"
#include "graphics/graphics.hpp"

#include "tmxparser.h"

using std::shared_ptr;

namespace Polarity {
World *world = nullptr;

World::World(const std::string& tmxFile, std::shared_ptr<AudioChannelPlayer> _audioPlayer,
            shared_ptr<PlayerState> _playerState, shared_ptr<GameState> _gameState)
        : physics(b2Vec2(0.0f, -10.0f)),
        graphicsScale(b2Vec2(96, 96)),
        camera(0, 300), //FIXME hard coded
        contactListener(this),
        screenDimensions(1, 1),
        keyState(SDLK_LAST),
        keyPressedThisTick(SDLK_LAST, false),
        layers(nullptr),
        fAudioPlayer(_audioPlayer),
        fPlayerState(_playerState),
        fGameState(_gameState) {
    std::cerr << "World has started"<<std::endl;
    for (int i=0; i< SDLK_LAST; ++i) {
      keyState[i] = false;
    }
    load(tmxFile);
    physics.SetContactListener(&contactListener);
}

void World::init(shared_ptr<AudioChannelPlayer> audioPlayer, shared_ptr<PlayerState> playerState, shared_ptr<GameState> gameState) {
    world = new World("assets/levels/level3.tmx", audioPlayer, playerState, gameState);

    // based on current game state, start music and sound effects
    world->fAudioPlayer->playChannel("white-music");
    world->fAudioPlayer->playChannel("black-music");
    if (world->playerState()->color == Polarity::PlayerColor::WHITE) {
        world->audio()->setChannelVolume("white-music", 1.0);
        world->audio()->setChannelVolume("black-music", 0.0);
    } else {
        world->audio()->setChannelVolume("white-music", 0.0);
        world->audio()->setChannelVolume("black-music", 1.0);
    }
}

GameObject* World::addObject(Behavior *behavior, const b2BodyDef&bdef, const b2FixtureDef&fixture, const std::string &name, GameObject::Type type, const PropertyMap &properties) {
    GameObject * object = new GameObject(&physics, behavior, bdef, fixture, name, type, properties);
    objects.emplace_back(object);
    return objects.back().get();
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
    float maxMovementX = 32.0;
    float maxMovementY = 1.0;
    if (lenY > maxMovementY) {
        delta.y *= maxMovementY / lenY;
    }
    if (lenX > maxMovementX) {
        delta.x *= maxMovementX / lenX;
    }
    camera += delta;
}

void World::tick() {
    if (keyState['a']) {
        camera.x -=1;
    }
    if (keyState['d']) {
        camera.x +=1;
    }
    if (keyState['w']) {
        camera.y +=1;
    }
    if (keyState['s']) {
        camera.y -=1;
    }
    // timeStep, velocityIterations, positionIterations
    physics.Step(0.0166666, 8, 4);
    for (auto &obj : objects) {
        obj->tick(this);
        //std::cerr << obj->printPosition()<<std::endl;
    }
    //for(auto &gameObject:objects){
    //  
    //}
    
    // Gets called every frame
}

void World::draw(SDL_Surface *screen) {
    for (auto& layer : layers->layers) {
        layer->draw(screen, -camera.x, -camera.y);
    }
    for (auto& object : objects) {
        object->draw(this, screen);
    }
    screenDimensions.x = screen->w;
    screenDimensions.y = screen->h;
}

void World::load(const std::string &tmxFile) {
    char *filecpy = new char[tmxFile.length() + 1];
    strncpy(filecpy, tmxFile.c_str(), tmxFile.length() + 1);
    std::string dir (dirname(filecpy));
    delete []filecpy;

    std::cerr << "Loading: " << tmxFile << " from directory " << dir << std::endl;
    tmxparser::TmxMap map;
    tmxparser::TmxReturn error = tmxparser::parseFromFile(tmxFile, &map);

    layers = std::unique_ptr<LayerCollection>(new LayerCollection(dir, map));
    for (auto &it : map.tilesetCollection) {
        std::cerr << "Found tileset: " << it.name << std::endl;
    }
    for (auto &it : map.layerCollection) {
        std::cerr << "Found layer: " << it.name << std::endl;
    }
    for (auto &it : map.objectGroupCollection) {
        std::cerr << "Found object group: " << it.name << std::endl;
        for (auto &oit : it.objects) {
            b2BodyDef body_def;
            GameObject::Type type = GameObject::parseTypeStr(oit.type);
            Behavior *behavior = NULL;
            if (type == GameObject::PLAYER) {
                behavior = new Polarity::KeyboardBehavior();
                std::cerr<<"Making dynamic "<<std::endl;
                body_def.type = b2_dynamicBody;
            } else {
                behavior = new Polarity::Behavior();
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
            body_def.position = graphicsToPhysics(b2Vec2(oit.x + oit.width / 2, y));
            b2PolygonShape dynamic_box;
            b2Vec2 wh = graphicsToPhysics(0.5 * b2Vec2(oit.width, oit.height), 1);
            dynamic_box.SetAsBox(wh.x, wh.y);
            b2FixtureDef fixture_def;
            fixture_def.shape = &dynamic_box;
            fixture_def.density = 1.0f;
            fixture_def.friction = 1.0f;
            GameObject* game_obj = addObject(behavior, body_def, fixture_def, oit.name, type, oit.propertyMap);

            std::cerr << "object name = " << oit.name << std::endl;
        }
    }
}
}
