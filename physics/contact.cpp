#include <algorithm>
#include <assert.h>

#include "physics/contact.hpp"
#include "physics/physics.hpp"
#include "world/world.hpp"

namespace Polarity {

ContactListener::ContactListener(World* world) {
}

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    GameObject* objectA = static_cast<GameObject*>(contact->GetFixtureA()->GetBody()->GetUserData());
    GameObject* objectB = static_cast<GameObject*>(contact->GetFixtureB()->GetBody()->GetUserData());
    assert(objectA && objectB);
    if (!objectB || !objectA) {
        std::cerr << "NULLITY!!!" << std::endl;
        return;
    }
    if (objectB->getType() == GameObject::PLAYER) {
        std::swap(objectA, objectB);
    }
    // std::cerr << "Contact begin " << objectA->getName() << " with " << objectB->getName() << std::endl;
    // Now, objectA will be player or nothing; objectB is never a player
    if (objectA->getType() == GameObject::PLAYER) {
        if (objectB->getType() == GameObject::DOOR) {
            contact->SetEnabled(false);
            objectA->groundBody->SetGravityScale(0);
            objectA->groundBody->SetLinearVelocity(
                    (objectB->groundBody->GetPosition() - objectA->groundBody->GetPosition()));
            // std::cerr<<"We have CONTACT!"<< std::endl;
        }
    }
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
}

void ContactListener::BeginContact(b2Contact* contact) {
}

void ContactListener::EndContact(b2Contact* contact) {
}

}
