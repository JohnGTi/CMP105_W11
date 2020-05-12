#pragma once
#include "Framework/GameObject.h"

class Background : public GameObject
{
private:
	//Attributes:
	//float speed = 400.f;
	//sf::RenderWindow* window;
	sf::Vector2f stepVelocity;
	sf::Vector2f acceleration;

	float sceneDuration = 0;

public:
	//Functions:
	Background();
	~Background();

	//void receiveWin(sf::RenderWindow* hwnd) { window = hwnd; };
	void update(float dt);
	bool sceneEnd(float dt);
};