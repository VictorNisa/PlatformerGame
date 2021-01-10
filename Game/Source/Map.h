#ifndef __MAP_H__
#define __MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "List.h"
#include "Point.h"
#include "Module.h"
#include "Collisions.h"
#include "Log.h"
#include "EntityManager.h"

// ----------------------------------------------------
union value
{
	const char* vString;
	int vInt;
	float vFloat;
};

struct Properties
{
	struct Property
	{
		SString name;
		value data;
	};

	~Properties()
	{
		List_item<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	value Get(const char* name, value* defaultValue = nullptr) const;

	List<Property*>	list;
};

// Object for Colliders
enum class ObjectType
{
	UNKNOWN = 0,
	PLATFORM,
	SOLID,
	DAMAGE,
	PLAYER,
	ENEMY,
	COIN,
	TELEPORT,
	ATTACK,

	MAX_OBJECTS = 7
};

struct Animations
{
	SString	name;
	uint id;
	uint nFrames;
	uint* frames;
	//p2SString prevAnimName = "idle";
	float speed;
	//float frameCount = 0.0f;
};

struct Object 
{
	uint id;
	SString name;
	ObjectType type;
	SDL_Rect* box;
	SDL_Texture* texture;
	Properties properties;
};

struct MapObjectgroup 
{
	uint id;
	SString name;
	Object* objects;
	uint objectsSize;
};

// ----------------------------------------------------
struct MapLayer 
{
	SString name;
	uint width;
	uint height;
	uint* data; 
	float speed;
	bool navigation;
	inline uint Get(int x, int y) const
	{
		return x + y * width;
	}
	inline uint GetPath(int x, int y) const
	{
		return data[(y*width) + x];
	}
};


// ----------------------------------------------------
struct TileSet
{
	SDL_Rect Tilerect ;

	SDL_Rect TileRect(uint tile_id) 
	{
		SDL_Rect ret = Tilerect; 

		int x = ((tile_id - firstgid) % num_tiles_width);
		int y = ((tile_id - firstgid) / num_tiles_width);

		ret.x = x*tile_width  + margin + spacing*x;
		ret.y = y*tile_height + margin + spacing*y;
		ret.w = tile_width;
		ret.h = tile_height;

		return ret;
	}
	
	SDL_Rect PlayerTilerect;

	SDL_Rect PlayerTileRect(uint tile_id) 
	{
		SDL_Rect ret = PlayerTilerect;

		int num_t_width = tex_width / tile_width;
		int num_t_height = tex_height / tile_height;

		int x = tile_id % num_t_width;
		int y = tile_id / num_t_width;

		ret.x = x * tile_width;
		ret.y = y * tile_height;
		ret.w = tile_width;
		ret.h = tile_height;

		return ret;
	}

	inline Point<uint> GetPos(uint x, uint y) 
	{
		Point<uint> ret;
		ret.x = x * tile_width;
		ret.y = y * tile_height;

		return ret;
	}

	SString name;
	int firstgid;
	int margin;
	int spacing;
	int tile_width;
	int tile_height;
	SDL_Texture* texture;
	int tex_width;
	int tex_height;
	int num_tiles_width;
	int num_tiles_height;
	int offset_x;
	int offset_y;
	List<Animations*> animations;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int width;
	int height;
	int tile_width;
	int tile_height;
	
	const char* name;
	fPoint startPosition;
	
	SDL_Color background_color;
	MapTypes type;
	List<TileSet*> tilesets;
	List<MapLayer*> layers;
	List<MapObjectgroup*> objectgroups;
};

// ----------------------------------------------------
class Map : public Module
{
public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	void DrawAnimation(SString name,char* tileset, fPoint& position, AnimationInfo* aInfo, bool flip=false);

	// Called before quitting
	bool CleanUp();

	// Load new map

	bool Load(int level);
	
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	iPoint MapToWorldCentered(int x, int y) const;
	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;
	TileSet* GetTilesetFromTileID(int id) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetAnimation(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadObjectGroup(pugi::xml_node& node, MapObjectgroup* objectgroup);

public:

	MapData data;
	bool debug;

	// Collider camera_collider;
	//SDL_Rect tile_rect;

private:

	// int i = 0;

	// int frameCount = 1;
	int cmp;
	int i;
	int j;
	int camera_x_tile;
	int camera_y_tile;
	int camera_w_tile;
	int camera_h_tile;
	int msToFrame;

	pugi::xml_document map_file;
	SString folder;
	bool map_loaded;
};

#endif // __MAP_H__