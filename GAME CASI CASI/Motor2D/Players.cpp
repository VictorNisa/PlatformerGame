#include "App.h"
#include "Players.h"
#include "Module.h"
#include "Point.h"
#include "Render.h"
#include "Input.h"
#include "Transition.h"
#include "Animation.h"
#include "Collisions.h"
#include "Log.h"
#include "Audio.h"

Players::Players() 
{
	name.create("player");
};

bool Players::Save(pugi::xml_node& node) const 
{
	LOG("Saving Player...");
	pugi::xml_node points = node.append_child("points");
	
	points.append_child("position").append_attribute("x") = player.position.x;
	points.child("position").append_attribute("y") = player.position.y;
	
	points.append_child("prevposition").append_attribute("x") = player.prevposition.x;
	points.child("prevposition").append_attribute("y") = player.prevposition.y;
	
	points.append_child("lastGroundedPos").append_attribute("x") = player.lastGroundedPos.x;
	points.child("lastGroundedPos").append_attribute("y") = player.lastGroundedPos.y;

	pugi::xml_node flags = node.append_child("flags");
	flags.append_attribute("able_to_jump") = player.able_to_jump;
	flags.append_attribute("able_to_dash") = player.able_to_dash;
	flags.append_attribute("dashing") = player.dashing;
	flags.append_attribute("jumping") = player.jumping;
	flags.append_attribute("drop_plat") = player.drop_plat;
	flags.append_attribute("playerGrounded") = player.playerGrounded;
	flags.append_attribute("flip") = player.flip;
	flags.append_attribute("godMode") = player.godMode;

	node.append_attribute("playerstate") = player.playerState;

	return true;
}

bool Players::Load(pugi::xml_node& node)
{
	LOG("Loading Player...");

	player.justLoaded = true;

	pugi::xml_node points = node.child("points");

	player.position.x = points.child("position").attribute("x").as_float();
	player.position.y = points.child("position").attribute("y").as_float();

	player.prevposition.x = points.child("prevposition").attribute("x").as_float();
	player.prevposition.y = points.child("prevposition").attribute("y").as_float();

	player.lastGroundedPos.x = points.child("lastGroundedPos").attribute("x").as_float();
	player.lastGroundedPos.y = points.child("lastGroundedPos").attribute("y").as_float();
	
	pugi::xml_node flags = node.child("flags");
	player.able_to_dash		= flags.attribute("able_to_dash").as_bool();
	player.able_to_jump		= flags.attribute("able_to_jump").as_bool();
	player.drop_plat		= flags.attribute("drop_plat").as_bool();
	player.dashing			= flags.attribute("dashing").as_bool();
	player.jumping			= flags.attribute("jumping").as_bool();
	player.playerGrounded	= flags.attribute("playerGrounded").as_bool();

	player.playerState = (PlayerState)node.attribute("playerstate").as_int();
	
	return true;
}

Players::~Players() 
{

};

bool Players::Init() 
{
	return true;
};

bool Players::Awake(pugi::xml_node& conf) 
{
	player.acceleration.x = conf.child("acceleration").attribute("x").as_float();
	player.acceleration.y = conf.child("acceleration").attribute("y").as_float();
	player.speed.x =		conf.child("speed").attribute("x").as_float();
	player.speed.y =		conf.child("speed").attribute("y").as_float();
	player.maxSpeed.x =		conf.child("maxSpeed").attribute("x").as_float();
	player.maxSpeed.y =		conf.child("maxSpeed").attribute("y").as_float();
	player.gravity =		conf.child("gravity").attribute("value").as_float();
	player.boxW =			conf.child("box").attribute("w").as_int();
	player.boxH =			conf.child("box").attribute("h").as_int();
	player.boxOffset_x =	conf.child("offset").attribute("x").as_int();

	App->audio->LoadFx("Assets/audio/fx/jump1.wav");
	App->audio->LoadFx("Assets/audio/fx/jump2.wav");
	App->audio->LoadFx("Assets/audio/fx/jump3.wav");

	App->audio->LoadFx("Assets/audio/fx/dash.wav");
	
	return true;
}; 
 
bool Players::Start() 
{
	StartPlayer();
	return true;
};

