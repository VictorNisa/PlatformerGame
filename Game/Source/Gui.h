#ifndef __GUI_H__
#define __GUI_H__

#include "App.h"
#include "Module.h"

#include "SString.h"
#include "Point.h"
#include "List.h"
#include "SDL/include/SDL_rect.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Audio.h"

#include "GuiManager.h"
#include "GuiButton.h"
#include "GuiImage.h"
#include "GuiText.h"
//#include "GuiConsole.h"
#include "GuiSlider.h"
#include "GuiWindow.h"

#define CURSOR_WIDTH 2

class Gui : public Module
{
public:

	Gui();

	virtual ~Gui();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	GuiText* CreateText(int x, int y, bool active, bool draggable, SString text, SDL_Color* color, _TTF_Font* font);

	GuiImage* CreateImage(int x, int y, bool active, bool draggable, SDL_Rect rect, SDL_Texture* image);

	GuiButton* CreateButton(int x, int y, bool active, bool draggable, SString text, SDL_Color* color, _TTF_Font* font, SDL_Rect* defaultRect, SDL_Rect* hoverRect, SDL_Rect* pressRect, SDL_Texture* image, SDL_Rect rect, int xOffset, int yOffset);

	GuiWindow* CreateGuiWindow(int x, int y, int enabled, GuiButton* xButton, SDL_Texture* borderTexture);

	//GuiConsole* CreateConsole(int x, int y, bool enabled, bool draggable, SDL_Color* color, _TTF_Font* font, SDL_Rect rect);

	GuiSlider* CreateSlider(int x, int y, bool active, bool draggable, int sliderPosition = 76);

	void disableGui();

	void enableGui();

private:

	List<GuiManager*> GuiList;

public:

	List<GuiWindow*> WindowsList;
	int clickAudio;

	//Console
	//GuiConsole* console;

	SDL_Color white = { (255),(255),(255),(255) };

	_TTF_Font* consoleFont;

	//SDL_Rect console_rect = { 0,0,700,300 };

};

#endif // __GUI_H__