#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Players.h"
#include "Map.h"
#include "Collisions.h"
#include "Pathfinding.h"
#include "Transition.h"
#include "Window.h"
#include <math.h>

Map::Map() : Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
Map::~Map()
{}

bool Map::Save(pugi::xml_node& node) const
{
	LOG("Saving Map...");
	node.append_child("mapName").append_attribute("fileName") = data.name;

	return true;
}

bool Map::Load(pugi::xml_node& node) 
{
	LOG("Loading Map...");

	//App->fade->FadeToBlack(node.child("mapName").attribute("fileName").as_string(), false);

	if (strcmp(node.child("mapName").attribute("fileName").as_string(), "Scene1.tmx") == 0) {
		App->fade->FadeToBlack(1, true, 1.0f);
	}
	else if (strcmp(node.child("mapName").attribute("filenNme").as_string(), "Scene1.tmx") == 0) {
		App->fade->FadeToBlack(2, true, 1.0f);
	}
	else LOG("Something's wrong!");

	return true;
}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	msToFrame = config.child("division").attribute("int").as_int();

	debug = false;

	return ret;
}

void Map::Draw()
{
	if(map_loaded == false)
		return;
	
	uint winWidth, winHeight;

	App->win->GetWindowSize(winWidth, winHeight);
	
	MapLayer* mapLayer = data.layers[0];
	List_item<MapLayer*>* layerIter = data.layers.start;

	camera_h_tile = winHeight / 32 + 1;
	camera_w_tile = winWidth / 32 + 1;
	
	while(layerIter != nullptr) 
	{ 
		uint* gid_list = layerIter->data->data;

		if (layerIter->data->navigation == true && debug == false)
		{
			break;
		}

		camera_x_tile = (-App->render->camera.x * layerIter->data->speed) / 32;
		camera_y_tile = (-App->render->camera.y * layerIter->data->speed) / 32;

		i = 0;
		j = camera_y_tile;

		for (int y = camera_y_tile; y < (camera_y_tile + camera_h_tile); y++)
		{
			for (int x = camera_x_tile; x < (camera_x_tile + camera_w_tile); x++)
			{
				i = j * data.width + x;
				App->render->Blit(data.tilesets[0]->texture, data.tilesets[0]->GetPos(x, y).x, data.tilesets[0]->GetPos(x, y).y, &data.tilesets[0]->TileRect(gid_list[i]), false, layerIter->data->speed);
			}
			j++;
		}
		layerIter = layerIter->next;
	}
}

