#ifndef __GUIWINDOW_H__
#define __GUIWINDOW_H__

class GuiWindow
{
public:

	List<GuiManager*> elements;

	GuiImage* border;

	iPoint position;

	bool enabled;

	GuiButton* xButton;

	SDL_Rect borderRect = { 0,0,448,384 };

	SDL_Texture* borderTexture;

public:

	GuiWindow(int x, int y, bool enabled, GuiButton* xButton, SDL_Texture* borderTexture);

	void Update();

	void Draw();

	void addGui(GuiManager* gui, int x, int y);

	void enable();

	void disable();
};

#endif // __GUIWINDOW_H__

