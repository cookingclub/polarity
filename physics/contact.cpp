#include <algorithm>
#include <assert.h>

#include "physics/contact.hpp"
#include "physics/game_object.hpp"
#include "world/world.hpp"

namespace Polarity {

ContactListener::ContactListener(World* world)
    : world(world) {
}

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    // Can call contact->SetEnabled(false); to disable some collisions.
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
}

void ContactListener::BeginContact(b2Contact* contact) {
    GameObject* objectA = GameObject::getContactObjectA(contact);
    GameObject* objectB = GameObject::getContactObjectB(contact);
    // b2Body's must always be tied to a GameObject.
    assert(objectA && objectB);

    bool sensorA = contact->GetFixtureA()->IsSensor();
    bool sensorB = contact->GetFixtureB()->IsSensor();

    if (objectA && !sensorB) {
        objectA->onBeginCollision(world, contact);
    }
    if (objectB && !sensorA) {
        objectB->onBeginCollision(world, contact);
    }
}

void ContactListener::EndContact(b2Contact* contact) {
    GameObject* objectA = GameObject::getContactObjectA(contact);
    GameObject* objectB = GameObject::getContactObjectB(contact);
    // b2Body's must always be tied to a GameObject.
    assert(objectA && objectB);

    bool sensorA = contact->GetFixtureA()->IsSensor();
    bool sensorB = contact->GetFixtureB()->IsSensor();

    if (!sensorB) {
        objectA->onEndCollision(world, contact);
    }
    if (!sensorA) {
        objectB->onEndCollision(world, contact);
    }
}

}
