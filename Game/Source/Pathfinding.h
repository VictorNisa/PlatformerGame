#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include "Module.h"
#include "Point.h"
#include "DynArray.h"

#define DEFAULT_PATH_LENGTH
#define INVALID_WALK_CODE 255

class Pathfinding : public Module
{
public:
	Pathfinding();
	~Pathfinding();

	bool CleanUp();

	void SetMap(uint width, uint height, uchar* data);

	int CreatePath(const iPoint& origin, const iPoint& destination);
	
	const DynArray<iPoint>* GetLastPath() const;

	// True: Position is inside the map boundaries
	// False: Position is not inside the map boundaries
	bool CheckBoundaries(const iPoint& position) const;
	bool IsWalkable(const iPoint& position) const;
	uchar GetTileAt(const iPoint& position) const;

private:
	uint width;
	uint height;
	uchar* map;
	DynArray<iPoint> lastPath;
};

struct PathList;

struct PathNode
{
	PathNode();
	PathNode(int g, int h, const iPoint& position, const PathNode* parent);
	PathNode(const PathNode& node);

	uint FindWalkableAdjacents(PathList& listToFill) const;
	int Score() const;
	int Calculate(const iPoint& destination);

	int g;
	int h;
	iPoint position;
	const PathNode* parent;
};

struct PathList
{
	List_item<PathNode>* Find(const iPoint& point) const;
	List_item<PathNode>* GetLowestScore() const;
	List<PathNode> list;
};

#endif // __PATHFINDING_H__