void Map::DrawAnimation(SString name, char* tileset, fPoint& position, AnimationInfo* aInfo, bool flip)
{
	//TileSet* animTileset = nullptr;

	List_item<TileSet*>* animTileset = data.tilesets.start;

	while (animTileset != NULL)
	{
		//LOG("%d", strcmp(TilesetIter->data->name.GetString(), tileset));

		cmp = strcmp(animTileset->data->name.GetString(), tileset);

		if (cmp == 0)
		{
			//animTileset_u = animTileset->data; //store found tileset into TileSet pointer
			break;
		}

		animTileset = animTileset->next;
	}

	if (animTileset == nullptr)
		return;

	//Animations* currentAnim = nullptr;

	List_item<Animations*>* currentAnim = animTileset->data->animations.start;

	while (currentAnim)
	{
		cmp = strcmp(currentAnim->data->name.GetString(), name.GetString());

		if (cmp == 0)
		{
			break;
		}

		currentAnim = currentAnim->next;
	}
	if (currentAnim == nullptr) return;
	
	if (aInfo->prevAnimName != currentAnim->data->name.GetString())
	{
		aInfo->iter = 0;
		aInfo->frameCount = 0.0f;
	}
	aInfo->prevAnimName = currentAnim->data->name;

	SDL_Rect tmpRec = animTileset->data->PlayerTileRect(currentAnim->data->frames[aInfo->iter]);

	App->render->Blit(animTileset->data->texture, position.x, position.y, &tmpRec, flip);			

	if (aInfo->frameCount > currentAnim->data->speed/1000)
	{
		aInfo->iter++;
		aInfo->frameCount = 0.0f;
	}

	if (aInfo->iter >= currentAnim->data->nFrames) 
	{				
		aInfo->iter = 0;
	}

	aInfo->frameCount += App->dt;
}

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	List_item<MapLayer*>* item;
	item = data.layers.start;

	for (item = data.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;
		if (layer->navigation == false)
		{
			continue;
		}
		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height*sizeof(uchar));

		for (int y = 0; y < data.height; y++)
		{
			for (int x = 0; x < data.width; x++)
			{
				int i = (y * layer->width) +x;
				int tile_id = layer->GetPath(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileID(tile_id) : NULL;
				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

TileSet* Map::GetTilesetFromTileID(int id) const
{
	List_item<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;
	while (item)
	{
		if (id < item->data->firstgid)
		{
			set = item->prev->data;
			break;
		}
		set = item->data;
		item = item->next;
	}

	return set;
}


// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");
 
	List_item<TileSet*>* item;
	item = App->map->data.tilesets.start;

	while (item != NULL)
	{
		item->data->animations.clear();

		//SDL_DestroyTexture(item->data->texture);
		//RELEASE(item->data);
		item = item->next;
	}
	App->map->data.tilesets.clear();

	List_item<MapObjectgroup*>* item2;
	item2 = App->map->data.objectgroups.start;

	while (item2 != NULL)
	{
		if (item2->data->objects != NULL)
		delete[] item2->data->objects;

		RELEASE(item2->data);
		item2 = item2->next;
	}
	App->map->data.objectgroups.clear();
	
	List_item<MapLayer*>* item3;
	item3 = App->map->data.layers.start;
	while (item3 != NULL)
	{
		delete item3->data;
		item3 = item3->next;
	}
	data.layers.clear();

	return true;
}

bool Map::Load(int level)
{
	bool ret = true;

	const char* fileName;
	if (level == 1) {
		fileName = "Scene1.tmx";
	}
	else if (level == 2) {
		fileName = "Scene1.tmx";
	}
	else {
		LOG("ERROR Unrecognised level value");
		fileName = "";
	}

	SString tmp("%s%s", folder.GetString(), fileName);
	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", fileName, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		data.name = fileName;
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetAnimation(tileset, set);
		}

		data.tilesets.add(set);
	}

	
	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer")) 
	{
		MapLayer* newLayer = new MapLayer();

		if (ret == true)
		{
			ret = LoadLayer(layer, newLayer);
		}

		data.layers.add(newLayer);
	}

	// Load collider info -------------------------------------------
	pugi::xml_node objectgroup;
	for (objectgroup = map_file.child("map").child("objectgroup"); objectgroup && ret; objectgroup = objectgroup.next_sibling("objectgroup"))
	{
		MapObjectgroup* newObjectgroup = new MapObjectgroup();

		if (ret == true)
		{
			ret = LoadObjectGroup(objectgroup, newObjectgroup);
		}

		data.objectgroups.add(newObjectgroup);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", fileName);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}
		
		List_item<MapLayer*>* item_layer = data.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}

		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
		{
			App->pathfinding->SetMap(w, h, data);
		}

		RELEASE_ARRAY(data);
	}

	map_loaded = ret;
	return ret;
}

bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.startPosition.x = map.child("properties").child("property").attribute("value").as_float();
		data.startPosition.y = map.child("properties").child("property").next_sibling().attribute("value").as_float();

		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		SString debug =folder.GetString();
		debug += image.attribute("source").as_string();
		set->texture = App->tex->Load(debug.GetString());
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool Map::LoadTilesetAnimation(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;

	for (pugi::xml_node iterator_node = tileset_node.child("tile"); iterator_node; iterator_node = iterator_node.next_sibling("tile")) 
	{ 
		Animations* newAnimation = new Animations;
		
		newAnimation->id = iterator_node.attribute("id").as_uint();
		newAnimation->name = iterator_node.child("properties").child("property").attribute("name").as_string();
		newAnimation->frames = new uint[12];
		memset(newAnimation->frames, 0, 12*sizeof(uint));

		int j = 0;
		for (pugi::xml_node iterator_node_anim = iterator_node.child("animation").child("frame"); iterator_node_anim; j++ )
		{
			newAnimation->frames[j] = iterator_node_anim.attribute("tileid").as_uint();
			newAnimation->speed = iterator_node_anim.attribute("duration").as_uint();
			iterator_node_anim = iterator_node_anim.next_sibling("frame");
		}

		newAnimation->nFrames = j;
		set->animations.add(newAnimation);
	}

	return ret;
};

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	layer->name = node.attribute("name").as_string();
	layer->height = node.attribute("height").as_uint();
	layer->width = node.attribute("width").as_uint();
	layer->speed = node.child("properties").child("property").attribute("value").as_float();

	if (strcmp(node.attribute("name").as_string(), "Navigation") == 0)
	{
		layer->speed = node.child("properties").child("property").next_sibling().attribute("value").as_float();
		layer->navigation = node.child("properties").child("property").attribute("value").as_bool();
	}
	else
	{
		layer->navigation = false;
	}

	layer->data = new uint[layer->height*layer->width];
	
	memset(layer->data, 0, layer->width * layer->height * sizeof(uint));

	int i = 0;
	for (pugi::xml_node iterator_node = node.child("data").child("tile"); iterator_node; iterator_node = iterator_node.next_sibling("tile"), i++) 
	{
		layer->data[i] = iterator_node.attribute("gid").as_uint();
	}

	return true;
}

