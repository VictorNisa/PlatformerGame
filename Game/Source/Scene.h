#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "SDL/include/SDL_rect.h"

class GuiManager;
class GuiImage;
struct SDL_Texture;
class GuiText;
class GuiButton;
class GuiWindow;
class GuiSlider;
class Timer;
struct _TTF_Font;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:
	void HUD();


public:
	int score;
	int time;
	int hp;

private:
	int channel;
	Timer* gameTime;

	// GUI TEXT
	GuiText* timeText;
	GuiText* scoreText;
	GuiText* hpText;
	GuiText* fpsCount;

	// DEBUG TEXTURES
	SDL_Texture* pathfindingTexture;

	// GUI BUTTONS
	GuiButton* button;
	SDL_Rect buttonRect = { 0, 0, 244, 56 };
	SDL_Rect hoverRect = { 0, 60, 244, 56 };
	SDL_Rect pressRect = { 0, 120, 244, 56 };

	GuiText* filler;
	SDL_Color fillerColor = { (200),(200),(200),(255) };
	SDL_Color red = { (255),(10),(10),(255) };

	// CLOSE WINDOW BUTTON
	GuiButton* xButton;
	SDL_Rect xRect = { 0,0,62,45 };
	SDL_Rect xDefault = { 0,0,62,45 };
	SDL_Rect xHover = { 0,45,62,45 };
	SDL_Rect xPress = { 0,90,62,45 };

	// PAUSE SETTINGS
	SDL_Rect borderRect = { 0,0,448,384 };
	SDL_Texture* borderTex;

	GuiWindow* settingsWindow;
	GuiSlider* fxSlider;
	GuiSlider* musicSlider;

	// HUD GUI
	SDL_Texture* guiHudHPTex;
	//SDL_Texture* guiHudScore;
	SDL_Texture* guiHudTimeTex;

	GuiImage* guiHudHP;
	GuiImage* guiHudTime;
	//GuiText* hpText;

	List<GuiManager*>* hudList;

	_TTF_Font* font;
};

#endif // __SCENE_H__