#ifndef _BEHAVIOR_HPP_
#define _BEHAVIOR_HPP_

#include <memory>

class b2Contact;

namespace Polarity {
class GameObject;
class World;

class Behavior {
public:
    Behavior() {
    }
    Behavior(const Behavior&x) = delete;
    Behavior& operator=(const Behavior&x) = delete;
    virtual ~Behavior() {
    }

    virtual void addedToWorld(World *world, GameObject *self) {
    }

    virtual void tick(World *world, GameObject *self){
    }

    virtual void onBeginCollision(World *world, GameObject *self,
            b2Contact *contact) {
    }

    virtual void onEndCollision(World *world, GameObject *self,
            b2Contact *contact) {
    }
};

class CombiningBehavior : public Behavior {
    std::unique_ptr<Behavior> first;
    std::unique_ptr<Behavior> second;
public:
    CombiningBehavior(Behavior* first, Behavior *second)
        : first(first), second(second) {
    }

    virtual void addedToWorld(World *world, GameObject *self) {
        first->addedToWorld(world, self);
        second->addedToWorld(world, self);
    }

    virtual void tick(World *world, GameObject *self){
        first->tick(world, self);
        second->tick(world, self);
    }

    virtual void onBeginCollision(World *world, GameObject *self,
            b2Contact *contact) {
        first->onBeginCollision(world, self, contact);
        second->onBeginCollision(world, self, contact);
    }

    virtual void onEndCollision(World *world, GameObject *self,
            b2Contact *contact) {
        first->onEndCollision(world, self, contact);
        second->onEndCollision(world, self, contact);
    }
};

}
#endif
