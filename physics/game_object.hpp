#ifndef POLARITY_PHYSICS_GAME_OBJECT_HPP__
#define POLARITY_PHYSICS_GAME_OBJECT_HPP__

#include <vector>
#include <map>
#include <Box2D/Box2D.h>
#include "behavior.hpp"
#include "graphics/animation.hpp"

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
    enum Charge {
		BLACK=-1,
		COLORLESS=0,
		WHITE=1
	};
    static Type parseTypeStr(const std::string &str);
    static float polarityForceMultiplier(Charge a, Charge b);
private:
    Charge polarityCharge;
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
    Charge getPolarity() const { return polarityCharge; }
    void setPolarity(Charge polarity) { polarityCharge = polarity; }
    void flipPolarity() { setPolarity((Charge)-getPolarity()); }
    void setAction(Actions a);
    b2AABB getBounds()const;
    virtual void tick(World*world);
    void draw(World * world, SDL_Surface* surface);
    Type getType() const { return type; }
    bool isJumpable() const { return jumpCooldown == 0; }
    void setJumpable(bool canJump) { jumpCooldown = canJump ? 0 : -1; }
    void setJumpCooldown(bool canJump) { jumpCooldown = canJump; }
    bool isCollidable() const { return collidable; }
    const std::string& getName() const { return name; }
    b2Body*groundBody;
    float printPosition();
    GameObject(b2World *world, Behavior * behavior, const b2BodyDef &bdef, const std::vector<b2FixtureDef> &fixtures, const std::string &name, Type type, const PropertyMap &properties);
    
    virtual ~GameObject(){}
};

}

#endif
