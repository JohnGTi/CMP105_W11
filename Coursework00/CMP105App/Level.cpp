#include "Level.h"

Level::Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;

	//Initialise game objects
	//audio->addMusic("sfx/cantina.ogg", "cantina");

	prtgBase.loadFromFile("gfx/ProtagARW.png");
	protag.setTexture(&prtgBase);
	protag.setSize(sf::Vector2f(128, 128));
	protag.setOrigin(64, 64);
	protag.setPosition(window->getSize().x * 0.5, window->getSize().y * 0.6);
	protag.setCollisionBox(sf::FloatRect(-12, -28, 22, 6)); //Collision dimensions set manually.
	protag.setInput(input);
	protag.receiveWin(window); //Set up for player includes setting collision boxes, positioning, passing parameters, etc.

	enemBase.loadFromFile("gfx/EnemyARW.png");
	enemy.setTexture(&enemBase);
	enemy.setSize(sf::Vector2f(128, 128));
	enemy.setOrigin(64, 64);
	enemy.setPosition(window->getSize().x * 0.5, -334);
	enemy.setCollisionBox(sf::FloatRect(-16, 4, 40, 6)); 
	enemy.setInput(input);

	prtgHitBox.setRadius(22);
	prtgHitBox.setPosition(protag.getPosition().x - 22, protag.getPosition().y - 28);
	prtgHitBox.setFillColor(sf::Color::Blue); //Sets up (previously square) circle to represent attack collision (enemy side).

	enemHitBox.setRadius(50);
	enemHitBox.setPosition(enemy.getPosition().x - 50, enemy.getPosition().y - 50);
	enemHitBox.setFillColor(sf::Color::Red);

	hMtexture.loadFromFile("gfx/hitmarker.png");
	hitMarker.setTexture(&hMtexture);
	hitMarker.setSize(sf::Vector2f(20, 20));
	hitMarker.setOrigin(hitMarker.getSize().x / 2, hitMarker.getSize().y / 2); //Centres origin for hitmarker.

	bgArt.loadFromFile("gfx/BGProtoLined.png"); //Background art currently only in prototype form.

	levelBackground.setTexture(&bgArt);
	levelBackground.setSize(sf::Vector2f(1200, 2363));
	levelBackground.setPosition(0, -1519); //Frames background at lowest point (image is larger than window to allow for end scene).
	levelBackground.setInput(input);

	blackBox.loadFromFile("gfx/BlackBoxAlpha.png");
	menuShade.setTexture(&blackBox);
	menuShade.setSize(sf::Vector2f(1200, 675));
	menuShade.setPosition(0, 0);

	// Render text
	if (!font.loadFromFile("font/slkscr.ttf")) //Validates font load.
	{
		std::cout << "Error Loading Font\n";
	}

	title.setFont(font);
	title.setString("Resume\nExit"); //String is changed for death screen to "Retry\nExit."
	title.setCharacterSize(24);
	title.setFillColor(sf::Color::White);
	title.setPosition(window->getSize().x / 2.75, window->getSize().y / 3);

	pointer.setFont(font);
	pointer.setString(">"); //Indicates user position on the menus.
	pointer.setCharacterSize(16);
	pointer.setFillColor(sf::Color::White);

	cursorImg.loadFromFile("gfx/mousePointer.png");
	cursor.setTexture(&cursorImg);
	cursor.setSize(sf::Vector2f(20, 20));
	cursor.setOrigin(sf::Vector2f(10, 10));
	//cursor.setFillColor(sf::Color(255, 255, 255, 128)); Set Opacity for any biz.
	window->setMouseCursorVisible(false);

	menuAmbience.openFromFile("sfx/Ambient/AmbientMenu.wav");
	menuAmbience.play();
	menuAmbience.setVolume(0);
	menuAmbience.setLoop(true);

	levelAmbience.openFromFile("sfx/Ambient/AmbientLevel.wav");
	levelAmbience.play();
	levelAmbience.setVolume(0);
	levelAmbience.setLoop(true);
}

Level::~Level()
{

}

