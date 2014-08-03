#include "world/world.hpp"
using namespace std;

namespace Polarity {
GameObject::Type GameObject::parseTypeStr(const std::string& typeStr) {
    if (typeStr == "start") {
      return PLAYER;
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
    currentAction = IDLE;
    groundBody = world->CreateBody(&bdef);
    groundBody->CreateFixture(&fixture);

    auto it = properties.find("animation");
    if (it != properties.end()) {
        idle = Animation::get("/" + it->second);
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
    b2Vec2 wh = 2. * world->physicsToGraphics(this->getBounds().GetExtents(), 1);
    b2Vec2 actualpos = world->physicsToGraphics(this->groundBody->GetPosition());
    actualpos -= world->getCamera();
    b2Vec2 drawpos = actualpos - 0.5 * wh;

    SDL_Rect rect;
    rect.x = drawpos.x;
    rect.y = drawpos.y;

    rect.w = wh.x;
    rect.h = wh.y;
    int r = type == TERRAIN ? 255 : 0;
    int g = type == DOOR || type == PLATFORM ? 255 : 0;
    int b = type == PLAYER || type == DOOR ? 255 : 0;
    // TODO: set the right color
    SDL_FillRect(screen, &rect, SDL_MapRGBA(screen->format, r, g, b, 70));

    auto actionAnimation = actions.find(currentAction);
    std::shared_ptr<Animation> actionAnim;
    if (actionAnimation != actions.end()) {
        actionAnim = actionAnimation->second;
    } else if (idle) {
        actionAnim = idle;
    }
    if (actionAnim) {
        actionAnim->draw(screen, actualpos.x - idle->width() / 2, actualpos.y - idle->height() / 2);
    }
}

float GameObject::printPosition(){
    return groundBody->GetPosition().y;
}
}
