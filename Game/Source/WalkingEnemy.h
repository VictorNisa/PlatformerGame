#include "EntityManager.h"
#include "Map.h"

class WalkingEnemy : public Entity
{
public:
	fPoint goal;
	iPoint gravity;

public:
	WalkingEnemy(float x, float y, EntityType Type);

	bool Awake(pugi::xml_node &);
	bool Start();
	bool Update(float dt);
	
	void pathfind();

	bool Save(pugi::xml_node & node) const;
	bool Load(pugi::xml_node & node);
	bool CleanUp();
};
