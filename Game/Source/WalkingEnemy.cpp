#include "App.h"
#include "Map.h"
#include "SDL/include/SDL_rect.h"

#include "Pathfinding.h"
#include "EntityManager.h"

#define DETECTION_RADIUS 500

// Soldier a.k.a. walking enemy
WalkingEnemy::WalkingEnemy(float x, float y, EntityType Type) : Entity(x, y, Type)
{
	colliderRect = { position.x, position.y, 50, 64 };
	collider = App->collisions->AddCollider(colliderRect, ObjectType::ENEMY, App->entities, (Entity*)this);

	speed = { 150,150 };
}

bool WalkingEnemy::Save(pugi::xml_node & node) const
{
	node.append_attribute("EntityType") = "WALKING_ENEMY";

	return true;
};

bool WalkingEnemy::Load(pugi::xml_node & node)
{
	return true;
};

bool WalkingEnemy::Awake(pugi::xml_node &)
{
	return true;
}

bool WalkingEnemy::Start()
{
	return true;
}

bool WalkingEnemy::Update(float dt)
{
	prevPosition = position;
	animation = "idle";
	pathfind();
	if (!grounded)
	{
		position.y += 150 * dt;
	}
	if (collider != nullptr)
	{
		collider->SetPos(position.x, position.y);
	}

	App->map->DrawAnimation(animation, "Knight", position, aInfo, flip);
	grounded = false;

	return true;
}

void WalkingEnemy::pathfind()
{
	//Pathfinding -------------------------------------------
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

			iPoint closestCenter = App->map->MapToWorldCentered(tile->x, tile->y);

			if (closestCenter.x > position.x - collider->rect.w / 2)
			{
				position.x += speed.x * App->dt;
				flip = true;
				animation = "walk";
			}
			else if (closestCenter.x < position.x - collider->rect.w / 2)
			{
				position.x -= speed.x * App->dt;
				flip = false;
				animation = "walk";
			}
		}
	}
}