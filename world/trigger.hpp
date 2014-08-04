#ifndef POLARITY_WORLD_TRIGGER_HPP__
#define POLARITY_WORLD_TRIGGER_HPP__

#include "physics/physics.hpp"

namespace Polarity {
class GameObject;

class Trigger {
protected:
    GameObject* owner;
public:
    Trigger();

    static Trigger* create(const std::string& name,
            const PropertyMap& properties=PropertyMap());

    GameObject* getOwner() { return owner; }
    virtual void setOwner(GameObject* owner);

    virtual ~Trigger() {}
    virtual void onBeginCollision(GameObject* other) {}
    virtual void onEndCollision(GameObject* other) {}
};

class DoorTrigger : public Trigger {
public:
    DoorTrigger();

    void onBeginCollision(GameObject* other);
    void onEndCollision(GameObject* other);
};

class FeetTrigger : public Trigger {
public:
    FeetTrigger();

    int numCollidingSurfaces;

    void onBeginCollision(GameObject* other);
    void onEndCollision(GameObject* other);
};

}

#endif
