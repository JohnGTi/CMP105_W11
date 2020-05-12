#pragma once

#include <SFML/Graphics.hpp>

#include "Framework/Input.h"
#include "Framework/AudioManager.h"
#include "Framework/GameState.h"
#include "Framework/GameObject.h"
#include "Framework/Collision.h"
#include "Background.h"

#include "Enemy.h"
#include "Protag.h"

#include <iostream>
#include <string>


class Level {
public:
	Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~Level();

	void handleInput(float dt);
	void update(float dt, bool);
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

	Protag protag;
	sf::Texture prtgBase;

	Enemy enemy;
	sf::Texture enemBase;

	sf::CircleShape prtgHitBox;
	sf::CircleShape enemHitBox;
	//sf::RectangleShape enemHitBox;

	GameObject hitMarker;
	sf::Texture hMtexture;

	Background levelBackground;
	sf::Texture bgArt;

	bool pause = false;
	int yNav = 0;

	GameObject menuShade;
	sf::Texture blackBox;

	sf::Font font;
	sf::Text title;
	sf::Text pointer;
};