#include "App.h"
#include "Gui.h"

GuiImage::GuiImage(int x, int y, bool active, bool draggable, SDL_Rect rect, SDL_Texture* image) : GuiManager(x, y, active, draggable), rect(rect), image(image)
{

}

// METHODS

void GuiImage::Update()
{

}

void GuiImage::Draw()
{
	App->render->Blit(image, position.x, position.y, &rect, false, 0.f);
}

void GuiImage::UpdatePosition(int x, int y) {
	if (!draggable) return;
	position.x = x;
	position.y = y;
};