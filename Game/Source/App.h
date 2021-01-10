#ifndef __APP_H__
#define __APP_H__

#include "List.h"
#include "Module.h"
#include "PugiXml\src\pugixml.hpp"
#include "PerfTimer.h"
#include "Timer.h"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene;
class Map;
class Pathfinding;
class Players;
class Collisions;
class Transition;
class EntityManager;
class Fonts;
class Gui;
//class MenuScene;

class Application
{
public:

	// Constructor
	Application(int argc, char* args[]);

	// Destructor
	virtual ~Application();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame();
	void SaveGame() const;
	void GetSaveGames(List<SString>& list_to_fill) const;

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

private:



	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

public:

	// Modules
	Window* win;
	Input* input;
	Render* render;
	Textures* tex;
	Audio* audio;
	Scene* scene;
	Map* map;
	Pathfinding* pathfinding;
	Players* player;
	Collisions* collisions;
	Transition* fade;
	EntityManager* entities;
	Gui* gui;
	Fonts* font;
	//MenuScene* menu;
	

	float dt;
	int frameRate;
	bool doLogic = false;
	pugi::xml_node config;
	uint32 lastSecFrameCount = 0;
	uint32 framesOnLastUpdate;
private:

	List<Module*> modules;
	uint frames;
	// float dt;
	int argc;
	char** args;

	SString title;
	SString organization;

	float accumulatedTime = 0.0f;

	mutable bool wantToSave;
	bool wantToLoad;
	SString loadGame; 
	mutable SString saveGame;

	PerfTimer pTimer;
	PerfTimer delayTimer;
	uint64 frameCount = 0;
	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;

	uint32 prevLastSecFrameCount = 0;


};

extern Application* App; // No student is asking me about that ... odd :-S

#endif