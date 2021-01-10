#ifndef __GUIIMAGE_H__
#define __GUIIMAGE_H__

class GuiImage : public GuiManager
{
public:
	SDL_Rect rect;
	SDL_Texture* image;

public:

	GuiImage(int x, int y, bool active, bool draggable, SDL_Rect rect, SDL_Texture* image = nullptr);

	void Update();
	void Draw();
	void UpdatePosition(int x, int y);
};
#endif // __GUIIMAGE_H__