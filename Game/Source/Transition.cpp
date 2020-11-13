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
	if(current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch(current_step)
	{
		case fade_step::fade_to_black:
		{
			if(now >= total_time)
			{
				SwitchMap(map_name);
				total_time += total_time;
				start_time = SDL_GetTicks();
				current_step = fade_step::fade_from_black;
			}
		} break;

		case fade_step::fade_from_black:
		{
			normalized = 1.0f - normalized;

			if(now >= total_time)
				current_step = fade_step::none;
				App->player->player.freeze = false;
		} break;
	}

	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

bool Transition::FadeToBlack(const char* mapname, bool reset_player, float time)
{
	bool ret = false;
	
	playerReset = reset_player;
	map_name = mapname;

	if(current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;

		App->player->player.freeze = true;
	}

	return ret;
}

bool Transition::SwitchMap(const char* mapname) 
{
	bool ret = true;
	LOG("Switching Maps...");

	App->collisions->colliders.clear();
	App->map->CleanUp();

	ret = App->map->Load(mapname);
	App->collisions->LoadFromMap();
	ret = App->player->StartPlayer();
	
	return ret;
}