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

World::World(const std::string& tmxFile, std::shared_ptr<AudioChannelPlayer> _audioPlayer)
        : physics(b2Vec2(0.0f, -10.0f)),
        graphicsScale(b2Vec2(96, 96)),
        camera(0, 300), //FIXME hard coded
        contactListener(this),
        screenDimensions(1, 1),
        keyState(SDLK_LAST),
        layers(nullptr),
        player(_audioPlayer) {
    std::cerr << "World has started"<<std::endl;
    for (int i=0; i< SDLK_LAST; ++i) {
      keyState[i] = false;
    }
    load(tmxFile);
    physics.SetContactListener(&contactListener);
}

void World::init(shared_ptr<AudioChannelPlayer> audioPlayer) {
    world = new World("assets/levels/level3.tmx", audioPlayer);
}

GameObject* World::addObject(Behavior *behavior, const b2BodyDef&bdef, const b2FixtureDef&fixture, const std::string &name, GameObject::Type type, const PropertyMap &properties) {
    GameObject * object = new GameObject(&physics, behavior, bdef, fixture, name, type, properties);
    objects.emplace_back(object);
    return objects.back().get();
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

void World::updateCamera(GameObject *obj, b2Vec2 player) {
    b2Vec2 delta = physicsToGraphics(player) - camera - 0.5f * screenDimensions;
    float len = delta.Length();
    float maxMovement = 1.0;
    if (len > maxMovement) {
        delta *= maxMovement / len;
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
