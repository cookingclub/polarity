#ifndef _JUMPING_BEHAVIOR_HPP_
#define _JUMPING_BEHAVIOR_HPP_

#include "physics/behavior.hpp"

namespace Polarity {

class JumpingBehavior : public Behavior {

    int numCollidingSurfaces;

public:
    // Assign this to the user data of the feet fixture
    static const char *FEET_FIXTURE;

    JumpingBehavior();

    void onBeginCollision(World *world, GameObject *self, b2Contact *contact) override;

    void onEndCollision(World *world, GameObject *self, b2Contact *contact) override;
};

}

#endif
