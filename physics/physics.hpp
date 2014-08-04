#ifndef POLARITY_PHYSICS_PHYSICS_HPP__
#define POLARITY_PHYSICS_PHYSICS_HPP__

#include <vector>
#include <map>
#include <Box2D/Box2D.h>
#include "behavior.hpp"
#include "graphics/graphics.hpp"

#include "SDL/SDL.h"


namespace Polarity {
class Behavior;
class World;
class Trigger;

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
    static Type parseTypeStr(const std::string &str);
private:
    Actions currentAction;
    Behavior * behavior;
    std::string name;
    PropertyMap properties;
    Type type;
    bool collidable;
    int jumpCooldown;
    std::shared_ptr<Animation> idle;
    std::map<Actions, std::shared_ptr<Animation> > actionsAnimation;
public:
    void setAction(Actions a);
    b2AABB getBounds()const;
    void tick(World*world);
    void draw(World * world, SDL_Surface* surface);
    Type getType() { return type; }
    bool isJumpable() { return jumpCooldown == 0; }
    void setJumpable(bool canJump) { jumpCooldown = canJump ? 0 : -1; }
    void setJumpCooldown(bool canJump) { jumpCooldown = canJump; }
    bool isCollidable() { return collidable; }
    const std::string& getName() { return name; }
    b2Body*groundBody;
    float printPosition();
    GameObject(b2World *world, Behavior * behavior, const b2BodyDef &bdef, const std::vector<b2FixtureDef> &fixtures, const std::string &name, Type type, const PropertyMap &properties);
};

}

#endif
