#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_rotozoom.h"
#include <iostream>
#include "behavior.hpp"
#include "world/world.hpp"

namespace Polarity {
void KeyboardBehavior::tick(World *world, GameObject *obj) {
    bool left = world->isKeyDown(SDLK_LEFT);
    bool right = world->isKeyDown(SDLK_RIGHT);
    bool jump = world->isKeyDown(SDLK_UP);
    b2Body *phyobj = obj->groundBody;
    if (left) {
        phyobj->ApplyForce( b2Vec2(-100,0), phyobj->GetWorldCenter(), true);
        std::cerr << "going left" << std::endl;
        world->audio()->playChannel("step-stone", -1);
    }
    if (jump) {
        phyobj->ApplyForce( b2Vec2(0,500), phyobj->GetWorldCenter(), true);
        std::cerr << "jumping" << std::endl;
    }
    if (right) {
        phyobj->ApplyForce( b2Vec2(100,0), phyobj->GetWorldCenter(), true);
        std::cerr << "going right" << std::endl;
        world->audio()->playChannel("step-stone", -1);
    }

    // stop sounds if we're not walking anymore
    if (!right && !left) {
        world->audio()->stopChannel("step-stone");
    }
}
}
