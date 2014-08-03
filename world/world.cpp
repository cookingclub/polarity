#include <iostream>
#include <iostream>
#include <libgen.h>
#include "SDL/SDL.h"
#include "world.hpp"
#include "graphics/graphics.hpp"

#include "tmxparser.h"


namespace Polarity {
World *world = nullptr;

World::World(const std::string& tmxFile)
        : physics(b2Vec2(0.0f, -10.0f)),
        keyState(SDLK_LAST),
        layers(nullptr) {
    std::cerr << "World has started"<<std::endl;
    for (int i=0; i< SDLK_LAST; ++i) {
      keyState[i] = false;
    }
    load(tmxFile);
}

void World::init() {
    world = new World("assets/levels/level3.tmx");
}

  GameObject* World::addObject(Behavior *behavior, const b2BodyDef&bdef, const b2FixtureDef&fixture) {
    GameObject * object = new GameObject(&physics, behavior, bdef, fixture);
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

void World::tick() {
    // timeStep, velocityIterations, positionIterations
    physics.Step(0.0166666, 1, 1);
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
        layer->draw(screen, -300, -300);
    }
    for (auto& object : objects) {
        object->draw(screen);
    }
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
            body_def.type = b2_dynamicBody;
            body_def.position.Set(oit.x, oit.y);
            b2PolygonShape dynamic_box;
            dynamic_box.SetAsBox(oit.width, oit.height);
            b2FixtureDef fixture_def;
            fixture_def.shape = &dynamic_box;
            fixture_def.density = 1.0f;
            fixture_def.friction = 0.3f;
            GameObject* game_obj = addObject(new Polarity::KeyboardBehavior(), body_def, fixture_def);

            std::cerr << "object name = " << oit.name << std::endl;
        }
    }
}
}
