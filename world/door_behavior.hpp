#ifndef POLARITY_WORLD_DOOR_BEHAVIOR_HPP__
#define POLARITY_WORLD_DOOR_BEHAVIOR_HPP__

#include "physics/behavior.hpp"

namespace Polarity {

class DoorBehavior : public Behavior {

    std::shared_ptr<World> newWorld;
    int ticksToNewWorld;

    void onBeginCollision(World *world, GameObject *self, b2Contact* contact) override;
    void onEndCollision(World *world, GameObject *self, b2Contact* contact) override;

    void tick(World *world, GameObject *self) override;
};

}

#endif

