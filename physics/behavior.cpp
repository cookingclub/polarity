#include "SDL/SDL.h"
#include "SDL/SDL_video.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_rotozoom.h"
#include <iostream>
#include "behavior.hpp"
#include "world/world.hpp"

namespace Polarity {

void KeyboardBehavior::handleMusicByPlayerColor(World *world) {
    if (world->playerState()->color == Polarity::PlayerColor::WHITE) {
        world->audio()->setChannelVolume("white-music", 1.0);
        world->audio()->setChannelVolume("black-music", 0.0);
    } else {
        world->audio()->setChannelVolume("white-music", 0.0);
        world->audio()->setChannelVolume("black-music", 1.0);
    }
}


/**
    Current controls:   left/right arrows:  move left/right
                        up arrow:           jump
                        m key:              mute/unmute music
                        Shift key:          toggle black/white
 */
void KeyboardBehavior::tick(World *world, GameObject *obj) {
    bool left = world->isKeyDown(SDLK_LEFT);
    bool right = world->isKeyDown(SDLK_RIGHT);
    bool jump = world->isKeyDown(SDLK_UP);
    bool toggleMute = world->wasKeyJustPressed(SDLK_m);
    bool toggleBlackWhite = world->wasKeyJustPressed(SDLK_RSHIFT) || world->wasKeyJustPressed(SDLK_LSHIFT);

    b2Body *phyobj = obj->groundBody;
    if (left) {
        obj->setAction(GameObject::WALK);
        phyobj->ApplyForce( b2Vec2(-50,0), phyobj->GetWorldCenter(), true);
        world->audio()->playChannel("step-stone", -1);
    }

	
   if (jump) {
//      phyobj->ApplyForce( b2Vec2(0,50), phyobj->GetWorldCenter(), true);
		b2ContactEdge* contact = phyobj->GetContactList();
//		b2Contact* contact = phyobj->GetContactList();
//		for (b2ContactEdge* contact = phyobj->GetContactList(); contact; contact = contact->next())
		
		if (contact){
			phyobj->ApplyLinearImpulse( b2Vec2(0,1), phyobj->GetWorldCenter(), true);
			world->audio()->playChannel("jump", 0);
			std:: cerr<< contact <<std:: endl;
		}	



		}
    if (right) {
        obj->setAction(GameObject::WALK);
        phyobj->ApplyForce( b2Vec2(50,0), phyobj->GetWorldCenter(), true);
        world->audio()->playChannel("step-stone", -1);
    }
    if (phyobj->GetLinearVelocity().LengthSquared() < .1) { // and contact
        obj->setAction(GameObject::IDLE);
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
        world->playerState()->color = (world->playerState()->color == Polarity::PlayerColor::WHITE ? Polarity::PlayerColor::BLACK : Polarity::PlayerColor::WHITE);
        KeyboardBehavior::handleMusicByPlayerColor(world);
    }

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
