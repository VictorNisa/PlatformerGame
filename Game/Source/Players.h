#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Module.h"
#include "Point.h"
#include "Render.h"
#include "Input.h"


struct Collider;

enum PlayerState 
{
	idle,
	running,
	jumping,
	falling,
	crouch,
	dashLeft,
	dashRight
};

struct Player 
{
	Point<float> acceleration;
	Point<float> speed;
	Point<float> maxSpeed;
	Point<float> position;
	Point<float> prevposition;
	Point<float> lastGroundedPos;

	SString animation;

	float gravity; 

	bool able_to_jump;
	bool able_to_dash;
	bool dashing;
	bool jumping;
	bool drop_plat;
	bool playerGrounded;
	bool flip;
	bool godMode;
	bool cealing;
	bool wall;
	bool onPlatform;
	bool movingRight;
	bool movingLeft;
	bool justLoaded;
	bool freeze = false;

	int boxW;
	int boxH;
	int boxOffset_x;

	SDL_Rect playerBox;
	PlayerState playerState;
	Collider*	collider;

	inline void SetGroundState(bool state) 
	{
		if (playerGrounded == true) 
		{
			lastGroundedPos = position;
		}
		playerGrounded = state;
	};
};

class Players : public Module 
{
public:

	Players();

	// Destructor
	virtual ~Players();

	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);

	bool Init();

	bool Awake(pugi::xml_node&);

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	bool CleanUp();

	void check_x_move();
	void MoveRight();
	void MoveLeft();
	void MoveUp();
	void MoveDown();

	void GodMode();
	bool StartPlayer();

	void Players::OnCollision(Collider* A, Collider* B);

private:

	
public:

	Player player;
	
private:

	int dashTime  = 0;
	int jumpSound = 1;
};

#endif
