#include "App.h"
#include "Gui.h"
#include "Input.h"
#include "Point.h"
#include "Log.h"


GuiButton::GuiButton(int x, int y, bool enabled, bool draggable, SString text, SDL_Color* color, _TTF_Font* font, SDL_Rect* defaultRect, SDL_Rect* hoverRect, SDL_Rect* pressRect, SDL_Texture* image, SDL_Rect rect, int xOffset, int yOffset)
	: GuiManager(x, y, enabled, draggable), font(font), defaultRect(defaultRect), hoverRect(hoverRect), pressRect(pressRect), imageTexture(image), text(text), color(color), rect(rect), textPos(xOffset, yOffset)
{

	textTexture = App->font->Print(text.GetString(), *color, font);
	XOffset = xOffset;
	YOffset = yOffset;
	rect.x = position.x;
	rect.y = position.y;
}

void GuiButton::Update()
{
	rect.x = position.x;
	rect.y = position.y;

	textPos.x = position.x + XOffset;
	textPos.y = position.y + YOffset;

	pressed = false;

	state = Button_State::DEFAULT;

	int x, y;

	App->input->GetMousePosition(x, y);

	if (x > rect.x && x < rect.x + rect.w
		&& y > rect.y && y < rect.y + rect.h)
	{
		state = Button_State::HOVER;

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			state = Button_State::PRESSED;
		}
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && !pressed)
		{
			LOG("Button pressed");
			App->audio->PlayFx(App->gui->clickAudio);
			pressed = true;
		}
	}
}

void GuiButton::Draw()
{
	switch (state)
	{
	case Button_State::DEFAULT:

		App->render->Blit(imageTexture, position.x, position.y, defaultRect, false, 0.f);
		break;

	case Button_State::HOVER:

		App->render->Blit(imageTexture, position.x, position.y, hoverRect, false, 0.f);
		break;

	case Button_State::PRESSED:

		App->render->Blit(imageTexture, position.x, position.y, pressRect, false, 0.f);
		break;
	}

	if (debug)
	{
		App->render->DrawQuad(rect, 0, 255, 255, 50);
	}
	if (text != "")
	{
		App->render->Blit(textTexture, textPos.x, textPos.y, NULL, false, 0.f);
	}
}

bool GuiButton::buttonPressed()
{
	return pressed;
}