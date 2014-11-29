#ifndef POLARITY_PHYSICS_GAME_OBJECT_HPP__
#define POLARITY_PHYSICS_GAME_OBJECT_HPP__

#include <vector>
#include <map>
#include <Box2D/Box2D.h>
#include "behavior.hpp"
#include "graphics/animation.hpp"

#ifdef USE_SDL2
#include "SDL2/SDL.h"
#else
#include "SDL/SDL.h"
#endif


namespace Polarity {
class Behavior;
class World;
class Canvas;

typedef std::unordered_map<std::string, std::string> PropertyMap;

class GameObject {
public:
    enum Type {
        TERRAIN,
        DOOR,
        PLATFORM,
        TRIGGER,
        PLAYER,
    };
    enum Actions {
        IDLE,
        WALK,
        RUN,
        JUMP,
        OPEN,
        NUM_ACTIONS,
    };
    enum Charge {
		BLACK=-1,
		COLORLESS=0,
		WHITE=1
	};
    static Type parseTypeStr(const std::string &str);
    static float polarityForceMultiplier(Charge a, Charge b);
    static inline GameObject *getContactObjectA(b2Contact *contact) {
        return static_cast<GameObject*>(
                contact->GetFixtureA()->GetBody()->GetUserData());
    }
    static inline GameObject *getContactObjectB(b2Contact *contact) {
        return static_cast<GameObject*>(
                contact->GetFixtureB()->GetBody()->GetUserData());
    }
private:
    Charge polarityCharge;
    Actions currentAction;
    std::unique_ptr<Behavior> behavior;
    std::string name;
    PropertyMap properties;
    Type type;
    int jumpCooldown;
    std::weak_ptr<World> wworld;
    std::shared_ptr<Animation> idle;
    std::map<Actions, std::shared_ptr<Animation> > actionsAnimation;
public:
    Charge getPolarity() const { return polarityCharge; }
    void setPolarity(Charge polarity) { polarityCharge = polarity; }
    void flipPolarity() { setPolarity((Charge)-getPolarity()); }
    void setAction(Actions a);
    Actions getAction() {
        return currentAction;
    }
    b2AABB getBounds()const;
    virtual void tick(World*world);
    void draw(World * world, Canvas* surface);
    Type getType() const { return type; }
    bool isJumpable() const { return jumpCooldown == 0; }
    void setJumpable(bool canJump) { jumpCooldown = canJump ? 0 : -1; }
    void setJumpCooldown(bool canJump) { jumpCooldown = canJump; }
    void onBeginCollision(World *world, b2Contact *contact);
    void onEndCollision(World *world, b2Contact *contact);
    inline b2Fixture *localFixture(b2Contact *contact) {
        return getContactObjectA(contact) == this ?
            contact->GetFixtureA() : contact->GetFixtureB();
    }
    inline b2Fixture *remoteFixture(b2Contact *contact) {
        return getContactObjectA(contact) == this ?
            contact->GetFixtureB() : contact->GetFixtureA();
    }
    inline GameObject *remoteObject(b2Contact *contact) {
        return static_cast<GameObject*>(
                remoteFixture(contact)->GetBody()->GetUserData());
    }
    std::string getProperty(const std::string &key) const {
        auto it = properties.find(key);
        if (it == properties.end()) {
            return std::string();
        }
        return it->second;
    }
    const std::string& getName() const { return name; }
    b2Body*groundBody;
    float printPosition();
    GameObject(const std::shared_ptr<Canvas> &canvas, World *world, Behavior * behavior, const b2BodyDef &bdef, const std::vector<b2FixtureDef> &fixtures, const std::string &name, Type type, const PropertyMap &properties);

    virtual ~GameObject();
};

}

#endif
