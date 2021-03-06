// #include "FlyingEnemy.h"
// #include "Collisions.h"

#include "SString.h"
#include "PugiXml/src/pugixml.hpp"
#include "List.h"
#include "Point.h"
#include "SDL/include/SDL.h"

// #include "EntityManager.h"

enum EntityType
{
	PLAYER = 0,
	FLYING_ENEMY,
	WALKING_ENEMY,
	COIN,
	UNKNOWN
};

struct AnimationInfo
{
	int iter = 0;
	SString prevAnimName = "idle";
	float frameCount = 0.0f;
};

//Entity base
class Entity
{
	//VARS
public:
	//Constructor
	fPoint position;
	EntityType type;

	//Vars
	SString* animation;
	fPoint speed;
	fPoint prevPosition;
	AnimationInfo ainfo;
	Collider* collider;
	SDL_Rect colliderRect;
	bool flip;
	bool grounded;

	//Methods
public:
	Entity(float x, float y, EntityType Type);

	virtual bool Awake();
	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool Update(float dt);
	virtual bool PostUpdate();
	virtual bool CleanUp();
	virtual bool Save(pugi::xml_node & node) const;
	virtual bool Load(pugi::xml_node & node);
};