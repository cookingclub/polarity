#ifndef POLARITY_PHYSICS_MEGNETIC_BEHAVIOR_HPP__
#define POLARITY_PHYSICS_MEGNETIC_BEHAVIOR_HPP__

#include "world/world.hpp"
#include "physics/behavior.hpp"
#include "physics/game_object.hpp"

namespace Polarity {

class MagneticBehavior : public Behavior {
public:
    void tick(World * world, GameObject* obj) override {
        if (obj == world->player) {
            return;
        }
        //logic for this 
        b2Vec2 pos = world->player->groundBody->GetPosition();
        b2Vec2 posK = obj->groundBody->GetPosition();

        float r1[] = {pos.x-posK.x, pos.y-posK.y};
        float r1m = sqrtf(r1[0]*r1[0]+r1[1]*r1[1]);

        // apply force
        float forceMult = GameObject::polarityForceMultiplier(
            world->player->getPolarity(),
            obj->getPolarity());

        b2Body *player1 = world->player->groundBody;
        if (r1m != 0 && r1m < 20) {
            float forcemag = 15/r1m/r1m*forceMult;
            player1->ApplyForce(b2Vec2(forcemag*r1[0]/r1m, forcemag*r1[1]/r1m),
                                            player1->GetWorldPoint( b2Vec2(0,-1) ) , true);
        }
    }
};

}

#endif