bool Players::PreUpdate() 
{
	if (player.freeze == true)
	{
		return true;
	} 
	
	player.SetGroundState(false);

	if (player.playerState != jumping && player.playerState != falling && !player.dashing)
	{
		player.playerState = idle;

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			player.playerState = crouch;
			player.drop_plat = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT 
			|| App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT 
			|| App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT 
			|| App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			player.playerState = running;
		}
	}

	if (player.able_to_dash && !player.dashing )
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN)
		{
			App->audio->PlayFx( 4 , 0 );
			if (player.flip)
			{
				player.playerState = dashLeft;
			}
			else
			{
				player.playerState = dashRight;
			}
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			{
				player.playerState = dashRight;
			}
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			{
				player.playerState = dashLeft;
			}
			player.able_to_dash = false;
		}
	}

	if (!player.godMode)
	{
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			if (player.able_to_jump)
			{
				jumpSound = rand() % 3 + 1;
				App->audio->PlayFx(jumpSound,0);

				player.playerState = jumping;

				player.speed.y = 0;	

			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		player.drop_plat = true;
	}
	else 
	{
		player.drop_plat = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		GodMode();
	}
	return true;
};

bool Players::Update(float dt)
{
	if (player.freeze == true)
	{
		return true;
	}
	
	player.prevposition = player.position;
	dashTime++;

	if (player.cealing || player.onPlatform && !player.jumping)
	{
		player.speed.y = 0;
	}

	if ((player.playerState == dashRight || player.playerState == dashLeft) && !player.dashing)
	{
		dashTime = 0;
	}

	if (!player.dashing)
	{
		check_x_move();
	}

	if (!player.godMode)
	{
		if (!player.godMode)
		{
			switch (player.playerState)
			{
			case idle:

				player.animation = "idle";
				player.speed.x = 0;

				break;
			case running:

				player.animation = "run";

				break;
			case crouch:

				player.animation = "crouch";

				break;
			case jumping:

				player.speed.y -= player.acceleration.y;
				player.jumping = true;
				player.able_to_jump = false;
				
				break;
			case falling:

				player.jumping = true;
				player.able_to_jump = false;

				break;
			case dashLeft:

				player.animation = "dash";
				player.speed.x = -player.maxSpeed.x * 2;
				player.dashing = true;

				break;
			case dashRight:

				player.animation = "dash";
				player.speed.x = player.maxSpeed.x * 2;
				player.dashing = true;

				break;
			}
		}
		
		if (player.playerGrounded || player.onPlatform)
		{
			player.able_to_jump = true;
			player.jumping = false;

			if (!player.dashing)
			{
				player.able_to_dash = true;
				player.playerState = idle;
			}
		}
		else
		{
			if (!player.dashing)
			{
				player.playerState = falling;
			}
		}

		if (player.dashing)
		{
			if (dashTime > 10)
			{
				player.playerState = falling;
				player.dashing = false;
				player.speed.y = 0;
			}
		}
		else
		{
			if (player.jumping )
			{
				player.speed.y += player.gravity;

				if (player.speed.y >= player.maxSpeed.y)
				{
					player.speed.y = player.maxSpeed.y;
				}

				if (player.speed.y < 0)
				{
					player.animation = "jump";
				}
				else
				{
					player.animation = "fall";
				}
			}

			player.position.y += player.speed.y;
		}

		player.position.x += player.speed.x;

		if (player.wall)
		{
			player.speed.x = 0;
		}
		
	}
	else
	{
		player.animation = "idle";

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			MoveUp();
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			MoveDown();
		}
		player.position.x += player.speed.x * 2;
	}

	player.playerBox.x = player.position.x;
	player.playerBox.y = player.position.y;

	App->map->DrawAnimation(player.animation,"Char",player.flip);
	
	player.collider->SetPos(player.position.x + player.boxOffset_x, player.position.y);

	player.cealing = false;
	player.wall = false;
	player.onPlatform = false;

	return true;
};

bool Players::PostUpdate() 
{
	if (player.freeze == true)
	{
		return true;
	}

	if (player.justLoaded == true) 
	{
		player.justLoaded = false;
	}

	return true;
};

bool Players::CleanUp() 
{
	return true;
};

