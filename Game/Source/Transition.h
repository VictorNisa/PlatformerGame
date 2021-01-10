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
	bool FadeToBlack(int lvl,  bool isSaveLoad = false, float time = 0.5f);

private:
	bool SwitchMap(int level);
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
	int level;
	bool isLoad;
};

#endif //__TRANSITION_H__