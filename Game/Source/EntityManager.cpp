#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Transition.h"

#include "EntityManager.h"

EntityManager::EntityManager()
{
	name.create("entities");
};


bool EntityManager::Start()
{
	List_item<Entity*>* entityIter = entity_list.start;

	while (entityIter != NULL)
	{
		entityIter->data->Start();
		entityIter = entityIter->next;
	}

	return true;
};

bool EntityManager::Init()
{
	return true;
};

bool  EntityManager::Awake(pugi::xml_node& config)
{
	// List_item<Entity*>* entityIter = entity_list.start;

	//while (entityIter != NULL)
	//{
	//	entityIter->data->Awake(config);

	//	entityIter = entityIter->next;
	//}

	return true;
};

bool EntityManager::PreUpdate()
{
	List_item<Entity*>* entityIter = entity_list.start;

	while (entityIter != NULL)
	{
		entityIter->data->PreUpdate();

		entityIter = entityIter->next;
	}

	return true;
}

bool EntityManager::Update(float dt)
{

	List_item<Entity*>* entityIter = entity_list.start;

	while (entityIter != NULL)
	{
		entityIter->data->Update(dt);

		entityIter = entityIter->next;
	}

	return true;
};

bool EntityManager::PostUpdate()
{
	List_item<Entity*>* entityIter = entity_list.start;

	while (entityIter != NULL)
	{
		entityIter->data->PostUpdate();

		entityIter = entityIter->next;
	}

	return true;
}

bool EntityManager::CleanUp()
{

	List_item<Entity*>* entityIter = entity_list.start;

	while (entityIter != NULL)
	{
		entityIter->data->CleanUp();

		entityIter = entityIter->next;
	}

	return true;
};

// Create an Entity and add to the list ----------------------------------------------------
Entity* EntityManager::CreateEntity(float x, float y, EntityType Type)
{

	static_assert(EntityType::UNKNOWN == 4, " Entity type error. I should check the code again ");

	Entity* ret = nullptr;

	switch (Type)
	{
	case PLAYER:

		ret = new Players(x, y, Type);

		player = (Players*)ret;

		if (ret != nullptr)
		{
			entity_list.add(ret);
		}

		ret->Awake();
		ret->Start();

		break;

	case PARTICLE:

		// Maybe add a simple particle system

		break;

	case FLYING_ENEMY:

		ret = new FlyingEnemy(x, y, Type);

		if (ret != nullptr)
		{
			entity_list.add(ret);
		}

		break;

	case WALKING_ENEMY:

		ret = new WalkingEnemy(x, y, Type);

		if (ret != nullptr)
		{
			entity_list.add(ret);
		}

		break;
	}

	return ret;
};


// Destroy an Entity and remove it from the list -----------------------------------------------------
void EntityManager::DeleteEntity(Entity* entity)
{
	entity->collider->toDelete = true;
	entity_list.del(entity_list.At(entity_list.find(entity)));
}

bool EntityManager::Save(pugi::xml_node& node) const
{
	List_item<Entity*>* entityIter = entity_list.start;
	while (entityIter != NULL)
	{
		pugi::xml_node entityNode = node.append_child("entity");

		entityNode.append_child("position").append_attribute("x") = entityIter->data->position.x;
		entityNode.child("position").append_attribute("y") = entityIter->data->position.y;

		entityNode.append_child("prevPosition").append_attribute("x") = entityIter->data->prevPosition.x;
		entityNode.child("prevPosition").append_attribute("y") = entityIter->data->prevPosition.y;

		entityNode.append_child("speed").append_attribute("x") = entityIter->data->speed.x;
		entityNode.child("speed").append_attribute("y") = entityIter->data->speed.y;

		//entityNode.append_child("animation").set_value(entityIter->data->animation.GetString());

		entityIter->data->Save(entityNode);

		entityIter = entityIter->next;
	}

	return true;
};

bool EntityManager::Load(pugi::xml_node& node)
{
	entity_list.clear();

	pugi::xml_node nodeI = node;
	Entity* loadedEntity;

	for (; nodeI; nodeI = nodeI.next_sibling("entity"))
	{
		iPoint pos;
		pos.x = node.child("position").attribute("x").as_int();
		pos.y = node.child("position").attribute("y").as_int();

		if (strcmp(node.attribute("EntityType").as_string(), "FLYING_ENEMY"))
		{
			loadedEntity = (Entity*)new FlyingEnemy(pos.x, pos.y, FLYING_ENEMY);
		}
		else if (strcmp(node.attribute("EntityType").as_string(), "WALKING_ENEMY"))
		{
			loadedEntity = (Entity*)new WalkingEnemy(pos.x, pos.y, WALKING_ENEMY);
		}
		else if (strcmp(node.attribute("EntityType").as_string(), "PLAYER"))
		{
			loadedEntity = (Entity*)new Players((float)pos.x, (float)pos.y, PLAYER);

			// If code breaks, check this line of code
		}
		else break;
		loadedEntity->prevPosition.x = node.child("position").attribute("x").as_int();
		loadedEntity->prevPosition.x = node.child("position").attribute("y").as_int();

		entity_list.add(loadedEntity);
	}

	return true;
};

void EntityManager::OnCollision(Collider* A, Collider* B)
{
	PlayerCollisions(A, B);

	AttackCollisions(A, B);


	WalkingEnemyCollisions(A, B);
}

void EntityManager::AttackCollisions(Collider* A, Collider* B)
{
	if (A->type == ObjectType::ATTACK && B->type == ObjectType::ENEMY) 
	{
		DeleteEntity(B->entity);
	}
}


