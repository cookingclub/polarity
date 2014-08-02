namespace Polarity {
namespace Polarity {
  class GameObject {
     Vector location;
     Vector min; // relative to location
     Vector max;
     static bool collide(const GameObject &obj0, const GameObject &obj1, Vector2f& normal){
         Vector bb0min = obj0.location - obj0.min;
         Vector bb0max = obj0.location + obj0.max;
    
      } 
  };
  class Movable :public GameObject {
      Vector velocity;
  };
  class Player: public Movable {
      Vector location;
  };
  class PolygonalObject : public BoundedObject {
     std::vector<Vector> vertices;
  };


 
 
 
  
}
