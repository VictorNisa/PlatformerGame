#ifndef __GUISLIDER_H__
#define __GUISLIDER_H__

class GuiSlider : public GuiManager
{
public:

	SDL_Rect clickableRect;
	SDL_Rect bar;
	SDL_Rect thumb;
	SDL_Rect thumbHovered;
	SDL_Texture* texture;
	bool hovered;
	int thumbOffset;
	int sliderPos;

public:

	GuiSlider(int x, int y, bool active, bool draggable, int sliderPosition = 76);

	void Update();
	void Draw();
	float SliderValue();
};

#endif //__GUISLIDER_H__
