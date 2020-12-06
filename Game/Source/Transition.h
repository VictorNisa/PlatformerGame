#ifndef __TRANSITION_H__
#define __TRANSITION_H__

#include "Module.h"
#include "SDL\include\SDL_rect.h"

class Transition : public Module
{
public:
	Transition();
	~Transition();

	bool Start();
	bool Awake(pugi::xml_node& conf);
	bool Update(float dt);
	bool FadeToBlack(const char* mapname,  bool reset_player = true, float time = 0.5f);
	bool SwitchMap(const char* mapname);

private:

	enum FadeStep
	{
		NONE,
		FADE_TO_BLACK,
		FADE_FROM_BLACK
	} CurrentStep = FadeStep::NONE;

public:

	bool playerReset = true;

private:
	
	Uint32 startTime = 0;
	Uint32 totalTime = 0;
	SDL_Rect screen;
	
	const char* mapName;
};

#endif //__TRANSITION_H__