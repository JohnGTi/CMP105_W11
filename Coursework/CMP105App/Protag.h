#pragma once
#include "Framework/GameObject.h"
#include "Framework\Animation.h"

class Protag : public GameObject
{
protected:
	Animation walk[12];
	Animation strike[12];
	Animation* currentAnimation;

	sf::Vector2f stepVelocity;
	sf::Vector2f sceneVelocity;
	sf::Vector2f acceleration;
	sf::RenderWindow* window;

	float strikePoint[2][12] = { {0, 42, 52, 58, 48, 38, 18, -48, -60, -58, -36, -20}, {-50, -38, -34, -20, 22, 26, 48, 28, 10, -4, -28, -36} };
	float health = 1.f;
	float lockDuration = 0;
	float sceneDuration = 0;
	float flinchDuration = 0;
	float theta;
	float slow = 0.f;

	bool applyForce = false;
	bool lock = false;
	bool hit = false;
	bool flinch = false;
	bool allowResolve = true;

	int stanceLock;
	int stance = 0;

public:
	Protag();
	~Protag();

	void receiveWin(sf::RenderWindow* hwnd) { window = hwnd; };
	void collisionResponse(GameObject*, sf::Vector2f);

	float getstrikePoint(int c) { return strikePoint[c][stance]; };
	float getTheta() { return theta; };
	float getHealth() { return health; };

	bool getHit() { return hit; };

	void handleInput(float, sf::Vector2f, int);
	void update(float, bool, bool);

	void resetPlay();
	void sceneEnd(float dt);
};