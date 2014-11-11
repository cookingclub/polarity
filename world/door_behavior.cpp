#include "world/door_behavior.hpp"
#include "world/world.hpp"
#include "physics/game_object.hpp"

namespace Polarity {

void DoorBehavior::onBeginCollision(World *world, GameObject *self, b2Contact* contact) {
    if (self->remoteObject(contact)->getType() == GameObject::PLAYER) {
        self->setAction(GameObject::OPEN);
        World::switchToWorld(newWorld);
        //other->groundBody->SetGravityScale(0);
        //other->groundBody->SetLinearVelocity(
        //        (other->groundBody->GetPosition() - owner->groundBody->GetPosition()));
        std::cerr<<"We have CONTACT!"<< std::endl;
    }
}

void DoorBehavior::onEndCollision(World *world, GameObject *self, b2Contact* contact) {
    if (self->remoteObject(contact)->getType() == GameObject::PLAYER) {
        self->setAction(GameObject::IDLE);
    }
}

void DoorBehavior::tick(World *world, GameObject *self) {
    if (!newWorld) {
        std::string dest = self->getProperty("level");
        if (!dest.empty()) {
            newWorld = world->loadNewWorld(dest);
        }
    }
}

}
