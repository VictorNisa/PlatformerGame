#include "EntityManager.h"

class FlyingEnemy : public Entity
{
public:
	iPoint goal;

	FlyingEnemy(float x, float y, EntityType Type);
	bool Awake(pugi::xml_node &);
	bool Start();
	bool Update(float dt);
	bool Save(pugi::xml_node & node) const;
	bool Load(pugi::xml_node & node);
};