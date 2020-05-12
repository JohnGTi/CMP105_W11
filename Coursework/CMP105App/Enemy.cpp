#include "Enemy.h"
#include <iostream>

Enemy::Enemy()
{
	int startPosY = 0;
	for (int i = 0; i < 12; i++) {
		walk[i].setFrameSpeed(2.25f / 10.f);
		walk[i].addFrame(sf::IntRect(0, startPosY, 64, 64));
		walk[i].addFrame(sf::IntRect(64, startPosY, 64, 64));
		walk[i].addFrame(sf::IntRect(128, startPosY, 64, 64));

		strike[i].setFrameSpeed(1.25f / 10.f);
		strike[i].addFrame(sf::IntRect(192, startPosY, 64, 64));
		strike[i].addFrame(sf::IntRect(256, startPosY, 64, 64));
		strike[i].addFrame(sf::IntRect(320, startPosY, 64, 64));
		strike[i].addFrame(sf::IntRect(384, startPosY, 64, 64));
		startPosY += 64;
	}

	currentAnimation = &walk[0];
	speed = 82.5f;

	stepVelocity.y = 10.f;
	acceleration.y = 10.f;
}

Enemy::~Enemy()
{

}

void Enemy::sceneEnd(float dt)
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
}

void Enemy::collisionResponse(GameObject* collider)
{

}

void Enemy::handleInput(float dt)
{

}

void Enemy::update(float dt, sf::Vector2f target, bool prtgSuccess)
{
	// if object is close enough to target
	if (Vector::magnitude(target - getPosition()) < 219.f) {
		walk[stance].setFrameSpeed(1.5f / 10.f);
		speed = 100.f - slow;
	}
	else { walk[stance].setFrameSpeed(2.25f / 10.f); }

	// calculate direction and move

	if (flinch == false) {
		direction = target - getPosition();
	}
	else {
		direction = getPosition() - target;
	}
	direction = Vector::normalise(direction);
	velocity = (direction * speed);

	if (target.x != 600) { initial = true; }

	if (lock == false || flinch == true) { if (initial) { setPosition(getPosition() + (velocity * dt)); } }
	speed = 82.5f - slow;

	if (Vector::magnitude(target - getPosition()) < 60.f && lock == false) //if (Vector::magnitude(target - getPosition()) < 219.f && lock == false)
	{
		//std::cout << "(" << getPosition().x << "),(" << getPosition().y << ")\n";
		lock = true;
		stanceLock = stance;
	}
	if (lock == true) {
		lockDuration += dt;
		if (flinch == false) { currentAnimation = &strike[stanceLock]; currentAnimation->animate(dt); }

		if (prtgSuccess) {
			if (!flinchRes && lockDuration >= 0.f && lockDuration < 0.0125f) {
				flinch = true; flinchRes = true; currentAnimation->reset(); currentAnimation = &strike[stanceLock - 4]; //std::cout << "Flinch.\n";
			}
			if (allowResolve) {
				health = health - 0.025f; //decrement enemy health by...
				
				if (health < 0.35f && slow == 0.f) {
					health = 1.f;
					slow = slow + 1.625f;
				} else if (slow > 0.f) { slow = slow + 1.625f; }

				allowResolve = false; std::cout << "enemy.health " << health <<".\n";
			}
		}
		if (!prtgSuccess) { allowResolve = true; }

		if (lockDuration >= 0.375f && lockDuration <= 0.5f && flinch == false) { //&& flinch == false
			float deltaX = (getPosition().x - target.x);
			float deltaY = (getPosition().y - (target.y - 6));
			if ((deltaX * deltaX + deltaY * deltaY) < ((70 + 22) * (70 + 22))) {
				if (stanceLock == stance) { hit = true; } //Enemy stance points to player position, if the stance position saved when the enemy struck is the same as the updating stance, then the player is in range.
			} //std::cout << "Hit.\n"; //stanceLock >= stance - 1 || stanceLock <= stance + 1
		}
		if (lockDuration >= 0.0125f && flinch == true) { flinch = false; lockDuration = 0.f; hit = false; currentAnimation->reset(); } //0.82f
		if (lockDuration >= 0.5f) { currentAnimation->reset(); currentAnimation = &walk[stanceLock]; currentAnimation->reset(); }
		if (lockDuration >= 0.68f && flinch == false) { lock = false; lockDuration = 0; hit = false; flinchRes = false; } //flinch = false; //0.63f
		//if (lockDuration >= 0.82f && flinch == true) { lock = false; lockDuration = 0; flinch = false; } //0.82f
	}

	float A;
	float B;
	float add = 0.f;
	if (target.x > getPosition().x && target.y < getPosition().y) { //(getPosition().y - 42) may improve accuracy of graphical representation (enemy head points to player body).
		A = getPosition().x;
		B = target.y;
		add = 0.f;
	} //C
	else if (target.x > getPosition().x && target.y > getPosition().y) {
		A = target.x;
		B = getPosition().y;
		add = 90.f;
	} //A
	else if (target.x < getPosition().x && target.y > getPosition().y) {
		A = getPosition().x;
		B = target.y;
		add = 180.f;
	} //S
	else {
		A = target.x;
		B = getPosition().y;
		add = 270.f;
	} //T
	//(input->getMouseX() < getPosition().x && input->getMouseY() < getPosition().y)

	// P(A, B)
	//adj^2 = sqrt/(Py - protagy)^2 + (Px - protagx)^2'
	float adj = (((A - getPosition().x) * (A - getPosition().x) + (B - getPosition().y) * (B - getPosition().y)));
	adj = sqrt(adj);
	float hyp = ((target.x - getPosition().x) * (target.x - getPosition().x)) + ((target.y - getPosition().y) * (target.y - getPosition().y));
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
	}
	if (theta > 345 || theta < 15) { stance = 0; }

	float deltaX = (getPosition().x - target.x);
	float deltaY = (getPosition().y - (target.y));
	if ((deltaX * deltaX + deltaY * deltaY) < ((32 + 26) * (32 + 26))) { stance = 0; }

	if (lock == false) {
		currentAnimation = &walk[stance];
		currentAnimation->animate(dt);
	}
	setTextureRect(currentAnimation->getCurrentFrame());
}

void Enemy::resetPlay()
{
	lockDuration = 0;
	health = 1.f;
	slow = 0.f;

	lock = false;
	flinch = false;
	flinchRes = false;
	hit = false;
	allowResolve = true;
	initial = false;
}
