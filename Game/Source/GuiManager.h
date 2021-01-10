#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

enum class GuiType
{
	TEXT = 0,
	IMAGE,
	BUTTON,
	UNKNOWN = 3
};


class GuiManager
{

public:

	iPoint position;
	iPoint startingPosition;

	//properties
	bool draggable;
	bool enabled;
	bool updateIt;

	bool debug;

public:

	GuiManager(int x, float y, bool active, bool draggable);

	virtual void Update();

	virtual void Draw();

	bool IsEnabled();

	void Enable();

	void Disable();

};

#endif // __GUIMANAGER_H__