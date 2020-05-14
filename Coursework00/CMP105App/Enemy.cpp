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
	//For loop sets up 12 different walking and striking animations (one for each stance).
	currentAnimation = &walk[0];
	speed = 82.5f;
	
	stepVelocity.y = 10.f; //Values set for motion in end scene.
	acceleration.y = 10.f;

	actionBuffer.loadFromFile("sfx/EnemyExert/EnemyExert1.wav");
	action.setBuffer(actionBuffer);
	impactBuffer.loadFromFile("sfx/ProtagImpact/ProtagImpact1.wav");
	impact.setBuffer(impactBuffer);

	srand(time(0)); //Initialise random num generator using time.
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
	int c = rand() % 14 + 1;
	//If object is close enough to target, the enemy picks up the pace/
	if (Vector::magnitude(target - getPosition()) < 219.f) {
		walk[stance].setFrameSpeed(1.5f / 10.f); //At a quicker pace, the walking animation is sped up.
		speed = 100.f - slow; //Set speed can be offset by variable as damage is taken.
	}
	else { walk[stance].setFrameSpeed(2.25f / 10.f); }

	//Calculate direction and move

	if (flinch == false) { 
		direction = target - getPosition(); //Enemy will move in the opposite direction of player when flinching (well timed stagger).
	}
	else {
		direction = getPosition() - target;
	}
	direction = Vector::normalise(direction);
	velocity = (direction * speed);

	if (target.x != 600) { initial = true; } //Players initial movement triggers enemy to enter frame.

	if (lock == false || flinch == true) { if (initial) { setPosition(getPosition() + (velocity * dt)); } } //Authorises and resolves player movement.
	speed = 82.5f - slow;

	if (Vector::magnitude(target - getPosition()) < 60.f && lock == false) //Enemy decides to attack when within range.
	{
		int c = rand() % 12 + 1;
		actionBuffer.loadFromFile("sfx/EnemyExert/EnemyExert" + std::to_string(c) + ".wav");
		action.play();
		lock = true; //Enemy commits to attack.
		stanceLock = stance; //Enemy commits to aim. (All similar to protag).
	}
	if (lock == true) {
		lockDuration += dt;
		if (flinch == false) { currentAnimation = &strike[stanceLock]; currentAnimation->animate(dt); } //Strike animation.

		if (prtgSuccess) {
			if (!flinchRes && lockDuration >= 0.f && lockDuration < 0.0125f) { //Enemy successfully staggered if player connects on first frame of enemy attack. Flinch is subtle, serves to allow more time for player to step back.
				flinch = true; flinchRes = true; currentAnimation->reset(); currentAnimation = &strike[stanceLock - 4]; //std::cout << "Flinch.\n";
				impactBuffer.loadFromFile("sfx/ProtagImpact/ProtagImpact" + std::to_string(c) + ".wav");
			}
			if (allowResolve) {
				health = health - 0.025f; //decrement enemy health by...
				impact.play();
				c = rand() % 19 + 1;
				actionBuffer.loadFromFile("sfx/EnemyWince/EnemyWince" + std::to_string(c) + ".wav");
				action.play();
				if (health < 0.35f && slow == 0.f) { //At 0.35, the enemy's health is reset. Further damage slows movement.
					health = 1.f;
					slow = slow + 1.625f;
				} else if (slow > 0.f) { slow = slow + 1.625f; }

				allowResolve = false;
			}
		}
		if (!prtgSuccess) { allowResolve = true; }

		if (lockDuration >= 0.375f && lockDuration <= 0.5f && flinch == false) { //&& flinch == false
			float deltaX = (getPosition().x - target.x);
			float deltaY = (getPosition().y - (target.y - 6));
			if ((deltaX * deltaX + deltaY * deltaY) < ((70 + 22) * (70 + 22))) {
				if (stanceLock == stance) { hit = true; } //Enemy stance points to player position, if the stance position saved when the enemy struck is the same as the updating stance, then the player is in range.
			}
			//The above if allows the attack range to be more generous, a whole circle representing the enemy's reach, then sectioned off by a check to ensure the player is in range of specific attack.
			//This allows strategic maneuvering as the player can bait an attack and reposition around the enemy to safely strike.
		}
		if (lockDuration >= 0.0125f && flinch == true) { flinch = false; lockDuration = 0.f; hit = false; currentAnimation->reset(); } //After the stagger period, the lock is not lifted, but reset. This triggers the enemy to immediately attack and combats stagger locking.
		if (lockDuration >= 0.5f) { currentAnimation->reset(); currentAnimation = &walk[stanceLock]; currentAnimation->reset(); }
		if (lockDuration >= 0.68f && flinch == false) { lock = false; lockDuration = 0; hit = false; flinchRes = false; }
	}

	float A;
	float B;
	float add = 0.f; //Same technique used in protag class. Evaluates relation between two points. Builds a third to be used to form a triangle and find the angle of direction.
	if (target.x > getPosition().x && target.y < getPosition().y) {
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
	//Previous code functions as in protag.

	//The below circle v circle fixes the error where the player could fit snug underneath the origin of the enemy on the far side, and the enemy would face the opposite direction and be unable to properly attack.
	float deltaX = (getPosition().x - target.x);
	float deltaY = (getPosition().y - (target.y));
	if ((deltaX * deltaX + deltaY * deltaY) < ((32 + 26) * (32 + 26))) { stance = 0; } //The two circles meet where this error would occur.

	if (lock == false) {
		currentAnimation = &walk[stance];
		currentAnimation->animate(dt);
	}
	setTextureRect(currentAnimation->getCurrentFrame());
}

void Enemy::resetPlay()
{
	//Reset values as appropriate for retry or new game.
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
