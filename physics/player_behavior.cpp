#include <iostream>
#include "player_behavior.hpp"
#include "world/world.hpp"

namespace Polarity {

void PlayerBehavior::handleMusicByPlayerColor(World *world, GameObject *obj) {
    if (obj->getPolarity() > 0) {
        world->audio()->setChannelVolume("white-music", 1.0);
        world->audio()->setChannelVolume("black-music", 0.0);
    } else if (obj->getPolarity() < 0) {
        world->audio()->setChannelVolume("white-music", 0.0);
        world->audio()->setChannelVolume("black-music", 1.0);
    } else {
        world->audio()->setChannelVolume("white-music", 0.5);
        world->audio()->setChannelVolume("black-music", 0.5);
    }
}


const float MAX_VELOCITY = 5;

void PlayerBehavior::addedToWorld(World *world, GameObject* obj) {
    if (world && world->audio()) {
        world->audio()->playChannel("white-music");
        world->audio()->playChannel("black-music");
    }
}

/**
    Current controls:   left/right arrows:  move forward/backwards
                        up arrow:           jump
                        m key:              mute/unmute music
                        Shift key:          toggle black/white
 */
void PlayerBehavior::tick(World *world, GameObject *obj) {
    bool left = world->isKeyDown(SDLK_LEFT);
    bool right = world->isKeyDown(SDLK_RIGHT);
    bool jump = world->isKeyDown(SDLK_UP);
    bool toggleMute = world->wasKeyJustPressed(SDLK_m);
    bool toggleBlackWhite = world->wasKeyJustPressed(SDLK_RSHIFT) || world->wasKeyJustPressed(SDLK_LSHIFT);

    b2Body *phyobj = obj->groundBody;

    // get the vectors for L, R, U (this is 3x redundant but easy)
    b2Vec2 forceDirectionR = phyobj->GetWorldVector( b2Vec2(1,0) );
    b2Vec2 forceDirectionL = phyobj->GetWorldVector( b2Vec2(-1,0) );
    b2Vec2 forceDirectionU = phyobj->GetWorldVector( b2Vec2(0,1.5) );

    if (left) {
        obj->setAction(GameObject::WALK);
        phyobj->ApplyLinearImpulse( forceDirectionL, phyobj->GetWorldCenter(), true); // moves backwards
        //phyobj->ApplyLinearImpulse( b2Vec2(-1,0), phyobj->GetWorldCenter(), true);
        world->audio()->playChannel("step-stone", -1);
    }

    
   if (jump && obj->isJumpable()) {
//      phyobj->ApplyForce( b2Vec2(0,50), phyobj->GetWorldCenter(), true);
//      b2ContactEdge* contact = phyobj->GetContactList();
//      b2Contact* contact = phyobj->GetContactList();
//      for (b2ContactEdge* contact = phyobj->GetContactList(); contact; contact = contact->next())

//        if (contact){
//          std:: cerr<< contact <<std:: endl;
        phyobj->ApplyLinearImpulse( forceDirectionU, phyobj->GetWorldCenter(), true);
        //phyobj->ApplyLinearImpulse( b2Vec2(0,1), phyobj->GetWorldCenter(), true);
        obj->setJumpCooldown(15);
        world->audio()->playChannel("jump", 0);
//        }
    }
    if (right) {
        obj->setAction(GameObject::WALK);
        phyobj->ApplyLinearImpulse( forceDirectionR, phyobj->GetWorldCenter(), true);
        //phyobj->ApplyLinearImpulse( b2Vec2(1,0), phyobj->GetWorldCenter(), true);
        world->audio()->playChannel("step-stone", -1);
    }
    b2Vec2 vel = phyobj->GetLinearVelocity();
    if (vel.LengthSquared() < .1) { // and contact
        obj->setAction(GameObject::IDLE);
    }
    if (abs(vel.x) > MAX_VELOCITY) {
        phyobj->SetLinearVelocity(b2Vec2(MAX_VELOCITY * vel.x / abs(vel.x), vel.y));
    }
    if (toggleMute) {
        cerr << "Toggling music mute" << endl;
        world->gameState()->musicOn = !world->gameState()->musicOn;
        if (!world->gameState()->musicOn) {
            world->audio()->stopChannel("white-music");
            world->audio()->stopChannel("black-music");
        } else {
            world->audio()->playChannel("white-music");
            world->audio()->playChannel("black-music");            
        }
    }

    if (toggleBlackWhite) {
        cerr << "Toggling player color" << endl;
        obj->flipPolarity();
    }
    PlayerBehavior::handleMusicByPlayerColor(world, obj);

    // stop sounds if we're not walking anymore
    if (!right && !left) {
        world->audio()->stopChannel("step-stone");
    }
    if (!jump) {
        world->audio()->stopChannel("jump");
    }
    world->updateCamera(obj, phyobj->GetWorldCenter());
    world->clearJustPressedStates();
}

}
