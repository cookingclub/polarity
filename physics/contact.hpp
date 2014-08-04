#ifndef POLARITY_PHYSICS_CONTACT_HPP__
#define POLARITY_PHYSICS_CONTACT_HPP__

#include <Box2D/Box2D.h>

namespace Polarity {
class World;

class ContactListener : public b2ContactListener {

    World *world;
public:
    ContactListener(World* world);

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

    void BeginContact(b2Contact* contact);

    void EndContact(b2Contact* contact);
};
}

#endif
