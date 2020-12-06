#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Pathfinding.h"

Pathfinding::Pathfinding() : Module(), map(NULL), lastPath(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.create("pathfinding");
}

Pathfinding::~Pathfinding()
{
	RELEASE_ARRAY(map);
}

bool Pathfinding::CleanUp()
{
	LOG("Cleaning Pathfinding");
	lastPath.Clear();
	RELEASE_ARRAY(map);
	return true;
}

void Pathfinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;
	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

bool Pathfinding::CheckBoundaries(const iPoint& position) const
{
	bool debug = (position.x >= 0 && position.x <= (int)width &&
		position.y >= 0 && position.y <= (int)height);
	return (position.x >= 0 && position.x <= (int)width &&
		position.y >= 0 && position.y <= (int)height);
}

bool Pathfinding::IsWalkable(const iPoint& position) const
{
	uchar tile = GetTileAt(position);
	bool debug = (tile != INVALID_WALK_CODE && tile > 0);
	return tile != INVALID_WALK_CODE && tile > 0;
}

uchar Pathfinding::GetTileAt(const iPoint& position) const
{
	if (CheckBoundaries(position))
		return map[(position.y*width) + position.x];
	return INVALID_WALK_CODE;
}

const DynArray<iPoint>* Pathfinding::GetLastPath() const
{
	return &lastPath;
}

List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	List_item<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.position == point)
			return item;
		item = item->next;
	}
	return NULL;
}

List_item<PathNode>* PathList::GetLowestScore() const
{
	List_item<PathNode>* ret = NULL;
	int min = 65535;

	List_item<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// CONSTRUCTORS
PathNode::PathNode() : g(-1), h(-1), position(-1, -1), parent(NULL) {}
PathNode::PathNode(int g, int h, const iPoint& position, const PathNode* parent) : g(g), h(h), position(position), parent(parent) {}
PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), position(node.position), parent(node.parent) {}

uint PathNode::FindWalkableAdjacents(PathList& listToFill) const
{
	iPoint cell;
	uint before = listToFill.list.count();

	//   N
	// . + .
	//   .
	cell.create(position.x, position.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	//   .
	// . + .
	//   S
	cell.create(position.x, position.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	//   .
	// . + E
	//   .
	cell.create(position.x + 1, position.y);
	if (App->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	//   .
	// W + .
	//   .
	cell.create(position.x - 1, position.y);
	if (App->pathfinding->IsWalkable(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	return listToFill.list.count();
}

int PathNode::Score() const
{
	return g + h;
}

int PathNode::Calculate(const iPoint& destination)
{
	g = parent->g + 1;
	h = position.DistanceTo(destination);
	return g + h;
}



// A*

int Pathfinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	int ret = -1;
	int iterations = 0;
	PathList open;
	PathList close;
	open.list.add(PathNode(0, 0, origin, nullptr));

	while (open.list.count() != 0)
	{
		List_item<PathNode>* node = close.list.add(open.GetLowestScore()->data);
		open.list.del(open.GetLowestScore());

		if (node->data.position == destination)
		{
			lastPath.Clear();
			const PathNode *pathNode = &node->data;
			while (pathNode)
			{
				lastPath.PushBack(pathNode->position);
				pathNode = pathNode->parent;
			}

			lastPath.Flip();
			ret = lastPath.Count();
			break;
		}

		PathList neighbours;
		node->data.FindWalkableAdjacents(neighbours);

		List_item<PathNode>* item = neighbours.list.start;

		while (item != nullptr)
		{
			if (close.Find(item->data.position) != NULL)
			{
				item = item->next;
				continue;
			}

			item->data.Calculate(destination);

			if (open.Find(item->data.position) == NULL)
			{
				open.list.add(item->data);
			}
			else if (item->data.g < open.Find(item->data.position)->data.g)
			{
				open.Find(item->data.position)->data.parent = item->data.parent;
			}

			item = item->next;
			iterations++;
		}
	}
	ret = iterations;
	return ret;
}