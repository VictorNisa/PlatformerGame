#include "Gui.h"

GuiWindow::GuiWindow(int x, int y, bool enabled, GuiButton* xButton, SDL_Texture* borderTexture) : position(x, y), enabled(enabled), xButton(xButton), borderTexture(borderTexture)
{
	addGui(xButton, 0, 0);
	border = App->gui->CreateImage(0, 100, false, false, borderRect, borderTexture);

	border->position = position;
	xButton->position.x = position.x + border->rect.w - xButton->rect.w - 8;
	xButton->position.y = position.y + 6;
}

void GuiWindow::Update()
{
	border->position = position;

	xButton->position.x = position.x + border->rect.w - xButton->rect.w - 8;
	xButton->position.y = position.y + 6;

	List_item<GuiManager*>* elementsIt = elements.start;

	while (elementsIt != NULL)
	{
		elementsIt->data->Update();
		elementsIt = elementsIt->next;
	}

	if (xButton->buttonPressed())
	{
		disable();
	}
}

void GuiWindow::Draw()
{
	border->Draw();

	List_item<GuiManager*>* elementsIt = elements.start;

	while (elementsIt != NULL)
	{
		elementsIt->data->Draw();
		elementsIt = elementsIt->next;
	}
}

void GuiWindow::addGui(GuiManager* gui, int x, int y)
{
	elements.add(gui);

	gui->position.x = position.x + x;
	gui->position.y = position.y + y;
}

void GuiWindow::enable()
{
	enabled = true;
}

void GuiWindow::disable()
{
	enabled = false;
}