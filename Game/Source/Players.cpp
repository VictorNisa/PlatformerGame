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

Players::Players(float x, float y, EntityType Type) : Entity(x, y, Type)
{

};

Players::~Players()
{

}

bool Players::Save(pugi::xml_node& node) const 
{
	LOG("Saving Player...");
	node.append_attribute("EntityType") = "PLAYER";
	// SAVING POINTS
	pugi::xml_node points = node.append_child("points");

	// SAVING POSITION
	points.append_child("position").append_attribute("x") = position.x;
	points.child("position").append_attribute("y") = position.y;

	// SAVING PREVIOUS POSITION
	points.append_child("prevPosition").append_attribute("x") = player.prevPosition.x;
	points.child("prevPosition").append_attribute("y") = player.prevPosition.y;
	
	// SAVING LAST GROUND POSITION
	points.append_child("lastGroundedPos").append_attribute("x") = player.lastGroundedPos.x;
	points.child("lastGroundedPos").append_attribute("y") = player.lastGroundedPos.y;

	// SAVING BOOLS
	pugi::xml_node flags = node.append_child("flags");
	flags.append_attribute("ableToJjump") = player.ableToJump;
	flags.append_attribute("ableToDash") = player.ableToDash;
	flags.append_attribute("dashing") = player.dashing;
	flags.append_attribute("jumping") = player.jumping;
	flags.append_attribute("dropPlat") = player.dropPlat;
	flags.append_attribute("playerGrounded") = player.playerGrounded;
	flags.append_attribute("flip") = player.flip;
	flags.append_attribute("godMode") = player.godMode;

	// SAVING PLAYER STATE
	node.append_attribute("playerstate") = player.playerState;

	return true;
}

bool Players::Load(pugi::xml_node& node)
{
	LOG("Loading Player...");

	player.justLoaded = true;

	pugi::xml_node points = node.child("points");

	// LOADING POSITION
	position.x = points.child("position").attribute("x").as_float();
	position.y = points.child("position").attribute("y").as_float();

	// LOADING PREVIOUS POSITION
	player.prevPosition.x = points.child("prevPosition").attribute("x").as_float();
	player.prevPosition.y = points.child("prevPosition").attribute("y").as_float();

	// LOADING LAST GROUND POSITION
	player.lastGroundedPos.x = points.child("lastGroundedPos").attribute("x").as_float();
	player.lastGroundedPos.y = points.child("lastGroundedPos").attribute("y").as_float();
	
	// LOADING BOOLS
	pugi::xml_node flags = node.child("flags");
	player.ableToDash		= flags.attribute("ableToDash").as_bool();
	player.ableToJump		= flags.attribute("ableToJump").as_bool();
	player.dropPlat		= flags.attribute("dropPlat").as_bool();
	player.dashing			= flags.attribute("dashing").as_bool();
	player.jumping			= flags.attribute("jumping").as_bool();
	player.playerGrounded	= flags.attribute("playerGrounded").as_bool();

	player.playerState = (PlayerState)node.attribute("playerstate").as_int();
	
	return true;
}

bool Players::Init() 
{
	return true;
};

bool Players::Awake() 
{
	pugi::xml_document	configFile;
	pugi::xml_node		config;

	config = App->LoadConfig(configFile);

	pugi::xml_node tmp = config.child("entities");

	player.acceleration.x = tmp.child("player").child("acceleration").attribute("x").as_float();
	player.acceleration.y = tmp.child("player").child("acceleration").attribute("y").as_float();
	player.speed.x = tmp.child("player").child("speed").attribute("x").as_float();
	player.speed.y = tmp.child("player").child("speed").attribute("y").as_float();
	player.maxSpeed.x = tmp.child("player").child("maxSpeed").attribute("x").as_float();
	player.maxSpeed.y = tmp.child("player").child("maxSpeed").attribute("y").as_float();
	player.gravity = tmp.child("player").child("gravity").attribute("value").as_float();
	player.boxW = tmp.child("player").child("box").attribute("w").as_int();
	player.boxH = tmp.child("player").child("box").attribute("h").as_int();
	player.boxOffset_x = tmp.child("player").child("offset").attribute("x").as_int();

	App->audio->LoadFx("Assets/audio/fx/jump1.wav");
	App->audio->LoadFx("Assets/audio/fx/jump2.wav");
	App->audio->LoadFx("Assets/audio/fx/jump3.wav");
	App->audio->LoadFx("Assets/audio/fx/dash.wav");
	
	return true;
}; 
 
bool Players::Start() 
{
	StartPlayer();
	player.attackBox = { 0, 0, 32, 64 };
	dashTimerCheck = new PerfTimer;
	jumpKeyBoolTimer = new PerfTimer;

	return true;
};

