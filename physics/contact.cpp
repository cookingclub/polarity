#include <algorithm>
#include <assert.h>

#include "physics/contact.hpp"
#include "physics/physics.hpp"
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
    if (!objectB || !objectA) {
        return;
    }
    if (triggerA) {
        triggerA->onBeginCollision(objectB);
    }
    if (triggerB) {
        triggerB->onBeginCollision(objectA);
    }
}

void ContactListener::EndContact(b2Contact* contact) {
    GameObject* objectA = static_cast<GameObject*>(contact->GetFixtureA()->GetBody()->GetUserData());
    GameObject* objectB = static_cast<GameObject*>(contact->GetFixtureB()->GetBody()->GetUserData());
    Trigger* triggerA = static_cast<Trigger*>(contact->GetFixtureA()->GetUserData());
    Trigger* triggerB = static_cast<Trigger*>(contact->GetFixtureB()->GetUserData());
    if (!objectB || !objectA) {
        return;
    }
    if (triggerA) {
        triggerA->onEndCollision(objectB);
    }
    if (triggerB) {
        triggerB->onEndCollision(objectA);
    }
}

}