bool Players::StartPlayer() 
{
	if(App->fade->playerReset == true)

	player.position = App->map->data.start_position;
	player.playerBox = { (int)player.position.x,(int)player.position.y,player.boxW,player.boxH };
	player.collider = App->collisions->AddCollider(player.playerBox, ObjectType::PLAYER, this);
	player.able_to_jump = false;
	player.able_to_dash = false;
	player.dashing = false;
	player.jumping = false;
	player.drop_plat;
	player.flip = false;
	player.godMode = false;
	player.cealing = false;
	player.wall = false;
	player.onPlatform = false;
	player.movingRight = false;
	player.movingLeft = false;
	player.justLoaded = false;
	
	return true;
}

void Players::OnCollision(Collider* A, Collider* B) 
{
	if (player.godMode) return;
	
	if (B->type == ObjectType::PLAYER)
	{
		Collider temp = *A;
		A = B;
		B = &temp;
	}
	if (A->type != ObjectType::PLAYER)
	{
		return;
	}

	//-------------COLLISSIONS (TODO: CHANGE THEM TO RAMON'S)----------//
	if (A->type == ObjectType::PLAYER && B->type == ObjectType::SOLID)
	{
		if (player.prevposition.y > (B->rect.y + B->rect.h - 1)) 
		{
			player.position.y = B->rect.y + B->rect.h ;
			player.cealing = true;
		}
		else if (player.position.y + (A->rect.h* 1.0f/4.0f) < B->rect.y + B->rect.h  && player.position.y + (A->rect.h* 3.0f / 4.0f) > B->rect.y ) 
		{
			player.wall = true;
			LOG("Touching WALL");

			if ((A->rect.x + A->rect.w) < (B->rect.x + B->rect.w / 4)) 
			{
				player.position.x = B->rect.x -A->rect.w - player.boxOffset_x -1;
			}
			else if (A->rect.x  > (B->rect.x + B->rect.w*3/4)) 
			{
				player.position.x = B->rect.x + B->rect.w - player.boxOffset_x -1; 
			}
		}

		else if (player.position.y + A->rect.h - player.maxSpeed.y -5 < B->rect.y && A->rect.x < B->rect.x + B->rect.w && A->rect.x + A->rect.w > B->rect.x && player.justLoaded == false)
		{
			if (player.speed.y > 0)
			{
				player.speed.y = 0;
			}
			player.position.y = B->rect.y - player.collider->rect.h + 1;
			player.SetGroundState(true);
		}
	}


	// ------------ TODO: CHANGE THEM FOR RAMON'S---------------------//
	if (A->type == ObjectType::PLAYER && B->type == ObjectType::PLATFORM)
	{
		if (player.drop_plat == false ) 
		{
			if ((player.prevposition.y + player.collider->rect.h) < B->rect.y + (B->rect.h / 2.0f) && (player.prevposition.y + player.collider->rect.h) > B->rect.y && player.speed.y >= 0) 
			{
				player.position.y = B->rect.y - player.collider->rect.h + 1;
				player.SetGroundState(true);
				player.able_to_jump = false;
				player.onPlatform = true;
			}
			else if ((player.position.y >= player.prevposition.y) && (player.prevposition.y + player.collider->rect.h) < B->rect.y && player.speed.y >= 0) 
			{
				player.position.y = B->rect.y - player.collider->rect.h + 1;
				player.SetGroundState(true);
				player.able_to_jump = false;
				player.onPlatform = true;
			}
		}
	}

	if (A->type == ObjectType::PLAYER && B->type == ObjectType::WARP)
	{
		App->fade->FadeToBlack(B->userdata->Get("MapToLoad").v_string);
	}
}

void Players::check_x_move()
{
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		player.movingRight = true;
		MoveRight();
		player.flip = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		player.movingLeft = true;
		MoveLeft();
		player.flip = true;
	}
	else
	{
		player.speed.x = 0;
	}
}

void Players::MoveRight()
{
	player.speed.x += player.acceleration.x;

	if (player.speed.x > player.maxSpeed.x)
	{
		player.speed.x = player.maxSpeed.x;
	}
}

void Players::MoveLeft()
{
	player.speed.x -= player.acceleration.x; 

	if (player.speed.x < -player.maxSpeed.x)
	{
		player.speed.x = -player.maxSpeed.x;
	}
}

void Players::MoveDown()
{
	player.position.y += player.maxSpeed.y;
}

void Players::MoveUp()
{
	player.position.y -= player.maxSpeed.y;
}

void Players::GodMode()
{
	if (player.godMode)
	{
		player.godMode = false;
	}
	else
	{
		player.godMode = true;
	}
}