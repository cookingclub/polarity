#include <vector>
namespace Polarity {
  class GameObject {
     Vector2f location;
     Vector2f min; // relative to location
     Vector2f max;
     static bool collide(const GameObject &obj0, const GameObject &obj1, Vector2f& normal){
         Vector2f bb0min = obj0.location - obj0.min;
         Vector2f bb0max = obj0.location + obj0.max;
	 return false;
      } 
  };
  class Movable :public GameObject {
      Vector2f velocity;
  };
  class Player: public Movable {
      Vector2f location;
  };
  class PolygonalObject : public Movable {
     std::vector<Vector2f> vertices;
  };


 
 
 
  
}
