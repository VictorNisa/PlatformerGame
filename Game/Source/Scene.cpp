#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Scene.h"
#include "Transition.h"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "Gui.h"
//#include "MenuScene.h"

Scene::Scene() : Module()
{
	name.create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	if (App->map->Load(1) == true)
	{

	}

	gameTime = new Timer;

	pathfindingTexture = App->tex->Load("Assets/Maps/PathfindingTexture.png");

	App->audio->PlayMusic("Assets/Audio/Music/Bgm.ogg" , 0.0f);

	// HUD
	font = App->font->Load("Assets/Fonts/Minecraft.ttf", 20);
	borderTex = App->tex->Load("Assets/Gui/GuiWindow.png");
	time = 300;
	score = 0;
	hp = 10;

	guiHudHPTex = App->tex->Load("Assets/Gui/GuiHudHP.png");
	guiHudTimeTex = App->tex->Load("Assets/Gui/GuiHudTime.png");

	guiHudHP = App->gui->CreateImage(10, 10, true, false, { 0,0,132,88 }, guiHudHPTex);
	guiHudTime = App->gui->CreateImage(1050, 10, true, false, { 0,0,158,104 }, guiHudTimeTex);

	// GUI TEXT
	timeText = App->gui->CreateText(72, 59, true, false, "0", &fillerColor, font);
	scoreText = App->gui->CreateText(1093, 76, true, false, "0", &fillerColor, font);
	fpsCount = App->gui->CreateText(1143, 19, true, false, "0", &red, font);

	// GUI SETTINGS
	SDL_Texture* xButtonTexture = App->tex->Load("Assets/Gui/GuiXButton.png");
	xButton = App->gui->CreateButton(0, 0, false, false, "", &fillerColor, font, &xDefault, &xHover, &xPress, xButtonTexture, xRect, 0, 0);
	settingsWindow = App->gui->CreateGuiWindow(275, 200, false, xButton, borderTex);

	SDL_Texture* icons = App->tex->Load("Assets/Gui/GuiSlider.png");
	int currentVol = ((float)App->audio->SetFxVol(-1.0f) / 128.0f) * 152.0f;

	fxSlider = App->gui->CreateSlider(0, 0, false, false, currentVol);
	settingsWindow->addGui(fxSlider, 90, 125);
	settingsWindow->addGui(App->gui->CreateText(0, 0, false, false, "fx", &fillerColor, font), 90, 90);
	settingsWindow->addGui(App->gui->CreateImage(0, 0, false, false, SDL_Rect{ 76, 48, 19, 22 }, icons), 104, 138);
	
	currentVol = ((float)App->audio->SetMusicVol(-1.0f) / 128.0f) * 152.0f;

	musicSlider = App->gui->CreateSlider(0, 0, false, false, currentVol);
	settingsWindow->addGui(musicSlider, 90, 275);
	settingsWindow->addGui(App->gui->CreateText(0, 0, false, false, "music", &fillerColor, font), 90, 240);
	settingsWindow->addGui(App->gui->CreateImage(0, 0, false, false, SDL_Rect{ 95, 48, 19, 22 }, icons), 104, 288);
	settingsWindow->addGui(App->gui->CreateText(0, 0, false, false, "Sound Settings", &fillerColor, font), 150, 20);

	gameTime->Start();
	
	// HUD END



	pathfindingTexture = App->tex->Load("Assets/Maps/PathfindingTexture.png");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

void Scene::HUD()
{
	char str[6];

	time = (300 - (int)gameTime->ReadSec());
	if (time < 0) {
		time = 0;

		//App->mainmenu->activate();
		App->scene->deactivate();
		App->entities->deactivate();
		App->collisions->deactivate();
	}

	sprintf_s(str, "%d", time);
	timeText->UpdateText(str);

	sprintf_s(str, "%d", App->framesOnLastUpdate);
	fpsCount->UpdateText(str);

	sprintf_s(str, "%d", score);
	scoreText->UpdateText(str);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		settingsWindow->enable();
	}
	App->audio->SetFxVol(fxSlider->SliderValue());
	App->audio->SetMusicVol(musicSlider->SliderValue());
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame();

	if(App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame();

	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->fade->FadeToBlack(1);

	if(App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->fade->FadeToBlack(1);

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) 
	{
		if(App->map->data.name == "Scene1.tmx") App->fade->FadeToBlack(1);
		else if (App->map->data.name == "Scene1.tmx") App->fade->FadeToBlack(1);
	}
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		if (App->frameRate == 0)
		{
			App->frameRate = 30;
		}
		else
		{
			App->frameRate = 0;
		}
	}


	App->map->Draw();

	HUD();

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	//if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	//	ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
