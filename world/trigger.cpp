#include "world/trigger.hpp"
#include "world/world.hpp"
#include "physics/physics.hpp"

namespace Polarity {
Trigger::Trigger()
    : owner(nullptr) {
}

void Trigger::setOwner(GameObject* newOwner) {
    owner = newOwner;
}

Trigger* Trigger::create(const std::string& name,
        const PropertyMap& properties) {
    if (name == "door") {
        return new DoorTrigger();
    } else if (name == "feet") {
        return new FeetTrigger();
    }
    return nullptr;
}

DoorTrigger::DoorTrigger() {
}

void DoorTrigger::onBeginCollision(GameObject* other) {
    if (other->getType() == GameObject::PLAYER) {
        other->groundBody->SetGravityScale(0);
        //other->groundBody->SetLinearVelocity(
        //        (other->groundBody->GetPosition() - owner->groundBody->GetPosition()));
        std::cerr<<"We have CONTACT!"<< std::endl;
    }
}

FeetTrigger::FeetTrigger()
    : numCollidingSurfaces(0) {
}

void FeetTrigger::onBeginCollision(GameObject* other) {
    if (numCollidingSurfaces == 0) {
        owner->setJumpable(true);
    }
    if (other->isCollidable()) {
        numCollidingSurfaces++;
    }
}

void FeetTrigger::onEndCollision(GameObject* other) {
    if (other->isCollidable()) {
        numCollidingSurfaces--;
    }
    if (numCollidingSurfaces == 0) {
        owner->setJumpable(false);
    }
}

}
