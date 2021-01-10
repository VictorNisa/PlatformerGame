#ifndef __GUIBUTTON_H__
#define __GUIBUTTON_H__

class GuiText;
class GuiImage;

enum class Button_State
{
	DEFAULT,
	HOVER,
	PRESSED
};

class GuiButton : public GuiManager
{
public:
	SDL_Rect rect;
	Button_State state;
	bool pressed;
	int XOffset;
	int YOffset;

	SString text;
	SDL_Texture* textTexture;
	SDL_Color* color;
	_TTF_Font* font;

	SDL_Texture* imageTexture;
	SDL_Rect* defaultRect;
	SDL_Rect* hoverRect;
	SDL_Rect* pressRect;
	iPoint textPos;

public:

	GuiButton(int x, int y, bool enable, bool draggable, SString text, SDL_Color* color, _TTF_Font* font, SDL_Rect* defaultRect, SDL_Rect* hoverRect, SDL_Rect* pressRect, SDL_Texture* image, SDL_Rect rect, int xOffset, int yOffset);

	void Update();

	void Draw();

	bool buttonPressed();


};

#endif // __GUIBUTTON_H__