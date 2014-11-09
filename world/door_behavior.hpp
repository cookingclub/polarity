#ifndef POLARITY_WORLD_DOOR_BEHAVIOR_HPP__
#define POLARITY_WORLD_DOOR_BEHAVIOR_HPP__

#include "physics/behavior.hpp"

namespace Polarity {

class DoorBehavior : public Behavior {
    void onBeginCollision(World *world, GameObject *self, b2Contact* contact) override;
    void onEndCollision(World *world, GameObject *self, b2Contact* contact) override;
public:
};

}

#endif

