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
        obj->setAction(GameObject::WALK);
        phyobj->ApplyForce( b2Vec2(-100,0), phyobj->GetWorldCenter(), true);
        world->audio()->playChannel("step-stone", -1);
    }
    if (jump) {
        phyobj->ApplyForce( b2Vec2(0,50), phyobj->GetWorldCenter(), true);
        world->audio()->playChannel("jump", 0);
    }
    if (right) {
        obj->setAction(GameObject::WALK);
        phyobj->ApplyForce( b2Vec2(100,0), phyobj->GetWorldCenter(), true);
        world->audio()->playChannel("step-stone", -1);
    }
    if (phyobj->GetLinearVelocity().LengthSquared() < .1) { // and contact
        obj->setAction(GameObject::IDLE);
    }

    // stop sounds if we're not walking anymore
    if (!right && !left) {
        world->audio()->stopChannel("step-stone");
    }
    if (!jump) {
        world->audio()->stopChannel("jump");
    }
    world->updateCamera(obj, phyobj->GetWorldCenter());
}
}
