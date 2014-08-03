#include "world/world.hpp"
using namespace std;

namespace Polarity {
GameObject::Type GameObject::parseTypeStr(const std::string& typeStr) {
    if (typeStr == "start") {
      return START;
    } else if (typeStr == "door") {
      return DOOR;
    } else if (typeStr == "platform") {
      return PLATFORM;
    } else if (!typeStr.empty()) {
      std::cerr << "Error: unknown type " << typeStr;
    }
    return TERRAIN;
}

GameObject::GameObject(b2World *world, Behavior *behavior, const b2BodyDef &bdef, const b2FixtureDef &fixture, const std::string &name, Type type, const PropertyMap &props)
        : behavior(behavior), name(name), properties(props), type(type) {
    groundBody = world->CreateBody(&bdef);
    groundBody->CreateFixture(&fixture);

    auto it = properties.find("animation");
    if (it != properties.end()) {
        anim = Animation::get("/" + it->second);
    }
}

void GameObject::tick(World *world) {
    behavior->tick(world, this);
}

b2AABB GameObject::getBounds() const{
    b2Transform transform;
    transform.SetIdentity();
    b2AABB aabb;
    aabb.lowerBound = b2Vec2(FLT_MAX,FLT_MAX);
    aabb.upperBound = b2Vec2(-FLT_MAX,-FLT_MAX); 
    b2Fixture* fixture = groundBody->GetFixtureList();

    while (fixture != nullptr) {
        const b2Shape *shape = fixture->GetShape();
        const int childCount = shape->GetChildCount();
        for (int child = 0; child < childCount; ++child) {
            const b2Vec2 r(shape->m_radius, shape->m_radius);
            b2AABB shapeAABB;
            shape->ComputeAABB(&shapeAABB, transform, child);
            shapeAABB.lowerBound = shapeAABB.lowerBound + r;
            shapeAABB.upperBound = shapeAABB.upperBound - r;
            aabb.Combine(shapeAABB);
        }
        fixture = fixture->GetNext();
    }
    return aabb;
}
void GameObject::draw(World * world, SDL_Surface* screen) {
    auto drawpos = this->groundBody->GetPosition() - world->getCamera();
    b2Vec2 wh = this->getBounds().GetExtents();
    SDL_Rect rect;
    rect.x = drawpos.x;
    rect.y = drawpos.y;

    rect.w = wh.x;
    rect.h = wh.y;
    int r = type == TERRAIN ? 255 : 0;
    int g = type == DOOR || type == PLATFORM ? 255 : 0;
    int b = type == START || type == DOOR ? 255 : 0;
    // TODO: set the right color
    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, r, g, b));

    if (anim) {
        anim->draw(screen, drawpos.x, drawpos.y);
    }
}

float GameObject::printPosition(){
    return groundBody->GetPosition().y;
}
}
