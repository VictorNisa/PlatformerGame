#include "App.h"
#include "Gui.h"
#include "Input.h"

GuiSlider::GuiSlider(int x, int y, bool active, bool draggable, int sliderPosition) : GuiManager(x, y, active, draggable)
{
	clickableRect = { x + 46, y + 4, 188, 40 };
	thumbOffset = 46;
	sliderPos = sliderPosition;
	texture = App->tex->Load("Assets/gui/GuiSlider.png");

	bar = { 0, 0, 280, 48 };
	thumb = { 0, 48, 36, 28 };
	thumbHovered = { 38, 48, 36, 28 };

	clickableRect = { position.x + 46, position.y + 4, 188, 40 };
}

void GuiSlider::Update()
{
	clickableRect = { position.x + 46, position.y + 4, 188, 40 };
	hovered = false;
	int x, y;

	App->input->GetMousePosition(x, y);

	if (x > clickableRect.x && x < clickableRect.x + clickableRect.w
		&& y > clickableRect.y && y < clickableRect.y + clickableRect.h)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			hovered = true;
			sliderPos = x - position.x - thumbOffset;
			if (sliderPos < 0)	sliderPos = 0;
			else if (sliderPos > 152) sliderPos = 152;
		}
	}
}

void GuiSlider::Draw() {

	App->render->Blit(texture, position.x, position.y, &bar, false, .0f);

	if (hovered) {
		App->render->Blit(texture, position.x + thumbOffset + sliderPos, position.y + 10, &thumbHovered, false, .0f);
	}
	else App->render->Blit(texture, position.x + thumbOffset + sliderPos, position.y + 10, &thumb, false, .0f);

	if (debug)
	{
		App->render->DrawQuad(clickableRect, 0, 255, 255, 50, true, false);
	}
}

float GuiSlider::SliderValue() {

	return ((float)sliderPos / 152.0f);
}