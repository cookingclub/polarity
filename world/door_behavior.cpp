#include "world/door_behavior.hpp"
#include "world/world.hpp"
#include "physics/game_object.hpp"

#ifdef USE_SDL2
#include "SDL2/SDL.h" // SDL_GetTicks()
#else
#include "SDL/SDL.h" // SDL_GetTicks()
#endif

namespace Polarity {

void DoorBehavior::onBeginCollision(World *world, GameObject *self, b2Contact* contact) {
    GameObject *other = self->remoteObject(contact);
    if (other->getType() == GameObject::PLAYER) {
        ticksToNewWorld = (int) SDL_GetTicks() + 800;
        self->setAction(GameObject::OPEN);
        other->setAction(GameObject::OPEN);
        world->setBackgroundWorld(newWorld);
        //other->groundBody->SetGravityScale(0);
        //other->groundBody->SetLinearVelocity(
        //        (other->groundBody->GetPosition() - owner->groundBody->GetPosition()));
        std::cerr<<"We have CONTACT!"<< std::endl;
    }
}

void DoorBehavior::onEndCollision(World *world, GameObject *self, b2Contact* contact) {
}

void DoorBehavior::tick(World *world, GameObject *self) {
    if (!newWorld) {
        std::string dest = self->getProperty("level");
        if (!dest.empty()) {
            newWorld = world->loadNewWorld(dest);
        }
    }
    if (self->getAction() == GameObject::OPEN) {
        if ((int)SDL_GetTicks() > ticksToNewWorld && newWorld->isLoaded()) {
            world->switchToBackgroundWorld();
            self->setAction(GameObject::IDLE);
        }
    }
}

}
