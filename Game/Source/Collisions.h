#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__


//#include "Module.h"
#include "EntityManager.h"
#include "SDL/include/SDL.h"

enum class ObjectType;
struct Object;
struct Properties;


struct Collider
{
	SDL_Rect rect;
	bool toDelete = false;
	ObjectType type;
	Module* callback = nullptr;
	Properties* userData;
	
	Entity* entity;

	Collider(SDL_Rect rectangle, ObjectType type, Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	Collider(Object object);

	Collider() {};

	inline void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	inline bool CheckCollision(const SDL_Rect& r) const;
};



class Collisions : public Module
{
	//Methods
public:
	Collisions();

	bool Init();
	// Destructor
	virtual ~Collisions() {};

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	Collider* AddCollider(SDL_Rect rect, ObjectType type, Module* callback, Entity* entity = nullptr, Properties* userData = nullptr);
	void LoadFromMap();

private:
	
	void DebugDraw();

	//Variables
public:

	List<Collider*> colliders;
	bool debugColliders;

private:

};

#endif
