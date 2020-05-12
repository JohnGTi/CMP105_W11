#include "Menu.h"

Menu::Menu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;

	// initialise game objects
	audio->addMusic("sfx/cantina.ogg", "cantina");

	// Render text
	if (!font.loadFromFile("font/slkscr.ttf"))
	{
		std::cout << "Error Loading Font\n";
	}

	title.setFont(font);
	title.setCharacterSize(24);
	title.setFillColor(sf::Color::White);

	script.setFont(font);
	script.setCharacterSize(16);
	script.setFillColor(sf::Color::White);

	backPage.setFont(font);
	backPage.setString("Return");
	backPage.setCharacterSize(16);
	backPage.setFillColor(sf::Color::White);
	backPage.setPosition(window->getSize().x / 1.66, window->getSize().y / 1.45);

	blackBox.loadFromFile("gfx/BlackBoxMenu.png");
	menuShade.setTexture(&blackBox);
	menuShade.setSize(sf::Vector2f(1200, 675));
	menuShade.setPosition(0, 0);

	pointer.setFont(font);
	pointer.setString(">");
	pointer.setCharacterSize(16);
	pointer.setFillColor(sf::Color::White);

	onOff.setFont(font);
	onOff.setString("*");
	onOff.setCharacterSize(16);
	onOff.setFillColor(sf::Color::White);

	bgArt.loadFromFile("gfx/BGProtoLined.png");
	menuBackground.setTexture(&bgArt);
	menuBackground.setSize(sf::Vector2f(1200, 2363));
	menuBackground.setPosition(0, 0); //1688 ;
}

Menu::~Menu()
{

}

// handle user input
void Menu::handleInput(float dt)
{
	if (input->isKeyDown(sf::Keyboard::Enter) == true)
	{
		input->setKeyUp(sf::Keyboard::Enter);
		switch (pgNav)
		{
		case 0: pgNav++; break;
		case 1: if (selectNav == 0) { pgNav = 0; std::cout << "Menu>Level.\n"; gameState->setCurrentState(State::LEVEL); }
				else if (selectNav == 1) { pgNav = 2; }
				else { pgNav = 3; } break;
		case 2: if (selectNav == 0) { invincible = true; }
				else if (selectNav == 1) { invincible = false; }
				else { pgNav = 1; selectNav = 1; } break;
		case 3: pgNav = 1; break;
		}
	}
	//std::cout << "pgNav " << pgNav << "; selectNav " << selectNav << ".\n";
	/*std::cout << "Menu>Level.\n";
	gameState->setCurrentState(State::LEVEL);*/
	//reset selector when changing page

	if (input->isKeyDown(sf::Keyboard::W) == true) {
		input->setKeyUp(sf::Keyboard::W);
		switch (pgNav)
		{
		case 0: break;
		case 1: if (selectNav > 0) { selectNav--; } break;
		case 2: if (selectNav < 2) { selectNav = 2; } else { selectNav = 0; } break;
		case 3: break;
		}
	}

	if (input->isKeyDown(sf::Keyboard::A) == true) {
		input->setKeyUp(sf::Keyboard::A);
		if (pgNav == 2 && selectNav == 1) { selectNav--; }
	}

	if (input->isKeyDown(sf::Keyboard::S) == true) {
		input->setKeyUp(sf::Keyboard::S);
		switch (pgNav)
		{
		case 0: break;
		case 1: if (selectNav < 2) { selectNav++; } break;
		case 2: if (selectNav < 2) { selectNav = 2; } else { selectNav = 0; } break;
		case 3: break;
		}
	}

	if (input->isKeyDown(sf::Keyboard::D) == true) {
		input->setKeyUp(sf::Keyboard::D);
		if (pgNav == 2 && selectNav == 0) { selectNav++; }
	}
}

// Update game objects
void Menu::update(float dt)
{
	//std::cout << "(" << input->getMouseX() << "),(" << input->getMouseY() << ")\n";

	switch (pgNav)
	{
	case 0 :
		script.setString("Please enter to begin.");
		script.setPosition(50, window->getSize().y - 74); break;
	case 1 :
		title.setString("Play\nSettings\nHelp");
		title.setPosition(window->getSize().x / 2.75, window->getSize().y / 3);
		script.setString("Navigate with WASD, Enter to select.");
		script.setPosition(window->getSize().x * 0.35, window->getSize().y / 1.45);
		
		pointer.setPosition(navValues[0][selectNav], navValues[1][selectNav]); break;
	case 2 :
		title.setString("Settings");
		title.setPosition(window->getSize().x / 2.75, window->getSize().y / 5);
		script.setString("Debug: Toggle invincibility on/off");
		script.setPosition(window->getSize().x / 2.75, window->getSize().y / 3);
		
		pointer.setPosition(navValues[0][selectNav + 3], navValues[1][selectNav + 3]); break;
	case 3:
		title.setString("Help");
		title.setPosition(window->getSize().x / 2.75, window->getSize().y / 5);
		script.setString("You are pitted against a tall and \nterrible angel.\n\n\nMovement is controlled by the \nWASD keys.\nPoint your character towards the \nenemy using the mouse, attacking \nwith the left mouse button.\n\n\nFor debugging purposes, \ninvincibility can be toggled in \nsettings.");
		script.setPosition(window->getSize().x / 2.8, window->getSize().y / 3);
		pointer.setPosition(navValues[0][5], navValues[1][5]); break;
	}
}

// Render level
void Menu::render()
{
	beginDraw(); window->draw(menuBackground);

	switch (pgNav)
	{
	case 0:
		window->draw(script); break;
	case 1:
		window->draw(menuShade);
		window->draw(title);
		window->draw(script);
		window->draw(pointer); break;
	case 2:
		window->draw(menuShade);
		window->draw(title);
		window->draw(script);
		window->draw(backPage);
		window->draw(pointer);
		if (invincible == true) { onOff.setPosition(721, 210); }
		else { onOff.setPosition(756, 210); }
		window->draw(onOff); break;
	case 3:
		window->draw(menuShade);
		window->draw(title);
		window->draw(script);
		window->draw(backPage);
		window->draw(pointer); break;
	}
	endDraw();
}

// Begins rendering to the back buffer. Background colour set to light blue.
void Menu::beginDraw()
{
	window->clear(sf::Color(100, 149, 237));
}

// Ends rendering to the back buffer, and swaps buffer to the screen.
void Menu::endDraw()
{
	window->display();
}