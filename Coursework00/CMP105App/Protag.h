#pragma once
#include "Framework/GameObject.h"
#include "Framework\Animation.h"

class Protag : public GameObject
{
protected:
	Animation walk[12];
	Animation strike[12];
	Animation* currentAnimation;
	//Declares both evolving animations for protag.

	sf::Vector2f stepVelocity;
	sf::Vector2f sceneVelocity;
	sf::Vector2f acceleration; //Vectors to be used in motion (play and scene).
	sf::RenderWindow* window; //Allows window to be received.

	float strikePoint[2][12] = { {0, 42, 52, 58, 48, 38, 18, -48, -60, -58, -36, -20}, {-50, -38, -34, -20, 22, 26, 48, 28, 10, -4, -28, -36} };
	//Strike points are the specific cordinates that correlate to one of twelve stances (attack positioning)
	float health = 1.f;
	float lockDuration = 0;
	float sceneDuration = 0;
	float flinchDuration = 0;
	float theta;
	float slow = 0.f;

	bool applyForce = false;
	bool lock = false;
	bool hit = false; //Booleans that assist in controlling protag action authorisation. 
	bool flinch = false;
	bool allowResolve = true;

	int stanceLock;
	int stance = 0; //Stance evolves depending on mouse position. Above lock forces the player to commit to an action.

public:
	Protag();
	~Protag();

	void receiveWin(sf::RenderWindow* hwnd) { window = hwnd; };
	void collisionResponse(GameObject*, sf::Vector2f); //Body collision.

	float getstrikePoint(int c) { return strikePoint[c][stance]; }; //Getter allows hitmarker to be set.
	float getTheta() { return theta; }; //Other files have access to the players direction.
	float getHealth() { return health; }; //Getter allows level to check state.

	bool getHit() { return hit; }; //Getter primarily to allow resolution with enemy.

	void handleInput(float, sf::Vector2f, int);
	void update(float, bool, bool);

	void resetPlay(); //Reset many above values for retry or new game.
	void sceneEnd(float dt); //Simulates player being anchored to the ground in win state.
};