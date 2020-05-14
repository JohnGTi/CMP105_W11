#pragma once

#include <SFML/Graphics.hpp>
#include "Framework/Input.h"
#include "Framework/AudioManager.h"
#include "Framework/GameState.h"
#include "Framework/GameObject.h"
#include "Framework/Animation.h"

#include <string>
#include <iostream>


class Menu {
public:
	Menu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~Menu();

	bool getMagic() { return invincible; }; //Setting screen allows this boolean to be toggled. Getter in place so the status can take effect in game.

	void handleInput(float dt);
	void update(float dt);
	void render();

private:
	// Default functions for rendering to the screen.
	void beginDraw();
	void endDraw();

	// Default variables for level class.
	sf::RenderWindow* window;
	Input* input;
	GameState* gameState;
	AudioManager* audio;

	sf::Music menuAmbience;

	sf::Font font;
	sf::Text title;
	sf::Text script;
	sf::Text pointer;
	sf::Text backPage;
	sf::Text onOff;
	//Above text are manipulated to display varied UI per page.

	GameObject menuShade; //GameObject serves as a backdrop for pause menu/death screen.
	sf::Texture blackBox;

	GameObject menuBackground; //Stationary canvas, framed at peak of image.
	sf::Texture bgArt;

	int navValues[2][6] = { {425, 425, 425, 725, 762, 710}, {233, 257, 282, 242, 242, 465} }; //Corresponding page and selection values instruct pointer to the user's position.
	int pgNav = 0; //Indication and traversal of menu screens.
	int selectNav = 0; //Indication and traversal of selection options.

	bool invincible = false;

	float elapsedDt; //Used to implement smoother transitions. Animate a fade over elapsedDt as opposed to an instant cut to the next screen.
	float opacity = 0; //increments by elapsed time / fade period * 255 (used to set alpha channel value).
};