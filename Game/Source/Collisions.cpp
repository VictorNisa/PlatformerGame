#include "App.h"
#include "Collisions.h"
#include "Map.h"
#include "Render.h"
#include "Input.h"

Collisions::Collisions() : Module(), debugColliders(false)
{
	name.create("collisions");
}

bool Collisions::Init() 
{
	return true;
};

bool Collisions::Awake(pugi::xml_node& config) 
{
	return true;
};

bool Collisions::Start() 
{
	LoadFromMap();

	return true;
};

bool Collisions::PreUpdate() 
{
	List_item<Collider*>* collIterator = colliders.start;
	while (collIterator != nullptr) 
	{
		if (collIterator->data->toDelete == true)
		{
			colliders.del(collIterator);
		}

		collIterator = collIterator->next;
	}

	// Calculate collisions
	Collider* c1;
	Collider* c2;

	collIterator = colliders.start;
	while (collIterator != nullptr) 
	{
		c1 = collIterator->data;
		
		List_item<Collider*>* collIterator2 = colliders.start;

		while (collIterator2 != nullptr) 
		{
			c2 = collIterator2->data;

			if (c1 != c2) 
			{
				if (c1->CheckCollision(c2->rect) == true)
				{
					if (c1->callback)
						c1->callback->OnCollision(c1, c2);

					if (c2->callback)
						c2->callback->OnCollision(c2, c1);
				}
			}
			collIterator2 = collIterator2->next;
		}
		collIterator = collIterator->next;
	}

	return true;
};

bool Collisions::Update(float dt) 
{
	DebugDraw();

	return true;
};

void Collisions::DebugDraw() 
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		if (debugColliders)
		{
			debugColliders = false;
		}
		else 
		{
			debugColliders = true;
		}
	}

	if (debugColliders == false)
		return;

	Uint8 alpha = 80; 	//Alpha value for all debug colliders
	List_item<Collider*>* collIterator = colliders.start;
	while (collIterator != nullptr) 
	{
		switch (collIterator->data->type)
		{
		case ObjectType::PLATFORM:
			App->render->DrawQuad(collIterator->data->rect, 255, 255, 255, alpha);
			break;
		case ObjectType::SOLID:
			App->render->DrawQuad(collIterator->data->rect, 255, 0, 0, alpha);
			break;
		case ObjectType::DAMAGE:
			App->render->DrawQuad(collIterator->data->rect, 0, 0, 255, alpha);
			break;
		case ObjectType::PLAYER:
			App->render->DrawQuad(collIterator->data->rect, 0, 255, 0, alpha);
			break;
		case ObjectType::TELEPORT:
			App->render->DrawQuad(collIterator->data->rect, 50, 200, 147, alpha);
			break;
		case ObjectType::ATTACK:
			App->render->DrawQuad(collIterator->data->rect, 255, 0, 0, alpha);
		case ObjectType::ENEMY:
			App->render->DrawQuad(collIterator->data->rect, 255, 75, 0, alpha);
		}
		collIterator = collIterator->next;
	}
}

bool Collisions::PostUpdate() 
{
	return true;
};

bool Collisions::CleanUp() 
{
	return true;
};


Collider* Collisions::AddCollider(SDL_Rect rect, ObjectType type, Module* callback, Entity* entity, Properties* userData)
{
	Collider* ret = new Collider;

	ret->callback = callback;
	ret->rect = rect;
	ret->type = type;
	ret->userData = userData;
	ret->entity = entity;

	colliders.add(ret);
	
	return ret;
}

//Add Colliders to the list from the map
void Collisions::LoadFromMap() 
{
	List_item<MapObjectgroup*>* list_i = App->map->data.objectgroups.start;
	while (list_i != nullptr) 
	{
		for (int i = 0; i < list_i->data->objectsSize; i++) 
		{
			if (list_i->data->objects[i].type == ObjectType::ENEMY)
			{
				if (strcmp(list_i->data->objects[i].properties.list.start->data->data.vString, "FLYING_ENEMY") == 0)
				{
					App->entities->CreateEntity(list_i->data->objects[i].box->x, list_i->data->objects[i].box->y, FLYING_ENEMY);
				}
				else if (strcmp(list_i->data->objects[i].properties.list.start->data->data.vString, "WALKING_ENEMY") == 0)
				{
					App->entities->CreateEntity(list_i->data->objects[i].box->x, list_i->data->objects[i].box->y, WALKING_ENEMY);
				}
				else if (strcmp(list_i->data->objects[i].properties.list.start->data->data.vString, "PLAYER") == 0)
				{
					App->entities->CreateEntity(list_i->data->objects[i].box->x, list_i->data->objects[i].box->y, PLAYER);
				}

			}
			else 
			{
			AddCollider(*list_i->data->objects[i].box, list_i->data->objects[i].type, nullptr, nullptr, &list_i->data->objects[i].properties);
			}
		}

		list_i = list_i->next;
	}
}

//  Struct Collider Methods --------------------------------------
bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return ((r.x + r.w > rect.x) && (r.x < rect.x + rect.w) &&
		(r.y + r.h > rect.y) && (r.y < rect.y + rect.h));
}

Collider::Collider(Object object) 
{
	rect = *object.box;
	type = object.type;
}