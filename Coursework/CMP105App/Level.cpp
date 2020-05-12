#include "Level.h"

Level::Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;

	// initialise game objects
	audio->addMusic("sfx/cantina.ogg", "cantina");

	prtgBase.loadFromFile("gfx/ProtagARW.png");
	protag.setTexture(&prtgBase);
	protag.setSize(sf::Vector2f(128, 128));
	protag.setOrigin(64, 64);
	protag.setPosition(window->getSize().x * 0.5, window->getSize().y * 0.6);
	protag.setCollisionBox(sf::FloatRect(-12, -28, 22, 6));
	protag.setInput(input);
	protag.receiveWin(window);

	enemBase.loadFromFile("gfx/EnemyARW.png");
	enemy.setTexture(&enemBase);
	enemy.setSize(sf::Vector2f(128, 128));
	enemy.setOrigin(64, 64);
	enemy.setPosition(window->getSize().x * 0.5, -334);
	enemy.setCollisionBox(sf::FloatRect(-16, 4, 40, 6)); // Starts at origin, duplicate values (size is x2).
	enemy.setInput(input);

	prtgHitBox.setRadius(22);
	prtgHitBox.setPosition(protag.getPosition().x - 22, protag.getPosition().y - 28); //26x25 x-24 y-30
	prtgHitBox.setFillColor(sf::Color::Blue);

	enemHitBox.setRadius(50);
	enemHitBox.setPosition(enemy.getPosition().x - 50, enemy.getPosition().y - 50); //26x25 x-24 y-30
	enemHitBox.setFillColor(sf::Color::Red);

	/*enemHitBox.setSize(sf::Vector2f(enemy.getCollisionBox().width, enemy.getCollisionBox().height));
	enemHitBox.setPosition(enemy.getCollisionBox().left, enemy.getCollisionBox().top);
	enemHitBox.setFillColor(sf::Color::Blue);*/
	//x y width height protag.getCollisionBox().left top width height

	hMtexture.loadFromFile("gfx/hitmarker.png");
	hitMarker.setTexture(&hMtexture);
	hitMarker.setSize(sf::Vector2f(20, 20));
	hitMarker.setOrigin(hitMarker.getSize().x / 2, hitMarker.getSize().y / 2);
	hitMarker.setPosition(window->getSize().x * 0.25, window->getSize().y * 0.25);
	hitMarker.setInput(input);

	bgArt.loadFromFile("gfx/BGProtoLined.png");

	levelBackground.setTexture(&bgArt);
	levelBackground.setSize(sf::Vector2f(1200, 2363));
	levelBackground.setPosition(0, -1519); //1688 ;
	levelBackground.setInput(input);

	blackBox.loadFromFile("gfx/BlackBoxMenu.png");
	menuShade.setTexture(&blackBox);
	menuShade.setSize(sf::Vector2f(1200, 675));
	menuShade.setPosition(0, 0);

	// Render text
	if (!font.loadFromFile("font/slkscr.ttf"))
	{
		std::cout << "Error Loading Font\n";
	}

	title.setFont(font);
	title.setString("Resume\nExit");
	title.setCharacterSize(24);
	title.setFillColor(sf::Color::White);
	title.setPosition(window->getSize().x / 2.75, window->getSize().y / 3);

	pointer.setFont(font);
	pointer.setString(">");
	pointer.setCharacterSize(16);
	pointer.setFillColor(sf::Color::White);
}

Level::~Level()
{

}

// handle user input
void Level::handleInput(float dt)
{
	protag.handleInput(dt, enemy.getPosition(), enemy.getStance());
	enemy.handleInput(dt);

	if (input->isKeyDown(sf::Keyboard::Enter) == true)
	{
		input->setKeyUp(sf::Keyboard::Enter);
		if (!pause) { pause = true; }
		else if (yNav == 0 && protag.getHealth() > 0){ title.setString("Resume\nExit"); pause = false; }
		else {
			protag.resetPlay();
			enemy.resetPlay();
			pause = false;
			enemy.setPosition(window->getSize().x * 0.5, -334);
			if (yNav == 1) {
				yNav = 0;
				std::cout << "Level>Menu.\n";
				gameState->setCurrentState(State::MENU);
			} yNav = 0;
		}
	}

	if (input->isKeyDown(sf::Keyboard::W) == true && pause)
	{
		input->setKeyUp(sf::Keyboard::W);
		if (yNav == 1) { yNav--; }
		else { yNav++; }
	}

	if (input->isKeyDown(sf::Keyboard::S) == true && pause)
	{
		input->setKeyUp(sf::Keyboard::S);
		if (yNav == 0) { yNav++; }
		else { yNav--; }
	}
}

