#include "world/world.hpp"
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

GameObject::GameObject(const std::shared_ptr<Canvas> &canvas, World *world, Behavior *behavior, const b2BodyDef &bdef, const std::vector<b2FixtureDef> &fixtures, const std::string &name, Type type, const PropertyMap &props)
        : behavior(behavior), name(name), properties(props), type(type) {
    this->wworld = world->weak_ptr();
    currentAction = IDLE;
    groundBody = world->getPhysicsWorld()->CreateBody(&bdef);
    groundBody->SetUserData(this);
    for (auto &fixture : fixtures) {
        groundBody->CreateFixture(&fixture);
    }
    jumpCooldown = -1;

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
                actionsAnimation[act] = Animation::get(canvas.get(), it->second);
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
        auto where = actionsAnimation.find(a);
        if (where != actionsAnimation.end()) {
            where->second->restart();
        }
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

void GameObject::onBeginCollision(World *world, b2Contact *contact) {
    if (behavior) {
        behavior->onBeginCollision(world, this, contact);
    }
}

void GameObject::onEndCollision(World *world, b2Contact *contact) {
    if (behavior) {
        behavior->onEndCollision(world, this, contact);
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
void GameObject::draw(World * world, Canvas* screen) {
    b2Vec2 wh = 2. * world->physicsToGraphics(this->getBounds().GetExtents(), 1);
    b2Vec2 actualpos = world->physicsToGraphics(this->groundBody->GetPosition());
    float angle = this->groundBody->GetAngle();
    actualpos -= world->getCamera();

    auto actionAnimationIter = actionsAnimation.find(currentAction);
    std::shared_ptr<Animation> actionAnim;
    if (actionAnimationIter != actionsAnimation.end()) {
        actionAnim = actionAnimationIter->second;
    } else if (idle) {
        actionAnim = idle;
    }
    if (actionAnim) {
        actionAnim->drawSprite(screen,
                               actualpos.x, actualpos.y,
                               actionAnim->width(), actionAnim->height(), angle);
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
GameObject::~GameObject() {
    std::shared_ptr<World> world(wworld.lock());
    if (world) {
        world->getPhysicsWorld()->DestroyBody(groundBody);    
    }
}
}

