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
	sf::Texture prtgBase; //Protag(onist) class. Player controlled GameObject.

	Enemy enemy;
	sf::Texture enemBase; //Enemy class. GameObject pursues player.

	sf::CircleShape prtgHitBox;
	sf::CircleShape enemHitBox; //Two shapes used to visualise collision detection for refining interactions.
	//sf::RectangleShape enemHitBox;

	GameObject hitMarker;
	sf::Texture hMtexture; //Hit marker indicates successful hits for the respective entities.

	Background levelBackground; //Stationary canvas, animates with a player win for effective transition.
	sf::Texture bgArt;

	bool pause = false; //Boolean controlling pause state.
	int yNav = 0; //Up and down navigation for pause menu/death screen.

	GameObject menuShade; //GameObject serves as a backdrop for pause menu/death screen.
	sf::Texture blackBox;

	sf::Font font; //Font points to a piece of work that is not my own. Should be referenced in report.
	sf::Text title; //Menu text for pause and death screen.
	sf::Text pointer;
};