void EntityManager::WalkingEnemyCollisions(Collider* A, Collider* B)
{
	if (A->type != ObjectType::ENEMY && B->type != ObjectType::ENEMY)
	{
		return;
	}
	if (B->type == ObjectType::ENEMY) 
	{
		Collider temp = *A;
		A = B;
		B = &temp;
	}
	if (B->type == ObjectType::ENEMY || B->type == ObjectType::PLAYER)
	{
		return;
	}

	if (A->type == ObjectType::ENEMY && B->type == ObjectType::SOLID)
	{

		// Check collisions from above
		if (A->entity->position.y + A->rect.h > B->rect.y
			&& A->entity->position.y < B->rect.y
			&& A->rect.x < B->rect.x + B->rect.w
			&& A->rect.x + A->rect.w > B->rect.x
			&& A->entity->prevPosition.y + A->rect.h <= B->rect.y + 1)
		{
			if (A->entity->speed.y > 0)
			{
				A->entity->speed.y = 0;
			}
			A->entity->position.y = B->rect.y - A->rect.h + 1;
			A->entity->collider->SetPos(A->entity->position.x, A->entity->position.y);
			A->entity->grounded = true;
		}

		// Check collisions from below
		if (A->entity->prevPosition.y > (B->rect.y + B->rect.h - 1))
		{
			A->entity->position.y = B->rect.y + B->rect.h;
			A->entity->collider->SetPos(A->entity->position.x, A->entity->position.y);
		}

		// Check collisions from the sides
		if (A->entity->position.y + (A->rect.h* 0.1f) < B->rect.y + B->rect.h
			&& A->entity->position.y + (A->rect.h* 0.9f) > B->rect.y)
		{
			if ((A->rect.x + A->rect.w) < (B->rect.x + B->rect.w / 4))
			{
				// ENEMY |WALL
				// ENEMY |WALL
				// ENEMY |WALL
				// ENEMY |WALL
				A->entity->position.x = B->rect.x - A->rect.w + 1;
			}
			else if (A->rect.x > (B->rect.x + B->rect.w * 3 / 4))
			{
				// WALL| ENEMY
				// WALL| ENEMY
				// WALL| ENEMY
				// WALL| ENEMY
				A->entity->position.x = B->rect.x + B->rect.w - 1;
			}
			A->entity->collider->SetPos(A->entity->position.x, A->entity->position.y);
		}
	}
}



void EntityManager::PlayerCollisions(Collider* A, Collider* B)
{
	if (player->player.godMode) return;

	if (B->type == ObjectType::PLAYER) {
		Collider temp = *A;
		A = B;
		B = &temp;
	}

	if (A->type != ObjectType::PLAYER) {
		return;
	}

	if (A->type == ObjectType::PLAYER && B->type == ObjectType::ENEMY)
	{
		App->fade->FadeToBlack("Scene1.tmx");
	}

	if (A->type == ObjectType::PLAYER && B->type == ObjectType::SOLID)
	{
		if (player->position.y + A->rect.h > B->rect.y
			&& player->position.y < B->rect.y
			&& A->rect.x < B->rect.x + B->rect.w
			&& A->rect.x + A->rect.w > B->rect.x
			&& player->player.prevPosition.y + player->player.boxH <= B->rect.y + 1
			&& player->player.justLoaded == false)
		{
			if (player->player.speed.y > 0)
			{
				player->player.speed.y = 0;
			}
			player->position.y = B->rect.y - player->player.collider->rect.h + 1;
			player->player.collider->SetPos(player->position.x + player->player.boxOffset_x, player->position.y);
			player->player.SetGroundState(true);
		}

		if (player->player.prevPosition.y > (B->rect.y + B->rect.h - 1))
		{
			player->position.y = B->rect.y + B->rect.h;
			player->player.cealing = true;
			player->player.collider->SetPos(player->position.x + player->player.boxOffset_x, player->position.y);
		}

		if (player->position.y + (A->rect.h* 0.1f) < B->rect.y + B->rect.h
			&& player->position.y + (A->rect.h* 0.9f) > B->rect.y)
		{
			player->player.wall = true;

			if ((A->rect.x + A->rect.w) < (B->rect.x + B->rect.w / 4))
			{
				player->position.x = B->rect.x - A->rect.w - player->player.boxOffset_x + 1;

			}
			else if (A->rect.x > (B->rect.x + B->rect.w * 3 / 4))
			{
				player->position.x = B->rect.x + B->rect.w - player->player.boxOffset_x - 1;
			}
			player->player.collider->SetPos(player->position.x + player->player.boxOffset_x, player->position.y);
		}
	}

	if (A->type == ObjectType::PLAYER && B->type == ObjectType::PLATFORM) {

		if (player->player.dropPlat == false) {

			if ((player->player.prevPosition.y + player->player.collider->rect.h) < B->rect.y + (B->rect.h / 2.0f) && (player->player.prevPosition.y + player->player.collider->rect.h) > B->rect.y && player->player.speed.y >= 0)
			{
				player->position.y = B->rect.y - player->player.collider->rect.h + 1;
				player->player.ableToJump = false;
				player->player.onPlatform = true;
			}
			else if ((player->position.y >= player->player.prevPosition.y) && (player->player.prevPosition.y + player->player.collider->rect.h) < B->rect.y && player->player.speed.y >= 0)
			{
				player->position.y = B->rect.y - player->player.collider->rect.h + 1;
				player->player.SetGroundState(true);
				player->player.ableToJump = false;
				player->player.onPlatform = true;
			}
		}
	}

	if (A->type == ObjectType::PLAYER && B->type == ObjectType::TELEPORT) {
		App->fade->FadeToBlack(B->userData->Get("MapToLoad").vString);
	}
}