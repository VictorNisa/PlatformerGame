#include "App.h"
#include "Map.h"
#include "Pathfinding.h"
#include "SDL/include/SDL_rect.h"
#include "EntityManager.h"

#define DETECTION_RADIUS 500

FlyingEnemy::FlyingEnemy(float x, float y, EntityType Type) : Entity(x, y, Type)
{
	colliderRect = { position.x, position.y, 50, 50 };
	collider = App->collisions->AddCollider(colliderRect, ObjectType::ENEMY, App->entities, (Entity*)this);
	speed = { 100,100 };
}

bool FlyingEnemy::Save(pugi::xml_node & node) const
{
	node.append_attribute("EntityType") = "FLYING_ENEMY";

	return true;
}

bool FlyingEnemy::Load(pugi::xml_node & node)
{
	return true;
}

bool FlyingEnemy::Awake(pugi::xml_node &)
{
	return true;
}

bool FlyingEnemy::Start()
{
	return true;
}

bool FlyingEnemy::Update(float dt)
{
	goal = App->entities->player->position;

	if (goal.DistanceTo(position) < DETECTION_RADIUS)
	{
		App->pathfinding->CreatePath(App->map->WorldToMap(position.x, position.y), App->map->WorldToMap(goal.x, goal.y));
		
		const DynArray<iPoint>* Path = App->pathfinding->GetLastPath();

		const iPoint* tile;
		if (Path->Count() != 0)
		{
			if (Path->Count() > 1)
			{
				tile = Path->At(1);
			}
			else
			{
				tile = Path->At(0);
			}

			iPoint closest_center = App->map->MapToWorldCentered(tile->x, tile->y);

			if (closest_center.x > position.x)
			{
				position.x += speed.x * dt;
				flip = false;
			}
			else if (closest_center.x < position.x)
			{
				position.x -= speed.x * dt;
				flip = true;
			}
			if (closest_center.y > position.y)
			{
				position.y += speed.y * dt;
			}
			else if (closest_center.y < position.y)
			{
				position.y -= speed.y * dt;
			}
		}
	}

	if (collider != nullptr)
	{
		collider->SetPos(position.x + 7, position.y + 7);
	}

	App->map->DrawAnimation("bat_idle", "Bat", position, aInfo, flip);

	return true;
}