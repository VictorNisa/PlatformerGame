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
				SwitchMap(mapName);
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

bool Transition::FadeToBlack(const char* mapname, bool resetPlayer, float time)
{
	bool ret = false;
	
	playerReset = resetPlayer;
	mapName = mapname;

	if(CurrentStep == FadeStep::NONE)
	{
		CurrentStep = FadeStep::FADE_TO_BLACK;
		startTime = SDL_GetTicks();
		totalTime = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;

		// App->player->player.freeze = true;
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
	// ret = App->player->StartPlayer();
	
	return ret;
}