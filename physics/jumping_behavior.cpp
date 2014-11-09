#include "world/world.hpp"
#include "physics/game_object.hpp"
#include "physics/jumping_behavior.hpp"

namespace Polarity {

const char *JumpingBehavior::FEET_FIXTURE = "feet";

JumpingBehavior::JumpingBehavior()
    : numCollidingSurfaces(0) {
}

void JumpingBehavior::onBeginCollision(World *world, GameObject *self, b2Contact *contact) {
    const char *userData = static_cast<const char *>(
            self->localFixture(contact)->GetUserData());
    if (!userData || std::strcmp(userData, FEET_FIXTURE)) {
        return;
    }
    if (numCollidingSurfaces == 0) {
        self->setJumpable(true);
    }
    numCollidingSurfaces++;
}

void JumpingBehavior::onEndCollision(World *world, GameObject *self, b2Contact *contact) {
    const char *userData = static_cast<const char *>(
            self->localFixture(contact)->GetUserData());
    if (!userData || std::strcmp(userData, FEET_FIXTURE)) {
        return;
    }
    numCollidingSurfaces--;
    if (numCollidingSurfaces == 0) {
        self->setJumpable(false);
    }
}

}
