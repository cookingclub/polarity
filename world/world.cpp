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
      // std::cerr << obj->printPosition()<<std::endl;
    }
    //for(auto &gameObject:objects){
    //  
    //}
    
    // Gets called every frame
}

void World::draw(SDL_Surface *screen) {
    for (auto& layer : layers->layers) {
        layer->draw(screen, 0, 0);
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
    // std::string name;
    // std::string color;
    // float opacity;
    // bool visible;
    // TmxPropertyMap_t propertyMap;
    // TmxObjectCollection_t objects;

        // typedef std::vector<TmxObject> TmxObjectCollection_t;


        std::cerr << "Found object group (name): " << it.name << std::endl;
        std::cerr << "Found object group (color): " << it.color << std::endl;
        std::cerr << "Found object group (opacity): " << it.opacity << std::endl;
        std::cerr << "Found object group (visible): " << it.visible << std::endl;
        std::cerr << "Found object group (propertyMap): " << it.propertyMap.size() << std::endl;
        for (auto &pit : it.propertyMap) {
            std::cerr << "meow " << pit.first << std::endl;
        }
        std::cerr << "Found object group (objects): " << it.objects.size() << std::endl;
        for (auto &oit : it.objects) {
            SDL_Surface* surface = SDL_CreateRGBSurface(0, oit.width, oit.height, 32, 0, 0, 0, 0);
            if(surface == NULL) {
                fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
                exit(1);
            }
            int err = SDL_FillRect(surface, NULL, 100);
            std::cerr << "error code = " <<  err << std::endl;

            std::cerr << "object name = " << oit.name << std::endl;
            std::cerr << "object type = " << oit.type << std::endl;
            std::cerr << "object x = " << oit.x << std::endl;
            std::cerr << "object y = " << oit.y << std::endl;
            std::cerr << "object width = " << oit.width << std::endl;
            std::cerr << "object height = " << oit.height << std::endl;
            std::cerr << "object rotation = " << oit.rotation << std::endl;
            std::cerr << "object referenceGid = " << oit.referenceGid << std::endl;
            std::cerr << "object visible = " << oit.visible << std::endl;
            std::cerr << "object propertyMap = " << oit.propertyMap.size() << std::endl;
            std::cerr << "object shapeType = " << oit.shapeType << std::endl;
            std::cerr << "object shapePoints = " << oit.shapePoints.size() << std::endl;
        }
// typedef struct
// {
//     std::string name;
//     std::string type;
//     int x;
//     int y;
//     unsigned int width;
//     unsigned int height;
//     float rotation;
//     unsigned int referenceGid;
//     bool visible;
//     TmxPropertyMap_t propertyMap;
//     TmxShapeType shapeType;
//     TmxShapePointCollection_t shapePoints;
// } TmxObject;



    }
}
}
