#include "Protag.h"
#include <iostream>

Protag::Protag()
{
	int startPosY = 0;
	for (int i = 0; i < 12; i++) {
		walk[i].setFrameSpeed(1.f / 10.f);
		walk[i].addFrame(sf::IntRect(0, startPosY, 64, 64));
		walk[i].addFrame(sf::IntRect(64, startPosY, 64, 64));

		strike[i].setFrameSpeed(1.25f / 10.f);
		strike[i].addFrame(sf::IntRect(128, startPosY, 64, 64));
		strike[i].addFrame(sf::IntRect(192, startPosY, 64, 64));
		startPosY += 64;
	}

	currentAnimation = &walk[0];

	sceneVelocity.y = 10.f;
	acceleration.y = 10.f;
}

Protag::~Protag()
{

}

void Protag::collisionResponse(GameObject* collider, sf::Vector2f enemPos)
{
	if ((enemPos.y + 4) >= (getPosition().y - 28)) { setPosition(getPosition().x, getPosition().y - 0.1f); }
	else { setPosition(getPosition().x, getPosition().y + 0.1f); }
}

void Protag::sceneEnd(float dt)
{
	sceneDuration += dt;

	if (sceneDuration >= 2.68f) {
		// calculate direction and move
		sf::Vector2f pos;
		pos = sceneVelocity * dt + 0.5f * acceleration * dt * dt; // s = ut + 1/2at^2
		setPosition(getPosition() + pos);
	}
	sceneVelocity += acceleration * dt; // v = u + at  note the += is not =
	//if (sceneDuration >= (2.68f + 0.73f)) { constant +y; }
}

void Protag::handleInput(float dt, sf::Vector2f enemPos, int enemStance)
{
	if (input->isKeyDown(sf::Keyboard::W) == true && lock == false)
	{
		stepVelocity.y = -150.0f + slow; //-150.0f;
		applyForce = true;
		//std::cout << "W.\n";

		/*currentAnimation = &walk;
		currentAnimation->animate(dt);
		setTextureRect(currentAnimation->getCurrentFrame());*/
	}

	if (input->isKeyDown(sf::Keyboard::A) == true && lock == false)
	{
		stepVelocity.x = -150.0f + slow;
		applyForce = true;
		//std::cout << "A.\n";
	}

	if (input->isKeyDown(sf::Keyboard::S) == true && lock == false)
	{
		stepVelocity.y = 150.0f - slow;
		applyForce = true;
		//std::cout << "S.\n";
	}

	if (input->isKeyDown(sf::Keyboard::D) == true && lock == false)
	{
		stepVelocity.x = 150.0f - slow;
		applyForce = true;
		//std::cout << "D.\n";
	}

	if (input->isMouseLDown() == true && lock == false)
	{
		input->setMouseLDown(false);
		//std::cout << "(" << getPosition().x << "),(" << getPosition().y << ")\n";
		lock = true;
		stanceLock = stance;
	}
	if (lock == true) {
		lockDuration += dt;
		currentAnimation = &strike[stanceLock];
		currentAnimation->animate(dt);

		int xOffset = 2;
		if (enemStance == 2 || enemStance == 3) { xOffset = (-10); }
		else if (enemStance == 9) { xOffset = (10); }

		if (lockDuration <= 0.125f && lockDuration <= 0.15f) {
			float deltaX = (enemPos.x + xOffset) - (getPosition().x + strikePoint[0][stance]);
			float deltaY = (enemPos.y - 4) - (getPosition().y + strikePoint[1][stance]);

			if ((deltaX * deltaX + deltaY * deltaY) < (26 * 26)) { hit = true; }
		}//std::cout << "Hit.\n";
		if (lockDuration >= 0.25f) { currentAnimation->reset(); currentAnimation = &walk[stanceLock]; currentAnimation->reset(); }
		if (lockDuration >= 0.35f) { lock = false; lockDuration = 0; hit = false; } //Change 0.35 to 0.45?
	}
}

