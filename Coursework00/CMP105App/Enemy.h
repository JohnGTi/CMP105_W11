#pragma once
#include "Framework/GameObject.h"
#include "Framework\Animation.h"
#include "Framework/Vector.h"
#include "Framework/AudioManager.h"
#include <cstdlib>
#include <ctime>
#include <string>

class Enemy : public GameObject
{
protected:
	sf::SoundBuffer actionBuffer;
	sf::Sound action;
	sf::SoundBuffer impactBuffer;
	sf::Sound impact;

	Animation* currentAnimation;
	Animation walk[12];
	Animation strike[12];
	//Declares both evolving animations for enemy.

	int stance = 6; //Preset to face downwards (towards player starting point).
	int stanceLock;

	float lockDuration = 0;
	float sceneDuration = 0;
	float health = 1.f;
	float slow = 0.f;
	float speed;
	float theta;
	float strikePoint[2][12] = { {-2, 38, 56, 56, 52, 24, -2, -36, -54, -52, -56, -28}, {-32, -32, 0, 16, 20, 16, 20, 24, 16, 8, 6, -32} };

	bool lock = false;
	bool flinch = false;
	bool flinchRes = false;
	bool hit = false;
	bool allowResolve = true;
	bool initial = false;

	//sf::Vector2f target;
	sf::Vector2f stepVelocity;
	sf::Vector2f direction;
	sf::Vector2f acceleration;

	//^^^Many of these values serve the same purpose as in protag, but are managed a little differentl in actual processess (e.g different movement, collsion principles).

public:
	Enemy();
	~Enemy();

	float getTheta() { return theta; }; //Other files have access to the players direction.
	float getstrikePoint(int c) { return strikePoint[c][stance]; }; //Getter allows hitmarker to be set.
	float getHealth() { return health; }; //Getter allows level to check state.

	bool getHit() { return hit; }; //Getter primarily to allow resolution with player.
	void sceneEnd(float dt); //Simulates enemy being anchored to the ground in win state.

	int getStance() { return stance; };
	int getStanceLock() { return stanceLock; };

	void collisionResponse(GameObject*); //Body collision.
	void handleInput(float dt);
	void update(float dt, sf::Vector2f, bool);
	void resetPlay(); //Reset many above values for retry or new game.
};