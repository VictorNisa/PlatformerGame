#include "App.h"
#include "Collisions.h"
#include "Map.h"
#include "Render.h"
#include "Input.h"

Collisions::Collisions() : Module(), debug_colliders(false)
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
	List_item<Collider*>* Coll_iterator = colliders.start;
	while (Coll_iterator != nullptr) 
	{
		if (Coll_iterator->data->to_delete == true)
		{
			colliders.del(Coll_iterator);
		}

		Coll_iterator = Coll_iterator->next;
	}

	// Calculate collisions
	Collider* c1;
	Collider* c2;

	Coll_iterator = colliders.start;
	while (Coll_iterator != nullptr) 
	{
		c1 = Coll_iterator->data;
		
		List_item<Collider*>* Coll_iterator2 = colliders.start;

		while (Coll_iterator2 != nullptr) 
		{
			c2 = Coll_iterator2->data;

			if (c1 != c2) 
			{
				if (c1->CheckCollision(c2->rect) == true && (c1->type == ObjectType::PLAYER || c2->type == ObjectType::PLAYER))
				{
					if (c1->callback)
						c1->callback->OnCollision(c1, c2);

					if (c2->callback)
						c2->callback->OnCollision(c2, c1);
				}
			}
			Coll_iterator2 = Coll_iterator2->next;
		}
		Coll_iterator = Coll_iterator->next;
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
		if (debug_colliders)
		{
			debug_colliders = false;
		}
		else 
		{
			debug_colliders = true;
		}
	}

	if (debug_colliders == false)
		return;

	Uint8 alpha = 80; 	//Alpha value for all debug colliders
	List_item<Collider*>* Coll_iterator = colliders.start;
	while (Coll_iterator != nullptr) 
	{
		switch (Coll_iterator->data->type)
		{
		case ObjectType::PLATFORM:
			App->render->DrawQuad(Coll_iterator->data->rect, 255, 255, 255, alpha);
			break;
		case ObjectType::SOLID:
			App->render->DrawQuad(Coll_iterator->data->rect, 255, 0, 0, alpha);
			break;
		case ObjectType::DAMAGE:
			App->render->DrawQuad(Coll_iterator->data->rect, 0, 0, 255, alpha);
			break;
		case ObjectType::PLAYER:
			App->render->DrawQuad(Coll_iterator->data->rect, 0, 255, 0, alpha);
			break;
		case ObjectType::WARP:
			App->render->DrawQuad(Coll_iterator->data->rect, 50, 200, 147, alpha);
			break;
		}
		Coll_iterator = Coll_iterator->next;
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


Collider* Collisions::AddCollider(SDL_Rect rect, ObjectType type, Module* callback, Properties* userdata)
{
	Collider* ret = new Collider;

	ret->callback = callback;
	ret->rect = rect;
	ret->type = type;
	ret->userdata = userdata;

	colliders.add(ret);
	
	return ret;
}

//Add Colliders to the list from the map
void Collisions::LoadFromMap() 
{
	List_item<MapObjectgroup*>* list_i = App->map->data.objectgroups.start;
	while (list_i != nullptr) 
	{
		for (int i = 0; i < list_i->data->objects_size; i++) 
		{
			AddCollider(*list_i->data->objects[i].box, list_i->data->objects[i].type, nullptr, &list_i->data->objects[i].properties);
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