void Protag::update(float dt, bool enemSuccess, bool invisOn)
{
	if (enemSuccess) {
		if (allowResolve) { //stanceRange
			if (!invisOn) { health = health - 0.05f; }//decrement protag health by...
			slow = slow + 1.625f; //slow protag overall speed by...
			allowResolve = false; std::cout << "protag.health " << health << ".\n";
		}
	} if (!enemSuccess) { allowResolve = true; }

	sf::Vector2f pos;
	if (applyForce == true) {
		pos = stepVelocity * dt; // s = ut
	}
	if ((getPosition().x + pos.x) > -144 && (getPosition().x + pos.x) < window->getSize().x + 144)
	{
		if ((getPosition().y + pos.y) > -144 && (getPosition().y + pos.y) < window->getSize().y + 144)
		{
			setPosition(getPosition() + pos);
		}
	}
	//^^^Player can only go so far out of bounds.
	stepVelocity = sf::Vector2f(0, 0);

	//if (input->isMouseLDown() == true) { setPosition(input->getMouseX(), input->getMouseY()); }

	float A;
	float B;
	float add = 0.f;

	if (input->getMouseX() > getPosition().x && input->getMouseY() < getPosition().y) {
		A = getPosition().x;
		B = input->getMouseY();
		add = 0.f;
		
	} //A
	else if (input->getMouseX() > getPosition().x && input->getMouseY() > getPosition().y) {
		A = input->getMouseX();
		B = getPosition().y;
		add = 90.f;
	} //C
	else if (input->getMouseX() < getPosition().x && input->getMouseY() > getPosition().y) {
		A = getPosition().x;
		B = input->getMouseY();
		add = 180.f;
	} //S
	else {
		A = input->getMouseX();
		B = getPosition().y;
		add = 270.f;
	} //T
	//(input->getMouseX() < getPosition().x && input->getMouseY() < getPosition().y)

	float adj = (((A - getPosition().x) * (A - getPosition().x) + (B - getPosition().y) * (B - getPosition().y)));
	adj = sqrt(adj);
	float hyp = ((input->getMouseX() - getPosition().x) * (input->getMouseX() - getPosition().x)) + ((input->getMouseY() - getPosition().y) * (input->getMouseY() - getPosition().y));
	hyp = sqrt(hyp);

	theta = adj / hyp;
	theta = acos(theta);
	theta = theta * 180 / 3.1415 + add;

	int max = 45;
	for (int p = 1; p < 12; p++) {
		if (theta < max) {
			stance = p;
			break;
		}
		else { max = max + 30; }
	} if (theta > 345 || theta < 15) { stance = 0; }

	if (lock == false) {
		currentAnimation = &walk[stance];
		if (applyForce == true) { currentAnimation->animate(dt); }
	}
	setTextureRect(currentAnimation->getCurrentFrame());
	applyForce = false;
}

void Protag::resetPlay()
{
	lockDuration = 0;
	flinchDuration = 0;
	health = 1.f;
	slow = 0.f;

	applyForce = false;
	lock = false;
	hit = false;
	flinch = false;
	allowResolve = true;

	setPosition(window->getSize().x * 0.5, window->getSize().y * 0.6);
}

/*for (int i = 0; i < 320; i = i + 64) { walk.addFrame(sf::IntRect(i, 0, 32, 32)); }
	for (int i = 0; i < 320; i = i + 64) { walk.addFrame(sf::IntRect(i, 32, 32, 32)); }*/

	/*if (theta > 345 || theta < 15) { stance = 0; }
	else if (theta < 45) { stance = 1; }
	else if (theta < 75) { stance = 2; }
	else if (theta < 105) { stance = 3; }
	else if (theta < 135) { stance = 4; }
	else if (theta < 165) { stance = 5; }
	else if (theta < 195) { stance = 6; }
	else if (theta < 225) { stance = 7; }
	else if (theta < 255) { stance = 8; }
	else if (theta < 285) { stance = 9; }
	else if (theta < 315) { stance = 10; }
	else { stance = 11; } //if (theta < 345)
	//Do above with a for loop?
	*/

	/*currentAnimation = &walk;

	walk.addFrame(sf::IntRect(p, q, 32, 32));
	walk.addFrame(sf::IntRect(p + 32, q, 32, 32));

	setTextureRect(currentAnimation->getCurrentFrame());*/