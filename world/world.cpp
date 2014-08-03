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
    world = new World("assets/levels/level1.tmx");
}

GameObject* World::addObject(Behavior *behavior, const b2BodyDef&bdef) {
    GameObject * object = new GameObject(&physics, behavior, bdef);
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
    physics.Step(0.0166666, 1, 1);

    for (auto &obj : objects) {
      obj->tick(this);
      std::cerr << obj->printPosition()<<std::endl;
    }
    //for(auto &gameObject:objects){
    //  
    //}
    
    // Gets called every frame
}

void World::load(const std::string &tmxFile) {
    char *filecpy = new char[tmxFile.length() + 1];
    strncpy(filecpy, tmxFile.c_str(), tmxFile.length() + 1);
    std::string dir (dirname(filecpy));
    delete []filecpy;

    std::cerr << "Loading: " << tmxFile << " from directory " << dir << std::endl;
    tmxparser::TmxMap map;
    tmxparser::TmxReturn error = tmxparser::parseFromFile(tmxFile, &map);

    layers = std::unique_ptr<LayerCollection>(new LayerCollection(
                dir,
                map.tilesetCollection, map.layerCollection));

    for (auto &it : map.tilesetCollection) {
        std::cerr << "Found tileset: " << it.name << std::endl;
    }
    for (auto &it : map.layerCollection) {
        std::cerr << "Found layer: " << it.name << std::endl;
    }
    for (auto &it : map.objectGroupCollection) {
        std::cerr << "Found object group: " << it.name << std::endl;

    }
}
}
