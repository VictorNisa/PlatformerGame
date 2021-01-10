#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Input.h"
#include "Gui.h"


Gui::Gui() : Module()
{
	name.create("gui");
}

// Destructor
Gui::~Gui()
{}

// Called before render is available
bool Gui::Awake(pugi::xml_node& conf)
{
	bool ret = true;
	clickAudio = App->audio->LoadFx("Assets/Audio/Fx/ButtonClick.wav");
	return ret;
}

// Called before the first frame
bool Gui::Start()
{
	clickAudio = App->audio->LoadFx("Assets/Audio/Fx/ButtonClick.wav");

	consoleFont = App->font->Load("Assets/Fonts/Minecraft.ttf", 20);
	//console = App->gui->CreateConsole(150, 0, false, false, &white, consoleFont, consoleRect);


	return true;
}

// Update all guis
bool Gui::PreUpdate()
{
	List_item<GuiManager*>* it = GuiList.start;

	while (it != nullptr)
	{
		if (it->data->IsEnabled() == true && it->data->updateIt) it->data->Update();

		it = it->next;
	}

	List_item<GuiWindow*>* winit = WindowsList.end;

	while (winit != nullptr)
	{
		if (winit->data->enabled)
		{
			winit->data->Update();

			disableGui();

			return true;
		}

		winit = winit->prev;
	}

	enableGui();

	return true;
}

// Update all guis
bool Gui::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		List_item<GuiManager*>* it = GuiList.start;

		while (it != nullptr)
		{
			it->data->debug = !it->data->debug;
			it = it->next;
		}
	}

	//if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN && !console->IsEnabled())
	//{
	//	console->Enable();
	//	console->text.Clear();
	//	SDL_StartTextInput();
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN && console->IsEnabled())
	//{
	//	console->Disable();
	//	SDL_StopTextInput();
	//}

	return true;
}

// Called after all Updates
bool Gui::PostUpdate()
{
	List_item<GuiManager*>* it = GuiList.start;

	while (it != nullptr)
	{

		if (it->data->IsEnabled() == true)  it->data->Draw();

		it = it->next;
	}

	List_item<GuiWindow*>* winit = WindowsList.start;

	while (winit != nullptr)
	{

		if (winit->data->enabled)
		{
			winit->data->Draw();
		}
		winit = winit->next;
	}

	return true;
}

// Called before quitting
bool Gui::CleanUp()
{
	LOG("Freeing GUI");

	return true;
}

GuiText* Gui::CreateText(int x, int y, bool enabled, bool draggable, SString text, SDL_Color* color, _TTF_Font* font)
{
	GuiText* ret = new GuiText(x, y, enabled, draggable, text, color, font);

	GuiList.add(ret);

	return ret;
}

GuiImage* Gui::CreateImage(int x, int y, bool enabled, bool draggable, SDL_Rect rect, SDL_Texture* image)
{
	GuiImage* ret = new GuiImage(x, y, enabled, draggable, rect, image);

	GuiList.add(ret);

	return ret;
}

GuiButton* Gui::CreateButton(int x, int y, bool active, bool draggable, SString text, SDL_Color* color, _TTF_Font* font, SDL_Rect* defaultRect, SDL_Rect* hoverRect, SDL_Rect* pressRect, SDL_Texture* image, SDL_Rect rect, int xOffset, int yOffset)
{
	GuiButton* ret = new GuiButton(x, y, active, draggable, text, color, font, defaultRect, hoverRect, pressRect, image, rect, xOffset, yOffset);

	GuiList.add(ret);

	return ret;
}

GuiWindow* Gui::CreateGuiWindow(int x, int y, int enabled, GuiButton* xButton, SDL_Texture* boardTexture)
{
	GuiWindow* ret = new GuiWindow(x, y, enabled, xButton, boardTexture);

	WindowsList.add(ret);

	return ret;
}

GuiSlider* Gui::CreateSlider(int x, int y, bool active, bool draggable, int sliderPosition)
{
	GuiSlider* ret = new GuiSlider(x, y, active, draggable, sliderPosition);

	GuiList.add(ret);

	return ret;
}

//GuiConsole* Gui::CreateConsole(int x, int y, bool enabled, bool draggable, SDL_Color* color, _TTF_Font* font, SDL_Rect rect)
//{
//	GuiConsole* ret = new GuiConsole(x, y, enabled, draggable, color, font, rect);
//
//	Guilist.add(ret);
//
//	return ret;
//}

void Gui::disableGui()
{
	List_item<GuiManager*>* it = GuiList.start;

	while (it != nullptr)
	{

		if (it->data->IsEnabled() == true)  it->data->updateIt = false;

		it = it->next;
	}
}

void Gui::enableGui()
{
	List_item<GuiManager*>* it = GuiList.start;

	while (it != nullptr)
	{

		if (it->data->IsEnabled() == true)  it->data->updateIt = true;

		it = it->next;
	}
}