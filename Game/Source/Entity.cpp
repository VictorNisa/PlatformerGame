#include "PugiXml/src/pugixml.hpp"
#include "List.h"
#include "Point.h"
#include "SDL/include/SDL.h"

#include "EntityManager.h"
//#include "Entity.h"
//#include "Flying_Enemy.h"


//Entity Methods

Entity::Entity(float x, float y, EntityType Type) : position(x, y), type(Type)
{

}

bool Entity::Awake()
{
	return true;
}
bool Entity::Start()
{
	return true;
}

bool Entity::Update(float dt)
{
	return true;
}

bool Entity::PostUpdate()
{
	return true;
}

bool Entity::PreUpdate()
{
	return true;
}

bool Entity::CleanUp()
{
	return true;
}

bool Entity::Load(pugi::xml_node &)
{
	return true;
}

bool Entity::Save(pugi::xml_node &) const
{
	return true;
}