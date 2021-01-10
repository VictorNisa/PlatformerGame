#include <math.h>

#include "App.h"
#include "Module.h"
#include "Transition.h"
#include "Window.h"
#include "Log.h"
#include "Render.h"
#include "Players.h"
#include "Collisions.h"
#include "Map.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"

Transition::Transition()
{
	name.create("fade");

}

Transition::~Transition()
{}

bool Transition::Awake(pugi::xml_node& conf) 
{	
	uint width, height;
	App->win->GetWindowSize(width, height);
	screen.w = width;
	screen.h = height;
	screen.x = screen.y = 0;

	return true;
}

// Load assets
bool Transition::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool Transition::Update(float dt)
{
	if(CurrentStep == FadeStep::NONE)
		return true;

	Uint32 now = SDL_GetTicks() - startTime;
	float normalized = MIN(1.0f, (float)now / (float)totalTime);

	switch(CurrentStep)
	{
		case FadeStep::FADE_TO_BLACK:
		{
			if(now >= totalTime)
			{
				SwitchMap(level);
				totalTime += totalTime;
				startTime = SDL_GetTicks();
				CurrentStep = FadeStep::FADE_FROM_BLACK;
			}
		} break;

		case FadeStep::FADE_FROM_BLACK:
		{
			normalized = 1.0f - normalized;
			if(now >= totalTime)
				CurrentStep = FadeStep::NONE;
				// App->player->player.freeze = false;
		} break;
	}

	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

bool Transition::FadeToBlack(int lvl, bool isSaveLoad, float time)
{
	bool ret = false;

	if(CurrentStep == FadeStep::NONE)
	{
		isLoad = isSaveLoad;
		level = lvl;
		CurrentStep = FadeStep::FADE_TO_BLACK;
		startTime = SDL_GetTicks();
		totalTime = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;

		// App->player->player.freeze = true;
	}

	return ret;
}

bool Transition::SwitchMap(int level) 
{
	bool ret = true;
	LOG("Switching Maps...");

	App->map->CleanUp();
	App->entities->CleanUp();
	App->collisions->CleanUp();

	App->map->Load(level);
	if (!isLoad)
	{
		App->collisions->LoadFromMap();
	}
	else if (isLoad) {
		App->collisions->LoadFromMap(false);

		//Delete all entities.
		List_item<Collider*>* Coll_iterator = App->collisions->colliders.start;
		while (Coll_iterator != nullptr) {

			if (Coll_iterator->data->type == ObjectType::ENEMY || Coll_iterator->data->type == ObjectType::PLAYER || Coll_iterator->data->type == ObjectType::COIN)
			{
				int tmp = App->entities->entity_list.find(Coll_iterator->data->entity);
				if (tmp != -1) {

					Entity* to_delete = App->entities->entity_list.At(tmp)->data;
					App->entities->DeleteEntity(to_delete);
				}

			}
			Coll_iterator = Coll_iterator->next;
		}
		App->entities->Load_Now();
	}
	
	return ret;
}