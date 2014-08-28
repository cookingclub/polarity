#ifndef _BEHAVIOR_HPP_
#define _BEHAVIOR_HPP_
#include <memory>
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

    virtual void addedToWorld(World *world, GameObject *obj) {
    }

    virtual void tick(World *world, GameObject *obj){
    }
};

class CombiningBehavior : public Behavior {
    std::unique_ptr<Behavior> first;
    std::unique_ptr<Behavior> second;
public:
    CombiningBehavior(Behavior* first, Behavior *second)
        : first(first), second(second) {
    }

    virtual void addedToWorld(World *world, GameObject *obj) {
        first->addedToWorld(world, obj);
        second->addedToWorld(world, obj);
    }

    virtual void tick(World *world, GameObject *obj){
        first->tick(world, obj);
        second->tick(world, obj);
    }
};

}
#endif
