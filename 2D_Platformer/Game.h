#pragma once
#include "stdafx.h"
#include "Player.h"
#include "TileMap.h"
#include "Camera.h"

class Game {
private:
	//Data
	sf::RenderWindow window;
	sf::Event ev;
	sf::Texture tileSheet;
	sf::Font font;

	Player* player;
	TileMap* tileMap;
	Camera* camera;
	std::map<std::string, sf::Keyboard::Key> keyBoardMappings;
	std::map<std::string, sf::Mouse::Button> mouseMappings;

	int currentTile = 0;

	//Functions
	void initWindow();
	void initInput();
	void initTileSheet();
	void initFont();
	void initPlayer();
	void initTileMap();
	void initCamera();

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
	void playerMove();
	void updateCollision();
	sf::FloatRect getIntersection(const sf::FloatRect& rect1, const sf::FloatRect& rect2);
	void updatePlayerDamage();
	void updateTileMap();
	
	void updateCamera();
	float clamp(float value, float min, float max);
	
	void update();

	void renderPlayer();
	void renderTileMap();
	void renderHP();
	void renderGameOver();
	void render();
	const sf::RenderWindow& getWindow() const;

};

