#pragma once
#include "stdafx.h"
#include "Player.h"
#include "TileMap.h"
#include "Camera.h"
#include "AIPlayer.h"
#include "GameEvaluator.h"
enum GameMode {PLAYER_MODE = 0, AI_MODE, TRAINING_MODE};
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
	AIPlayer* ai;
	std::map<std::string, sf::Keyboard::Key> keyBoardMappings;
	std::map<std::string, sf::Mouse::Button> mouseMappings;
	GameMode mode = TRAINING_MODE;
	int currentTile = 0;

	//Functions
	void initWindow();
	void initInput();
	void initTileSheet();
	void initFont();
	void initPlayer();
	void initTileMap();
	void initCamera();
	void initAI();

public:

	inline GameMode getMode() { return mode; };

	//Initializers
	Game();
	virtual ~Game();

	//Accessors
	int getCurrentTile();

	//Modifiers
	void setCurrentTile(int tileNum);

	//Functions
	void updateInput();
	void updateAI();
	void updatePlayer();
	void playerMove();
	void updateCollision();
	sf::FloatRect getIntersection(const sf::FloatRect& rect1, const sf::FloatRect& rect2);
	void updatePlayerDamage();
	void updateTileMap();
	
	void updateCamera();
	float clamp(float value, float min, float max);
	
	void update();
	void updateTrainingStep();

	bool playerWin();
	bool playerLost();

	void renderPlayer();
	void renderTileMap();
	void renderHP();
	void renderGameOver();
	void render();
	const sf::RenderWindow& getWindow() const;

};

