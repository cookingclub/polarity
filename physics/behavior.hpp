#ifndef _BEHAVIOR_HPP_
#define _BEHAVIOR_HPP_
namespace Polarity {
class GameObject;
class World;

class Behavior {
public:
    Behavior() {
    }
    virtual void tick(World *world, GameObject *obj){
    }
};

 
class KeyboardBehavior : public Behavior{
public:
    KeyboardBehavior() {
    }
    virtual void handleMusicByPlayerColor(World *world);
    virtual void tick(World * world, GameObject *obj);
};
}
#endif
