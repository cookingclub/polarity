#ifndef POLARITY_PLAYER_BEHAVIOR_HPP_
#define POLARITY_PLAYER_BEHAVIOR_HPP_

#include "behavior.hpp"

namespace Polarity {

class PlayerBehavior : public Behavior{
public:
    virtual void addedToWorld(World * world, GameObject *obj);
    virtual void tick(World * world, GameObject *obj);

    void handleMusicByPlayerColor(World *world, GameObject *obj);
};

}
#endif
