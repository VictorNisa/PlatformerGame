#include "Gui.h"

GuiText::GuiText(int x, int y, bool active, bool draggable, SString text, SDL_Color* color, _TTF_Font* font) : GuiManager(x, y, active, draggable), text(text), color(color), font(font)
{
	texture = App->font->Print(text.GetString(), *color, font);
}

void GuiText::Update()
{

}

void GuiText::Draw()
{
	App->render->Blit(texture, position.x, position.y, NULL, false, 0.f);
}

void GuiText::UpdateText(SString text)
{
	SDL_DestroyTexture(texture);
	texture = App->font->Print(text.GetString(), *color, font);
}