// handle user input
void Level::handleInput(float dt)
{
	protag.handleInput(dt, enemy.getPosition(), enemy.getStance());
	enemy.handleInput(dt);

	if (input->isKeyDown(sf::Keyboard::Enter) == true) //Enter key pauses game.
	{
		input->setKeyUp(sf::Keyboard::Enter);
		if (!pause) { pause = true; } //Decision so that enter can be pressed again to make selection.
		else if (yNav == 0 && protag.getHealth() > 0){ title.setString("Resume\nExit"); pause = false; } //This path resumes game (pause screen).
		else {
			protag.resetPlay(); //Else begins with resetting the stage. User is either exiting or retrying.
			enemy.resetPlay();
			pause = false;
			enemy.setPosition(window->getSize().x * 0.5, -334);
			if (yNav == 1) { //If the enter key was pressed on navY 1, the user selected exit.
				yNav = 0;
				levelAmbience.setVolume(0);
				std::cout << "Level>Menu.\n";
				gameState->setCurrentState(State::MENU); //Change gameState to the Main Menu (splash screen, as menuPage is reset when the level is entered).
			} yNav = 0; //Set to zero in preparation for screen reopening.
		}
	}

	if (input->isKeyDown(sf::Keyboard::W) == true && pause) //W and S are utilised for navigation while the menu is open.
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
	menuAmbience.setVolume(2);

	cursor.setPosition(input->getMouseX(), input->getMouseY());
	cursor.setRotation(protag.getTheta());

	prtgHitBox.setPosition(protag.getPosition().x - 22, protag.getPosition().y - 28); //Updates positioning for hitboxes, should they need to be drawn.
	enemHitBox.setPosition(enemy.getPosition().x - 50, enemy.getPosition().y - 50);

	if (protag.getHealth() > 0 && pause == false) { //Protag and enemy will not update during pause and when their respective health is fully diminished.
		protag.update(dt, enemy.getHit(), invincibility);
	}
	if (enemy.getHealth() > 0 && pause == false) { levelAmbience.setVolume(50); enemy.update(dt, protag.getPosition(), protag.getHit()); } //volume 50

	if (Collision::checkBoundingBox(&protag, &enemy)) { //Function part of collision class utilised for player and enemybody collision.
		protag.collisionResponse(NULL, enemy.getPosition()); //As the space attempts to emulate 3dimensions, the collision boxes allow generous movement around each other.
		enemy.collisionResponse(NULL); //Main purpose of this check is to ensure characters do not walk through each other.
	}

	if (protag.getHealth() < 0 && pause == false) { //Checks for player death, resolves with death screen and options to retry or quit.
		//Relocate to death scene if animation is completed.
		title.setString("Retry\nExit"); //Death screen draws string as 'Retry' as opposed to 'Resume.'
		pause = true;
	}

	if (enemy.getHealth() <= 0 && pause == false) { //Checks for enemy death, resolves with 'death scene' (entity animations yet to be implemented), camera pans away from protag drawing closer to the fallen enemy.
		//Relocate to death scene if animation is completed.
		if (levelBackground.sceneEnd(dt)) { //Scene (camera pan and pull back) plays out in function until true is returned.
			protag.resetPlay(); //From here, the stage can be reset and the transition back to the main menu complete.
			enemy.resetPlay();
			pause = false;
			enemy.setPosition(window->getSize().x * 0.5, -334);
			yNav = 0;
			//protagBreath.setVolume(0);
			levelAmbience.setVolume(0);
			std::cout << "Level>Menu.\n";
			gameState->setCurrentState(State::MENU);
		}
		else {
			enemy.sceneEnd(dt); //Entities must follow opposite of the pan.
			protag.sceneEnd(dt);
		}
	}

	if (pause == true) {
		if (yNav == 0) { pointer.setPosition(425, 233); } //Repositions the selection pointer depending on nav value.
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

	if ((protag.getPosition().x + 28) < (enemy.getPosition().x - 34) || (protag.getPosition().x - 16) > (enemy.getPosition().x + 34)) { //Series of ifs consider relation between player and entity position and decide which order to render.
		if (enemy.getTheta() > 90 && enemy.getTheta() < 270) { //Allows attack to appear properly when in range.
			window->draw(enemy); window->draw(protag);
		}
		else {
			window->draw(protag); window->draw(enemy);
		}
	}
	else if ((protag.getPosition().y - 28) < (enemy.getPosition().y + 4)) { window->draw(protag); window->draw(enemy); } //First if considers outside of set x range, this decision is based on closer proximity.
	else { window->draw(enemy); window->draw(protag); }

	//window->draw(prtgHitBox);
	//window->draw(enemHitBox);

	hitMarker.setPosition(protag.getPosition().x + protag.getstrikePoint(0), protag.getPosition().y + protag.getstrikePoint(1)); //positions the hitmarker ready to be drawn for successful protag attacks.
	if (protag.getHit()) { window->draw(hitMarker); }
	hitMarker.setPosition(protag.getPosition().x, protag.getPosition().y); //Vice versa for enemy attacks.
	if (enemy.getHit()) { window->draw(hitMarker); }

	if (pause == true) { //Displays menu objects.
		window->draw(menuShade);
		window->draw(pointer);
		window->draw(title);
	}

	if (enemy.getHealth() > 0) { window->draw(cursor); } //Cursor will not display after player win.
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