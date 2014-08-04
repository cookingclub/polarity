#include "world/trigger.hpp"
#include "world/world.hpp"
#include "physics/physics.hpp"

namespace Polarity {
Trigger* Trigger::create(const std::string& name,
        const PropertyMap& properties) {
    if (name == "door") {
        return new DoorTrigger;
    }
    return nullptr;
}

void DoorTrigger::onBeginCollision(GameObject* other) {
    if (other->getType() == GameObject::PLAYER) {
        other->groundBody->SetGravityScale(0);
        //other->groundBody->SetLinearVelocity(
        //        (other->groundBody->GetPosition() - owner->groundBody->GetPosition()));
        std::cerr<<"We have CONTACT!"<< std::endl;
    }
}
}
