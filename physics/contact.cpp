#include <algorithm>
#include <assert.h>

#include "physics/contact.hpp"
#include "physics/game_object.hpp"
#include "world/trigger.hpp"
#include "world/world.hpp"

namespace Polarity {

ContactListener::ContactListener(World* world) {
}

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    GameObject* objectA = static_cast<GameObject*>(contact->GetFixtureA()->GetBody()->GetUserData());
    GameObject* objectB = static_cast<GameObject*>(contact->GetFixtureB()->GetBody()->GetUserData());
    if (!objectB || !objectA) {
        return;
    }
    if (!objectA->isCollidable() || !objectB->isCollidable()) {
        contact->SetEnabled(false);
    }
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
}

void ContactListener::BeginContact(b2Contact* contact) {
    GameObject* objectA = static_cast<GameObject*>(contact->GetFixtureA()->GetBody()->GetUserData());
    GameObject* objectB = static_cast<GameObject*>(contact->GetFixtureB()->GetBody()->GetUserData());
    Trigger* triggerA = static_cast<Trigger*>(contact->GetFixtureA()->GetUserData());
    Trigger* triggerB = static_cast<Trigger*>(contact->GetFixtureB()->GetUserData());
    bool sensorA = contact->GetFixtureA()->IsSensor();
    bool sensorB = contact->GetFixtureB()->IsSensor();
    if (!objectB || !objectA) {
        return;
    }
    if (triggerA && !sensorB) {
        triggerA->onBeginCollision(objectB);
    }
    if (triggerB && !sensorA) {
        triggerB->onBeginCollision(objectA);
    }
}

void ContactListener::EndContact(b2Contact* contact) {
    GameObject* objectA = static_cast<GameObject*>(contact->GetFixtureA()->GetBody()->GetUserData());
    GameObject* objectB = static_cast<GameObject*>(contact->GetFixtureB()->GetBody()->GetUserData());
    Trigger* triggerA = static_cast<Trigger*>(contact->GetFixtureA()->GetUserData());
    Trigger* triggerB = static_cast<Trigger*>(contact->GetFixtureB()->GetUserData());
    bool sensorA = contact->GetFixtureA()->IsSensor();
    bool sensorB = contact->GetFixtureB()->IsSensor();
    if (!objectB || !objectA) {
        return;
    }
    if (triggerA && !sensorB) {
        triggerA->onEndCollision(objectB);
    }
    if (triggerB && !sensorA) {
        triggerB->onEndCollision(objectA);
    }
}

}
