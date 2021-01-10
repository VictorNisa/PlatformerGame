#ifndef __GUITEXT_H__
#define __GUITEXT_H__

struct SDL_Texture;
struct SDL_Color;
struct _TTF_Font;

class GuiText : public GuiManager
{
	SString text;
	SDL_Texture* texture;
	SDL_Color* color;
	_TTF_Font* font;

public:

	GuiText(int x, int y, bool active, bool draggable, SString text, SDL_Color* color, _TTF_Font* font);

	void Update();
	void Draw();
	void UpdateText(SString text);
};

#endif // __GUITEXT_H__
