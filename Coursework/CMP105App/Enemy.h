#pragma once
#include "Framework/GameObject.h"
#include "Framework\Animation.h"
#include "Framework/Vector.h"

class Enemy : public GameObject
{
protected:
	Animation* currentAnimation;
	Animation walk[12];
	Animation strike[12];


	int stance = 6;
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

public:
	Enemy();
	~Enemy();

	float getTheta() { return theta; };
	float getstrikePoint(int c) { return strikePoint[c][stance]; };
	float getHealth() { return health; };

	bool getHit() { return hit; };
	void sceneEnd(float dt);

	int getStance() { return stance; };
	int getStanceLock() { return stanceLock; };

	void collisionResponse(GameObject*);
	void handleInput(float dt);
	void update(float dt, sf::Vector2f, bool);
	void resetPlay();
};