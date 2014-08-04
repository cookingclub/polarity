#ifndef POLARITY_WORLD_TRIGGER_HPP__
#define POLARITY_WORLD_TRIGGER_HPP__

#include "physics/physics.hpp"

namespace Polarity {
class GameObject;

class Trigger {
protected:
    GameObject* owner;
public:
    static Trigger* create(const std::string& name,
            const PropertyMap& properties);

    virtual ~Trigger() {}
    virtual void onBeginCollision(GameObject* other) {}
    virtual void onEndCollision(GameObject* other) {}
};

class DoorTrigger : public Trigger {
    void onBeginCollision(GameObject* other);
};

}

#endif
