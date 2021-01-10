#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"

#include "List.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Timer.h"

#include "Entity.h"
#include "FlyingEnemy.h"
#include "Players.h"
#include "WalkingEnemy.h"
#include "Coin.h"

// Manager class
class EntityManager : public Module
{

public:

	EntityManager();

	// Destructor
	// virtual ~EntityMachine();

	bool Init();

	bool Awake(pugi::xml_node& config);

	Entity* CreateEntity(float x, float y, EntityType Type);

	void DeleteEntity(Entity* entity);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(Collider* A, Collider* B);


	bool Save(pugi::xml_node& node) const;
	bool Load(pugi::xml_node& node);

	bool Load_Now();

	//Extra Functions
	void PlayerCollisions(Collider* A, Collider* B);

	void AttackCollisions(Collider* A, Collider* B);

	void WalkingEnemyCollisions(Collider* A, Collider* B);

private:

	//VARIABLES
public:

	pugi::xml_node conf;

	List<Entity*> entity_list;

	Players* player = nullptr;

	Timer* damageTimer;

	//FlyingEnemy* bat = nullptr;

	//WalkingEnemy* knight = nullptr;

	//WalkingEnemy* knight2 = nullptr;

private:
	pugi::xml_node loadNode;
	int coin;
	int hurt;
};


#endif //__ENTITYMACHINE_H__
