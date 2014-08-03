#include "world/world.hpp"
using namespace std;

namespace Polarity {
  GameObject::GameObject(b2World *world, Behavior *behavior, const b2BodyDef &bdef, const b2FixtureDef &fixture) {
    this->behavior = behavior;
    groundBody = world->CreateBody(&bdef);
    groundBody->CreateFixture(&fixture);
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
    SDL_Rect rect;
    rect.x = this->groundBody->GetPosition().x - world->getCamera().x;
    rect.y = this->groundBody->GetPosition().y - world->getCamera().y;
    b2Vec2 wh = this->getBounds().GetExtents();
    
    rect.w = wh.x;
    rect.h = wh.y;
    // TODO: set the right color
    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 255, 0, 0));
  }

  float GameObject::printPosition(){
    return groundBody->GetPosition().y;
  }
}
