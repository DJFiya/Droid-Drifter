#pragma once
#include "stdafx.h"
#include "Player.h"
#include "TileMap.h"

class Game {
private:
	//Data
	sf::RenderWindow window;
	sf::Event ev;
	sf::Texture tileSheet;

	Player* player;
	TileMap* tileMap;
	std::map<std::string, sf::Keyboard::Key> keyBoardMappings;
	std::map<std::string, sf::Mouse::Button> mouseMappings;

	int currentTile = 0;

	//Functions
	void initWindow();
	void initInput();
	void initTileSheet();
	void initPlayer();
	void initTileMap();

public:

	//Initializers
	Game();
	virtual ~Game();

	//Accessors
	int getCurrentTile();

	//Modifiers
	void setCurrentTile(int tileNum);

	//Functions
	void updateInput();
	void updatePlayer();
	void updateCollision();
	void updateTileMap();
	void update();

	void renderPlayer();
	void renderTileMap();
	void render();
	const sf::RenderWindow& getWindow() const;

};

