#include "Gui.h"


GuiManager::GuiManager(int x, float y, bool enabled, bool draggable) : position(x, y), enabled(enabled), draggable(draggable)
{
	debug = false;

	startingPosition = position;
}

void GuiManager::Update()
{

}

void GuiManager::Draw()
{

}

bool GuiManager::IsEnabled()
{
	return enabled;
}

void GuiManager::Enable()
{
	enabled = true;
}

void GuiManager::Disable()
{
	enabled = false;
}