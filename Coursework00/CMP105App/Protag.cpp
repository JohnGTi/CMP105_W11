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
	//For loop sets up 12 different walking and striking animations (one for each stance).
	currentAnimation = &walk[0];

	sceneVelocity.y = 10.f; //Values set for motion in end scene.
	acceleration.y = 10.f;

	actionBuffer.loadFromFile("sfx/ProtagAttack/ProtagAttack1.wav");
	action.setBuffer(actionBuffer);
	action.setPitch(3);
	exertBuffer.loadFromFile("sfx/ProtagExert/ProtagExert1.wav");
	exert.setBuffer(exertBuffer);
	exert.setVolume(50);
	exert.setPitch(1.5);
	//action.setVolume(0);

	srand(time(0)); //Initialise random num generator using time.
}

Protag::~Protag()
{

}

void Protag::collisionResponse(GameObject* collider, sf::Vector2f enemPos)
{
	if ((enemPos.y + 4) >= (getPosition().y - 28)) { setPosition(getPosition().x, getPosition().y - 0.1f); } //If protag is in contact with enemy, movement in this space is prevented.
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
	if (input->isKeyDown(sf::Keyboard::W) == true && lock == false) //Will not perform movement while the character is locked into an attack.
	{
		stepVelocity.y = -150.0f + slow; //-150.0f; Sets the protag movement speed, offset by slowing effects as a result of damage taken.
		applyForce = true;
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
		lock = true; //Lock protag into attack (vulnerability encourages attention to timing of attacks).
		stanceLock = stance; //Locks player in committed direction.
	}
	if (lock == true) {
		lockDuration += dt;
		currentAnimation = &strike[stanceLock]; //Animate protag strike.
		currentAnimation->animate(dt);

		int xOffset = 2;
		if (enemStance == 2 || enemStance == 3) { xOffset = (-10); } //Enemy circle is offset when the enemy turns to either side so as to maintain precision of collision.
		else if (enemStance == 9) { xOffset = (10); }

		if (lockDuration <= 0.125f && lockDuration <= 0.15f) { //Point v Circle collision detection performed within a small window.
			float deltaX = (enemPos.x + xOffset) - (getPosition().x + strikePoint[0][stance]);
			float deltaY = (enemPos.y - 4) - (getPosition().y + strikePoint[1][stance]);

			if ((deltaX * deltaX + deltaY * deltaY) < (26 * 26)) { hit = true; } //Hit is true if tip of sword connects anywhere with enemy circle.
		}
		if (lockDuration >= 0.15f && lockDuration < (0.15f + dt)) { //&& lockDuration < (0.15f + dt)
			int c = rand() % 14 + 1;
			actionBuffer.loadFromFile("sfx/ProtagAttack/ProtagAttack" + std::to_string(c) + ".wav");
			action.play();
			exertBuffer.loadFromFile("sfx/ProtagExert/ProtagExert" + std::to_string(c) + ".wav");
			exert.play();
		}
		if (lockDuration >= 0.25f) { currentAnimation->reset(); currentAnimation = &walk[stanceLock]; currentAnimation->reset(); } //Reset animation after attack has run.
		if (lockDuration >= 0.35f) { lock = false; lockDuration = 0; hit = false; } //Set period until player can move normally.
	}
}

void Protag::update(float dt, bool enemSuccess, bool invisOn) //Here is where invincibility is passed as damage taken is resolved in this function.
{
	if (enemSuccess) { //Checks enemy attack collision feedback and resolves.
		if (allowResolve) { //allowResolve ensures a sinlge hit only registers as a single hit.
			if (!invisOn) { health = health - 0.05f; } //decrement protag health by... (if not invincible).
			int c = rand() % 20 + 1;
			actionBuffer.loadFromFile("sfx/ProtagExert/ProtagExert" + std::to_string(c) + ".wav");
			action.play();
			slow = slow + 1.625f; //slow protag overall speed by...
			allowResolve = false;
		}
	} if (!enemSuccess) { allowResolve = true; }

	sf::Vector2f pos;
	if (applyForce == true) { //Force applied by WASD key presses.
		pos = stepVelocity * dt; // s = ut
	}
	if ((getPosition().x + pos.x) > -144 && (getPosition().x + pos.x) < window->getSize().x + 144) //Player is not restricted exactly to frame. But will not move if outwith drawn background.
	{
		if ((getPosition().y + pos.y) > -144 && (getPosition().y + pos.y) < window->getSize().y + 144)
		{
			setPosition(getPosition() + pos);
		}
	}
	//^^^Player can only go so far out of bounds.
	stepVelocity = sf::Vector2f(0, 0); //Reset velocity

	float A;
	float B;
	float add = 0.f; 
	//Point A,B is decided by comparison of protag position and mouse position.
	//The series of ifs place the mouse in a quadrant related to the player's position. 

	if (input->getMouseX() > getPosition().x && input->getMouseY() < getPosition().y) {
		A = getPosition().x; //Point (A, B) is set as the third point of a triangle.
		B = input->getMouseY();
		add = 0.f; //Depending on quadrant, an amount is added to the angle calculated to make data appropriate to compare with scale of 0-12 (theta 0-360).
		
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

	theta = adj / hyp; //Trigonometry used to determine angle related to mouse position.
	theta = acos(theta);
	theta = theta * 180 / 3.1415 + add;

	int max = 45;
	for (int p = 1; p < 12; p++) {
		if (theta < max) {
			stance = p; //Stance is set if within range of theta.
			break;
		}
		else { max = max + 30; }
	} if (theta > 345 || theta < 15) { stance = 0; }

	if (lock == false) {
		currentAnimation = &walk[stance]; //Sets animation according to stance.
		if (applyForce == true) { currentAnimation->animate(dt); } //Animates while force applied.
	}
	setTextureRect(currentAnimation->getCurrentFrame());
	applyForce = false;
}

void Protag::resetPlay()
{
	//Reset values as appropriate for retry or new game.
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