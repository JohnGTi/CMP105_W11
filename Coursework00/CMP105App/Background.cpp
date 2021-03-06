#include "Background.h"

Background::Background()
{
	//stepVelocity.y = (1519.f / 5.88f);
	stepVelocity.y = 10.f;
	acceleration.y = 10.f; //Values set for motion in end scene.
}

Background::~Background()
{

}

void Background::update(float dt)
{

}

bool Background::sceneEnd(float dt)
{
	sceneDuration += dt;

	if (sceneDuration >= 2.68f) {
		// calculate direction and move
		sf::Vector2f pos;
		pos = stepVelocity * dt + 0.5f * acceleration * dt * dt; // s = ut + 1/2at^2
		setPosition(getPosition() + pos);
	}
	stepVelocity += acceleration * dt; // v = u + at  note the += is not =
	//if (sceneDuration >= (2.68f + 0.73f)) { constant +y; }

	if (getPosition().y >= 0.f) { sceneDuration = 0; return true; }
	
	//This function accelerates the background image vertically until the top of the image alligns with the window.
	//False is returned until the top of the image is framed properly. After which returning true allows the function call in level to change the gameState over to the splash screen.

	return false;
}