bool Players::PreUpdate() 
{
	if (player.freeze == true)
	{
		return true;
	} 
	
	player.SetGroundState(false);

	if (player.playerState != JUMPING && player.playerState != FALLING && !player.dashing)
	{
		player.playerState = IDLE;
		RunCheck();
	}

	if (!player.godMode && !player.dashing)
	{
		JumpInput();
	}

	if (player.ableToDash && !player.dashing)
	{
		DashInput();
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		player.dropPlat = true;
	}
	else
	{
		player.dropPlat = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		GodMode();
	}

	return true;
};

bool Players::Update(float dt)
{
	player.prevPosition = position;

	if (player.freeze == true)
	{
		return true;
	}

	if (player.cealing || player.onPlatform && !player.jumping)
	{
		player.speed.y = 0;
	}

	if (!player.dashing)
	{
		checkXMove(dt);
	}

	if (!player.godMode)
	{
		if (!player.godMode)
		{
			switch (player.playerState)
			{
			case IDLE:

				player.animation = "idle";
				player.speed.x = 0;

				break;
			case RUNNING:

				player.animation = "run";

				break;
			case CROUCHING:

				player.animation = "crouch";

				break;
			case JUMPING:

				Jump();
				
				break;
			case FALLING:

				Fall();

				break;
			case LDASHING:

				Dash();

				break;
			case RDASHING:

				Dash();

				break;
			case ATTACKING:

				Attack();

				break;
			}
		}

		GroundedLogic();
		
	}
	else
	{
		player.animation = "idle";

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			MoveUp(dt);
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			MoveDown(dt);
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			player.speed.x = -player.maxSpeed.x;
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			player.speed.x = player.maxSpeed.x;
		}

		position.x += player.speed.x * 2;
	}

	player.playerBox.x = position.x;
	player.playerBox.y = position.y;

	App->map->DrawAnimation(player.animation,"Char",position, aInfo, player.flip);
	
	player.collider->SetPos(position.x + player.boxOffset_x, position.y);

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

	position = App->map->data.startPosition;
	player.playerBox = { position.x,position.y,player.boxW,player.boxH };
	player.collider = App->collisions->AddCollider(player.playerBox, ObjectType::PLAYER, App->entities, (Entity*)this);
	
	player.ableToJump = false;
	player.ableToDash = false;
	player.dashing = false;
	player.jumping = false;
	player.dropPlat;
	player.flip = false;
	player.godMode = false;
	player.cealing = false;
	player.wall = false;
	player.onPlatform = false;
	player.movingRight = false;
	player.movingLeft = false;
	player.justLoaded = false;
	player.attacking = false;
	
	return true;
}

void Players::OnCollision(Collider* A, Collider* B) 
{
//	if (player.godMode) return;
//	
//	if (B->type == ObjectType::PLAYER)
//	{
//		Collider temp = *A;
//		A = B;
//		B = &temp;
//	}
//	if (A->type != ObjectType::PLAYER)
//	{
//		return;
//	}
//
//	//-------------COLLISSIONS (TODO: CHANGE THEM TO RAMON'S)----------//
//	if (A->type == ObjectType::PLAYER && B->type == ObjectType::SOLID)
//	{
//		if (player.prevPosition.y > (B->rect.y + B->rect.h - 1)) 
//		{
//			player.position.y = B->rect.y + B->rect.h ;
//			player.cealing = true;
//		}
//		else if (player.position.y + (A->rect.h* 1.0f/4.0f) < B->rect.y + B->rect.h  && player.position.y + (A->rect.h* 3.0f / 4.0f) > B->rect.y ) 
//		{
//			player.wall = true;
//			LOG("Touching WALL");
//
//			if ((A->rect.x + A->rect.w) < (B->rect.x + B->rect.w / 4)) 
//			{
//				player.position.x = B->rect.x -A->rect.w - player.boxOffset_x -1;
//			}
//			else if (A->rect.x  > (B->rect.x + B->rect.w*3/4)) 
//			{
//				player.position.x = B->rect.x + B->rect.w - player.boxOffset_x -1; 
//			}
//		}
//
//		else if (player.position.y + A->rect.h - player.maxSpeed.y -5 < B->rect.y && A->rect.x < B->rect.x + B->rect.w && A->rect.x + A->rect.w > B->rect.x && player.justLoaded == false)
//		{
//			if (player.speed.y > 0)
//			{
//				player.speed.y = 0;
//			}
//			player.position.y = B->rect.y - player.collider->rect.h + 1;
//			player.SetGroundState(true);
//		}
//	}
//
//
//	// ------------ TODO: CHANGE THEM FOR RAMON'S---------------------//
//	if (A->type == ObjectType::PLAYER && B->type == ObjectType::PLATFORM)
//	{
//		if (player.dropPlat == false ) 
//		{
//			if ((player.prevPosition.y + player.collider->rect.h) < B->rect.y + (B->rect.h / 2.0f) && (player.prevPosition.y + player.collider->rect.h) > B->rect.y && player.speed.y >= 0) 
//			{
//				player.position.y = B->rect.y - player.collider->rect.h + 1;
//				player.SetGroundState(true);
//				player.ableToJump = false;
//				player.onPlatform = true;
//			}
//			else if ((player.position.y >= player.prevPosition.y) && (player.prevPosition.y + player.collider->rect.h) < B->rect.y && player.speed.y >= 0) 
//			{
//				player.position.y = B->rect.y - player.collider->rect.h + 1;
//				player.SetGroundState(true);
//				player.ableToJump = false;
//				player.onPlatform = true;
//			}
//		}
//	}
//
//	if (A->type == ObjectType::PLAYER && B->type == ObjectType::TELEPORT)
//	{
//		App->fade->FadeToBlack(B->userData->Get("MapToLoad").vString);
//	}
}

