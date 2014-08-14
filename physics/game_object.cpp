#include "world/world.hpp"
#include "world/trigger.hpp"
#include "graphics/animation.hpp"

using namespace std;

namespace Polarity {
GameObject::Type GameObject::parseTypeStr(const std::string& typeStr) {
    if (typeStr == "start") {
      return PLAYER;
    } else if (typeStr == "trigger") {
      return TRIGGER;
    } else if (typeStr == "door") {
      return DOOR;
    } else if (typeStr == "platform") {
      return PLATFORM;
    } else if (!typeStr.empty()) {
      std::cerr << "Error: unknown type " << typeStr;
    }
    return TERRAIN;
}

GameObject::GameObject(b2World *world, Behavior *behavior, const b2BodyDef &bdef, const std::vector<b2FixtureDef> &fixtures, const std::string &name, Type type, const PropertyMap &props)
        : behavior(behavior), name(name), properties(props), type(type) {
    currentAction = IDLE;
    groundBody = world->CreateBody(&bdef);
    groundBody->SetUserData(this);
    for (auto &fixture : fixtures) {
        Trigger* trig = static_cast<Trigger*const>(fixture.userData);
        if (trig) {
            trig->setOwner(this);
        }
        groundBody->CreateFixture(&fixture);
    }
    jumpCooldown = -1;
    collidable = true; // deprecated: use b2FixtureDef::isSensor

    idle = nullptr;

	polarityCharge = COLORLESS;
	auto it=properties.find("color");
    if (it != properties.end()){
        if (it -> second == "black") {
            polarityCharge = BLACK;
        }
        else if (it->second == "white") {
            polarityCharge = WHITE;
        }
	}

    for(int i=0;i<NUM_ACTIONS;++i) {
        Actions act = (Actions)i;
        std::string stringName;
        switch (act) {
        case IDLE: stringName = "idle";
            break;
        case WALK: stringName = "walk";
            break;
        case RUN: stringName = "run";
            break;
        case JUMP: stringName = "jump";
            break;
        case OPEN: stringName = "open";
            break;
        default:
            break;
        }
        if (stringName.length()) {
            auto it = properties.find(stringName);
            if (it != properties.end()) {
                actionsAnimation[act] = Animation::get(it->second);
                if (act == OPEN) {
                    actionsAnimation[act]->setLoop(false);
                }
            }
        }
    }
    auto animit = actionsAnimation.find(IDLE);
    if (animit != actionsAnimation.end()) {
        idle = animit->second;
    }
}

void GameObject::setAction(Actions a) {
    if (currentAction != a) {
        currentAction = a;
        actionsAnimation[a]->restart();
    }
}

void GameObject::tick(World *world) {
    if (jumpCooldown > 0) {
        jumpCooldown--;
    }
    if (behavior) {
        behavior->tick(world, this);
    }
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
    if (world->isKeyDown('b')) {
        SDL_FillRect(screen, &rect, SDL_MapRGBA(screen->format, r, g, b, 70));
    }
    auto actionAnimationIter = actionsAnimation.find(currentAction);
    std::shared_ptr<Animation> actionAnim;
    if (actionAnimationIter != actionsAnimation.end()) {
        actionAnim = actionAnimationIter->second;
    } else if (idle) {
        actionAnim = idle;
    }
    if (actionAnim) {
        actionAnim->draw(screen, actualpos.x - actionAnim->width() / 2, actualpos.y - actionAnim->height() / 2);
    }
}

float GameObject::printPosition(){
    return groundBody->GetPosition().y;
}

float GameObject::polarityForceMultiplier(Charge a, Charge b) {
	if (a == 0 || b == 0) {
		return 0;
	}
	if (a == b) {
		return 1;
	} else if (a == -b) {
		return -1;
	} else {
		return 0;
	}
}

}

