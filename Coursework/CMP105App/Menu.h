#pragma once

#include <SFML/Graphics.hpp>
#include "Framework/Input.h"
#include "Framework/AudioManager.h"
#include "Framework/GameState.h"
#include "Framework/GameObject.h"
#include <string>
#include <iostream>


class Menu {
public:
	Menu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~Menu();

	bool getMagic() { return invincible; };

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

	sf::Font font;
	sf::Text title;
	sf::Text script;
	sf::Text pointer;
	sf::Text backPage;
	sf::Text onOff;

	GameObject menuShade;
	sf::Texture blackBox;

	GameObject menuBackground;
	sf::Texture bgArt;

	int navValues[2][6] = { {425, 425, 425, 725, 762, 710}, {233, 257, 282, 242, 242, 465} };
	int pgNav = 0;
	int selectNav = 0;

	bool invincible = false;

	float elapsedDt;
};