bool Map::LoadObjectGroup(pugi::xml_node& node, MapObjectgroup* objectgroup)
{

	objectgroup->name = node.attribute("name").as_string();
	objectgroup->id = node.attribute("id").as_uint();

	int AmountObjects = 0;
	for (pugi::xml_node iterator_node = node.child("object"); iterator_node; iterator_node = iterator_node.next_sibling("object"), AmountObjects++) {}

	objectgroup->objectsSize = AmountObjects;
	objectgroup->objects = new Object[AmountObjects];
	memset(objectgroup->objects, 0, AmountObjects * sizeof(Object));

	int i = 0;
	for (pugi::xml_node iterator_node = node.child("object"); iterator_node; iterator_node = iterator_node.next_sibling("object"), i++)
	{
		SDL_Rect* box = new SDL_Rect;
		
		box->x = iterator_node.attribute("x").as_uint();
		box->y = iterator_node.attribute("y").as_uint();
		box->w = iterator_node.attribute("width").as_uint();
		box->h = iterator_node.attribute("height").as_uint();

		objectgroup->objects[i].box = box;
		objectgroup->objects[i].id = iterator_node.attribute("id").as_uint();
		objectgroup->objects[i].name = iterator_node.attribute("name").as_string();

		SString type(iterator_node.attribute("type").as_string());

		if (type == "platform")
		{
			objectgroup->objects[i].type = ObjectType::PLATFORM;
		}
		else if (type == "solid")
		{
			objectgroup->objects[i].type = ObjectType::SOLID;
		}
		else if (type == "damage")
		{
			objectgroup->objects[i].type = ObjectType::DAMAGE;
		}
		else if (type == "teleport")
		{
			objectgroup->objects[i].type = ObjectType::TELEPORT;
			
			Properties::Property* temp = new Properties::Property;
			temp->name = iterator_node.child("properties").child("property").attribute("name").as_string();
			temp->data.vString = iterator_node.child("properties").child("property").attribute("value").as_string();
			objectgroup->objects[i].properties.list.add(temp);
		}
		else if (type == "enemy")
		{
			Properties::Property* temp = new Properties::Property;
			temp->name = iterator_node.child("properties").child("property").attribute("name").as_string();
			temp->data.vString = iterator_node.child("properties").child("property").attribute("value").as_string();
			objectgroup->objects[i].properties.list.add(temp);
			objectgroup->objects[i].type = ObjectType::ENEMY;
		}
		else if (type == "coin")
		{
			Properties::Property* temp = new Properties::Property;
			temp->name = iterator_node.child("properties").child("property").attribute("name").as_string();
			temp->data.vString = iterator_node.child("properties").child("property").attribute("value").as_string();
			objectgroup->objects[i].properties.list.add(temp);
			objectgroup->objects[i].type = ObjectType::COIN;
		}
		else
		{
			objectgroup->objects[i].type = ObjectType::UNKNOWN;
		}
	}
	
	return true;
}

value Properties::Get(const char* name, value* defaultValue) const
{
	List_item<Property*>* item = list.start;

	while (item)
	{
		if (item->data->name == name)
			return item->data->data;
		item = item->next;
	}

	return *defaultValue;
}

iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;
	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Map type error");
		ret.x = x;
		ret.y = y;
	}

	return ret;
}

iPoint Map::MapToWorldCentered(int x, int y) const
{
	iPoint ret;
	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width + data.tile_width / 2;
		ret.y = y * data.tile_height + data.tile_height / 2;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Map type error");
		ret.x = x;
		ret.y = y;
	}
	
	return ret;
}

iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);
	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2);
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Map type error");
		ret.x = x;
		ret.y = y;
	}

	return ret;
}