// Update game objects
void Level::update(float dt, bool invincibility)
{
	prtgHitBox.setPosition(protag.getPosition().x - 22, protag.getPosition().y - 28);
	enemHitBox.setPosition(enemy.getPosition().x - 50, enemy.getPosition().y - 50);
	//prtgHitBox.setPosition(protag.getCollisionBox().left, protag.getCollisionBox().top);
	//enemHitBox.setPosition(enemy.getCollisionBox().left, enemy.getCollisionBox().top);

	//protag.setPosition(window->getSize().x * 0.5, window->getSize().y * 0.5);


	if (protag.getHealth() > 0 && pause == false) { protag.update(dt, enemy.getHit(), invincibility); }
	if (enemy.getHealth() > 0 && pause == false) { enemy.update(dt, protag.getPosition(), protag.getHit()); } //Should update regardless. 'If health' should change gamestate.

	if (Collision::checkBoundingBox(&protag, &enemy)) {
		protag.collisionResponse(NULL, enemy.getPosition());
		enemy.collisionResponse(NULL);
	}

	if (protag.getHealth() < 0 && pause == false) {
		//Relocate to death scene if animation is completed.
		title.setString("Retry\nExit");
		pause = true;
	}

	if (enemy.getHealth() <= 0 && pause == false) {
		//Relocate to death scene if animation is completed.
		if (levelBackground.sceneEnd(dt)) {
			protag.resetPlay();
			enemy.resetPlay();
			pause = false;
			enemy.setPosition(window->getSize().x * 0.5, -334);
			yNav = 0;
			std::cout << "Level>Menu.\n";
			gameState->setCurrentState(State::MENU); //Harsh cut in with text is no good.
		}
		else {
			enemy.sceneEnd(dt);
			protag.sceneEnd(dt);
		}
	}

	if (pause == true) {
		if (yNav == 0) { pointer.setPosition(425, 233); }
		else { pointer.setPosition(425, 257); }
	}
	//std::cout << "(" << input->getMouseX() << "),(" << input->getMouseY() << ")\n";
}

// Render level
void Level::render()
{
	beginDraw();
	window->draw(levelBackground);

	//window->draw(enemHitBox);
	//window->draw(prtgHitBox);

	if ((protag.getPosition().x + 28) < (enemy.getPosition().x - 34) || (protag.getPosition().x - 16) > (enemy.getPosition().x + 34)) {
		if (enemy.getTheta() > 90 && enemy.getTheta() < 270) {
			window->draw(enemy); window->draw(protag);
		}
		else {
			window->draw(protag); window->draw(enemy);
		}
	}
	else if ((protag.getPosition().y - 28) < (enemy.getPosition().y + 4)) { window->draw(protag); window->draw(enemy); }
	else { window->draw(enemy); window->draw(protag); }

	//window->draw(prtgHitBox);
	//window->draw(enemHitBox);

	hitMarker.setPosition(protag.getPosition().x + protag.getstrikePoint(0), protag.getPosition().y + protag.getstrikePoint(1));
	if (protag.getHit()) { window->draw(hitMarker); }
	hitMarker.setPosition(protag.getPosition().x, protag.getPosition().y);
	if (enemy.getHit()) { window->draw(hitMarker); }

	if (pause == true) {
		window->draw(menuShade);
		window->draw(pointer);
		window->draw(title);
	}

	//std::cout << "(" << protag.getPosition().y << "),(" << input->getMouseY() << ")\n";
	endDraw();
}

// Begins rendering to the back buffer. Background colour set to light blue.
void Level::beginDraw()
{
	window->clear(sf::Color(100, 149, 237));
}

// Ends rendering to the back buffer, and swaps buffer to the screen.
void Level::endDraw()
{
	window->display();
}