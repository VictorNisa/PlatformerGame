#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "Point.h"
#include "Render.h"
#include "Input.h"
#include "PerfTimer.h"
#include "EntityManager.h"

struct Collider;

enum PlayerState 
{
	IDLE,
	RUNNING,
	JUMPING,
	FALLING,
	CROUCHING,
	LDASHING,
	RDASHING,
	ATTACKING
};

struct Player 
{
	Point<float> acceleration;
	Point<float> speed;
	Point<float> maxSpeed;
	// Point<float> position;
	fPoint prevPosition;
	Point<float> lastGroundedPos;

	SString animation;

	float gravity; 

	bool ableToJump;
	bool ableToDash;
	bool dashing;
	bool jumping;
	bool dropPlat;
	bool playerGrounded;
	bool flip;
	bool godMode;
	bool cealing;
	bool wall;
	bool onPlatform;
	bool movingRight;
	bool movingLeft;
	bool justLoaded;
	bool attacking;
	bool freeze = false;

	int boxW;
	int boxH;
	int boxOffset_x;

	SDL_Rect playerBox;
	PlayerState playerState;
	Collider* collider;
	Collider* attackCollider;
	SDL_Rect attackBox;
	bool attackColliderBool = false;

	inline void SetGroundState(bool state) 
	{
		playerGrounded = state;
	};
};

class Players : public Entity 
{
public:

	Players(float x, float y, EntityType Type);

	// Destructor
	virtual ~Players();

	bool Init();

	bool Awake();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	bool CleanUp();

	void checkXMove(float dt);
	void MoveRight(float dt);
	void MoveLeft(float dt);
	void MoveUp(float dt);
	void MoveDown(float dt);
	void DashInput();
	void JumpInput();
	void RunCheck();
	void Jump();
	void Fall();
	void Attack();

	void GodMode();
	bool StartPlayer();
	bool Dash();
	void GroundedLogic();

	void Players::OnCollision(Collider* A, Collider* B);

	virtual bool Save(pugi::xml_node & node) const;
	virtual bool Load(pugi::xml_node & node);

private:

public:

	Player player;
	
private:

	bool jumpKeyBool = false;
	float gravityTimer = 0;

	int dashTime  = 0;
	int jumpSound = 1;

	PerfTimer* dashTimerCheck;
	PerfTimer* jumpKeyBoolTimer;
};

#endif
