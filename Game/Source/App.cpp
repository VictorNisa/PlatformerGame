#include <iostream> 

#include "Defs.h"
#include "Log.h"

#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"
#include "Map.h"
#include "Pathfinding.h"
#include "EntityManager.h"
//#include "Players.h"
#include "Collisions.h"
#include "Transition.h"
#include "App.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "Gui.h"
#include "Fonts.h"
//#include "MenuScene.h"

// Constructor
Application::Application(int argc, char* args[]) : argc(argc), args(args)
{
	frames = 0;
	wantToSave = wantToLoad = false;

	input = new Input();
	win = new Window();
	render = new Render();
	tex = new Textures();
	audio = new Audio();
	scene = new Scene();
	map = new Map();
	pathfinding = new Pathfinding();
	entities = new EntityManager();
	// player = new Players();
	gui = new Gui();
	collisions = new Collisions();
	fade = new Transition();
	font = new Fonts();
	//menu = new MenuScene();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(scene);
	//AddModule(menu);
	AddModule(entities);
	// AddModule(player);
	AddModule(gui);
	AddModule(collisions);
	AddModule(fade);
	AddModule(font);

	// Render last to swap buffer
	AddModule(render);

	//scene->deactivate();
	//collisions->deactivate();
	//entities->deactivate();
}

// Destructor
Application::~Application()
{
	// release modules
	List_item<Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void Application::AddModule(Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool Application::Awake()
{
	pugi::xml_document configFile;
	// pugi::xml_node config;
	pugi::xml_node appConfig;

	bool ret = false;
		
	config = LoadConfig(configFile);

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		appConfig = config.child("app");
		title.create(appConfig.child("title").child_value());
		organization.create(appConfig.child("organization").child_value());
		loadGame = config.child("fileSystem").child("loadTo").child_value();
		saveGame = config.child("fileSystem").child("saveTo").child_value();

		if (appConfig.attribute("framerateCap").as_int() == NULL)
		{
			frameRate = 0;
		}
		else
		{
			frameRate = appConfig.attribute("framerateCap").as_int();
		}
	}

	if(ret == true)
	{
		List_item<Module*>* item;
		item = modules.start;

		while(item != NULL && ret == true)
		{
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	return ret;
}

// Called before the first frame
bool Application::Start()
{
	bool ret = true;
	List_item<Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		if (item->data->active)
		{
			ret = item->data->Start();
		}
		item = item->next;
	}

	return ret;
}

// Called each loop iteration
bool Application::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node Application::LoadConfig(pugi::xml_document& configFile) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = configFile.load_file("config.xml");

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = configFile.child("config");

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	frameCount++;
	lastSecFrameCount++;

	dt = frameTime.ReadSec();

	frameTime.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if(wantToSave == true)
		SavegameNow();

	if(wantToLoad == true)
		LoadGameNow();

	if (lastSecFrameTime.Read() > 1000)
	{
		lastSecFrameTime.Start();
		prevLastSecFrameCount = lastSecFrameCount;
		lastSecFrameCount = 0;
	}

	float avgFps = float(frameCount) / startupTime.ReadSec();
	float secondsSinceStartup = startupTime.ReadSec();
	uint32 lastFrameMs = frameTime.Read();
	framesOnLastUpdate = prevLastSecFrameCount;

	static char title[256];
	//sprintf_s(title, 256, "Av.FPS: %.2f Last Frame Ms: %02u Last sec frames: %i  Time since startup: %.3f Frame Count: %lu ",
	//	avgFps, lastFrameMs, framesOnLastUpdate, secondsSinceStartup, frameCount);
	sprintf_s(title, 256, "Hellbound, by Victor Nisa");
	App->win->SetTitle(title);

	uint32 frametimeTmp = frameTime.Read();

	delayTimer.Start();

	if (frameRate != 0)
	{
		if (1000 / frameRate > frameTime.Read())
		{

			SDL_Delay((1000 / frameRate) - frameTime.Read());

		}
	}
}

// Call modules before each loop iteration
bool Application::PreUpdate()
{
	bool ret = true;
	List_item<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool Application::DoUpdate()
{
	bool ret = true;
	List_item<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	if (doLogic == true) {
		accumulatedTime = 0.0f;
		doLogic = false;
	}

	accumulatedTime += dt;

	if (accumulatedTime >= 0.1f)
	{
		doLogic = true;
	}

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool Application::PostUpdate()
{
	bool ret = true;
	List_item<Module*>* item;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool Application::CleanUp()
{
	bool ret = true;
	List_item<Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int Application::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* Application::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* Application::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* Application::GetOrganization() const
{
	return organization.GetString();
}

// Load / Save
void Application::LoadGame()
{
	wantToLoad = true;
}

// ---------------------------------------
void Application::SaveGame() const
{
	wantToSave = true;
}

// ---------------------------------------
void Application::GetSaveGames(List<SString>& list_to_fill) const
{

}

bool Application::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(loadGame.GetString());

	if(result != NULL)
	{
		LOG("Loading new Game State from %s...", loadGame.GetString());

		root = data.child("gameState");

		List_item<Module*>* item = modules.start;
		ret = true;

		while(item != NULL && ret == true)
		{
			ret = item->data->Load(root.child(item->data->name.GetString()));
			item = item->next;
		}

		data.reset();
		if(ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", loadGame.GetString(), result.description());

	wantToLoad = false;
	return ret;
}

bool Application::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", saveGame.GetString());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;
	
	root = data.append_child("gameState");

	List_item<Module*>* item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Save(root.append_child(item->data->name.GetString()));
		item = item->next;
	}

	if(ret == true)
	{
		data.save_file(saveGame.GetString());
		LOG("... finished saving", );
	}
	else
		LOG("Save process halted from an error in module %s", (item != NULL) ? item->data->name.GetString() : "unknown");

	data.reset();
	wantToSave = false;
	return ret;
}