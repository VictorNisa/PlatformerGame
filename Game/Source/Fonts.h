#ifndef __FONTS_H__
#define __FONTS_H__

#include "Module.h"
#include "SDL\include\SDL_pixels.h"

#define DEFAULT_FONT "Assets/Fonts/Opensans/OpenSansRegular.ttf"
#define DEFAULT_FONT_SIZE 12

struct SDL_Texture;
struct _TTF_Font;

class Fonts : public Module
{
public:

	Fonts();

	virtual ~Fonts();
	bool Awake(pugi::xml_node&);
	bool CleanUp();

	_TTF_Font* const Load(const char* path, int size = 12);

	SDL_Texture* Print(const char* text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;

public:

	List<_TTF_Font*> fonts;
	_TTF_Font* default;
};


#endif // __FONTS_H__