void Players::checkXMove(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		player.movingRight = true;
		MoveRight(dt);
		player.flip = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		player.movingLeft = true;
		MoveLeft(dt);
		player.flip = true;
	}
	else
	{
		player.speed.x = 0;
	}
}

void Players::JumpInput()
{
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		if (player.ableToJump)
		{
			player.playerState = JUMPING;
			jumpKeyBool = true;
			jumpKeyBoolTimer->Start();
			player.speed.y = 0;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && jumpKeyBool && jumpKeyBoolTimer->ReadMs() < 250.0f && !player.dashing)
	{
		player.playerState = JUMPING;
		jumpKeyBool = true;
	}
	else
	{
		jumpKeyBool = false;
	}
}

void Players::DashInput()
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN)
	{
		App->audio->PlayFx(4, 0);

		if (player.flip || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			player.playerState = LDASHING;
		}
		else if (!player.flip || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			player.playerState = RDASHING;
		}

		dashTime = 0;

		dashTimerCheck->Start();

		player.ableToDash = false;
	}
}

void Players::RunCheck()
{
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		player.playerState = CROUCHING;
		player.dropPlat = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT
		|| App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT
		|| App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT
		|| App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		player.playerState = RUNNING;
	}
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

bool Players::Dash()
{
	player.animation = "dash";
	if (!player.wall)
	{
		if (player.playerState == LDASHING)
		{
			player.speed.x = -player.maxSpeed.x * 2 * App->dt;
		}
		else
		{
			player.speed.x = player.maxSpeed.x * 2 * App->dt;
		}
	}
	player.dashing = true;

	if (dashTimerCheck->ReadMs() >= 25.0f && !player.attackColliderBool)
	{
		player.attackCollider = App->collisions->AddCollider(player.attackBox,ObjectType::ATTACK,App->entities );
		player.attackColliderBool = true;
	}

	if (player.attackColliderBool)
	{
		if (!player.flip)
		{
			player.attackCollider->SetPos(position.x + 70, position.y);
		}
		else
		{
			player.attackCollider->SetPos(position.x - 20, position.y);
		}
	}


	if (dashTimerCheck->ReadMs() >= 200.0f)
	{
		player.attackCollider->toDelete = true;
		player.attackColliderBool = false;
	}

	if (dashTimerCheck->ReadMs() >= 225.0f)
	{
		player.playerState = FALLING;
		player.dashing = false;
		player.speed.y = 0;
	}
	return true;
}

void Players::Jump()
{
	player.animation = "jump";
	player.jumping = true;
	player.ableToJump = false;

	if (player.speed.y > 0)
	{
		player.playerState = FALLING;
	}

	player.speed.y -= player.acceleration.y * App->dt;
	player.speed.y += (player.gravity * 0.8f) * App->dt;
	position.y += player.speed.y;
}

void Players::Fall()
{
	player.animation = "fall";
	player.jumping = true;
	player.ableToJump = false;

	player.speed.y += player.gravity * App->dt;
	position.y += player.speed.y;
}

void Players::GroundedLogic()
{
	if (player.playerGrounded || player.onPlatform)
	{
		player.ableToJump = true;
		player.jumping = false;

		if (!player.dashing)
		{
			player.ableToDash = true;
			player.playerState = IDLE;
		}
	}
	else
	{
		if (!player.dashing)
		{
			player.playerState = FALLING;
		}
	}

	if (player.speed.y >= player.maxSpeed.y * App->dt)
	{
		player.speed.y = player.maxSpeed.y * App->dt;
	}

	position.x += player.speed.x;

	if (player.wall)
	{
		player.speed.x = 0;
	}
}

void Players::Attack()
{
	// TODO: Implement attacking
}

void Players::MoveRight(float dt)
{
	player.speed.x += player.acceleration.x * dt;

	if (player.speed.x > player.maxSpeed.x*dt)
	{
		player.speed.x = player.maxSpeed.x*dt;
	}

}

void Players::MoveLeft(float dt)
{
	player.speed.x -= player.acceleration.x*dt;

	if (player.speed.x < -player.maxSpeed.x*dt)
	{
		player.speed.x = -player.maxSpeed.x*dt;
	}
}

void Players::MoveDown(float dt)
{
	position.y += player.maxSpeed.y*dt;
}

void Players::MoveUp(float dt)
{
	position.y -= player.maxSpeed.y*dt;
}