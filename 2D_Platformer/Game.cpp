#include "stdafx.h"
#include "Game.h"

void Game::initWindow(){
	this->window.create(sf::VideoMode(960, 640), "Platformer AI", sf::Style::Close | sf::Style::Titlebar);
	this->window.setFramerateLimit(90);
}

void Game::initInput() {
	this->mouseMappings["BTN_ADD_TILE"] = sf::Mouse::Button::Left;
	this->mouseMappings["BTN_REMOVE_TILE"] = sf::Mouse::Button::Right;

	for (int i = 0; i < 8; i++) {
		this->keyBoardMappings["Tile_" + std::to_string(i)] = static_cast<sf::Keyboard::Key>(sf::Keyboard::Num0 + i);
	}

	this->keyBoardMappings["KEY_MOVE_LEFT"] = sf::Keyboard::Key::A;
	this->keyBoardMappings["KEY_MOVE_RIGHT"] = sf::Keyboard::Key::D;
	this->keyBoardMappings["KEY_JUMP"] = sf::Keyboard::Key::W;
	this->keyBoardMappings["KEY_ATTACK"] = sf::Keyboard::Key::Space;

}

void Game::initTileSheet(){
	if (!this->tileSheet.loadFromFile("Textures/tile_sheet.png")) {
		std::cout << "ERROR: Tile Sheet Texture failed to load!\n";
	}
}

void Game::initPlayer(){
	this->player = new Player();
}

void Game::initTileMap(){
	this->tileMap = new TileMap(20, 20, &this->tileSheet, 64);
}

Game::Game(){
	this->initWindow();
	this->initInput();
	this->initTileSheet();
	this->initTileMap();
	this->initPlayer();
}

Game::~Game(){
	delete this->player;
	delete this->tileMap;
}

int Game::getCurrentTile()
{
	return this->currentTile;
}

void Game::setCurrentTile(int tileNum)
{
	this->currentTile = tileNum;
}

void Game::updateInput() {
	const int mouseX = int(sf::Mouse::getPosition(this->getWindow()).x) / int(this->tileMap->getTileSize());
	const int mouseY = int(sf::Mouse::getPosition(this->getWindow()).y) / int(this->tileMap->getTileSize());

	//
	if (sf::Keyboard::isKeyPressed(this->keyBoardMappings["KEY_MOVE_LEFT"])) {
		this->player->move(-1.f, 0.f);
	}
	else if (sf::Keyboard::isKeyPressed(this->keyBoardMappings["KEY_MOVE_RIGHT"])) {
		this->player->move(1.f, 0.f);
	}
	if (sf::Keyboard::isKeyPressed(this->keyBoardMappings["KEY_JUMP"]) && this->player->getCanJump()) {
		this->player->jump();
	}
	else if ((!sf::Keyboard::isKeyPressed(this->keyBoardMappings["KEY_JUMP"])) && this->player->getVelocity().y<0) {
		this->player->shortjump();
	}

	//Set Tile
	for (int i = 0; i <= 7; ++i) {
		if (sf::Keyboard::isKeyPressed(this->keyBoardMappings["Tile_" + std::to_string(i)])) {
			this->setCurrentTile(i);
			break; 
		}
	}

	//Tile Functions
	if (sf::Mouse::isButtonPressed(this->mouseMappings["BTN_ADD_TILE"])) {	
		this->tileMap->addTile(mouseX, mouseY, this->getCurrentTile());
	}
	else if (sf::Mouse::isButtonPressed(this->mouseMappings["BTN_REMOVE_TILE"])) {
		this->tileMap->removeTile(mouseX, mouseY);
	}
}

void Game::updatePlayer(){
	this->player->update();
}

void Game::playerMove()
{
	this->player->move();
}

void Game::updateCollision(){
	
	int origin_x = player->getPosition().x + player->getGlobalBounds().width / 2;
	int origin_y = player->getPosition().y + player->getGlobalBounds().height / 2;
	//std::cout << "Position: " << player->getPosition().x << " " << player->getPosition().y << "\n";
	int tileSize = static_cast<int>(tileMap->getTileSize());
	int grid_pos_x = origin_x / tileSize;
	int grid_pos_y = origin_y / tileSize;
	std::vector<Tile *> walls;
	int mapWidth = tileMap->getWidth();
	int mapHeight = tileMap->getHeight();
	for (int i = grid_pos_x - 2; i <= grid_pos_x + 2; i++) {
		for (int j = grid_pos_y - 2; j <= grid_pos_y + 2; j++) {
			//std::cout << "map: " << mapWidth << " " << mapHeight << "\n";
			if (i < 0 || j < 0 || i >= mapWidth || j >= mapHeight) continue;
			//std::cout << "wall: " << i << " " << j << "\n";
			Tile* tl = tileMap->getTile(i, j);
			if(tl) walls.push_back(tl);
		}
	}
	sf::FloatRect nextPos;
	sf::FloatRect nextPosX;
	sf::FloatRect nextPosY;
	for (auto& wall : walls) {
		sf::FloatRect playerBounds = player->getGlobalBounds();
		sf::FloatRect wallBounds = wall->getGlobalBounds();
		
		nextPos = nextPosX = nextPosY = playerBounds;
		nextPos.left += player->getVelocity().x;
		nextPos.top += player->getVelocity().y;
		nextPosX.left = nextPos.left;
		nextPosY.top = nextPos.top;

		if (wallBounds.intersects(nextPos)) {
			//std::cout << "Collision";
			//std::cout << wallBounds.top<<" "<<nextPos.top+nextPos.height<<"\n";
			bool intX = wallBounds.intersects(nextPosX);
			bool intY = wallBounds.intersects(nextPosY);
			//std::cout << "bools: " << intX << " " << intY<<"\n";
			bool posvy = player->getVelocity().y > 0;
			sf::FloatRect inter = getIntersection(nextPos, wallBounds);
			//std::cout << "Intersection Rectangle: " << inter.width << " " << inter.height << "\n";
			if (intX == intY) {
				if (inter.width < inter.height) {
					if (player->getVelocity().x > 0) { // Moving right
						player->setPosition(wallBounds.left - player->getGlobalBounds().width, player->getPosition().y);
					}
					else { // Moving left
						player->setPosition(wallBounds.left + wallBounds.width, player->getPosition().y);
					}
					player->resetVelocityX();
				}
				else {
					this->player->setPosition(this->player->getPosition().x, std::round(player->getPosition().y / tileSize) * tileSize);
					this->player->resetVelocityY();
					if (posvy) {
						this->player->setCanJump(true);
					}
				}
			}
			else if (intX) {
				if (player->getVelocity().x > 0) { // Moving right
					player->setPosition(wallBounds.left - player->getGlobalBounds().width, player->getPosition().y);
				}
				else { // Moving left
					player->setPosition(wallBounds.left + wallBounds.width, player->getPosition().y);
				}
				player->resetVelocityX();
			}
			else {
				this->player->setPosition(this->player->getPosition().x, std::round(player->getPosition().y / tileSize) * tileSize);
				this->player->resetVelocityY();
				if (posvy) this->player->setCanJump(true);
			}
		}
	}
}

sf::FloatRect Game::getIntersection(const sf::FloatRect& rect1, const sf::FloatRect& rect2)
{
	float left = std::max(rect1.left, rect2.left);
	float top = std::max(rect1.top, rect2.top);
	float right = std::min(rect1.left + rect1.width, rect2.left + rect2.width);
	float bottom = std::min(rect1.top + rect1.height, rect2.top + rect2.height);

	if (right > left && bottom > top) {
		return sf::FloatRect(left, top, right - left, bottom - top);
	}
	return sf::FloatRect();
}


void Game::updateTileMap(){
	this->tileMap->update();
}

void Game::update(){
	//Polling Window events
	while (this->window.pollEvent(this->ev)) {
		if (this->ev.type == sf::Event::Closed) this->window.close();
		else if (this->ev.type == sf::Event::KeyPressed && this->ev.key.code == sf::Keyboard::Escape) this->window.close();
	}
	
	this->updateInput();

	this->updatePlayer();
	this->updateCollision();
	this->playerMove();

	this->updateTileMap();
}

void Game::renderPlayer(){
	this->player->render(this->window);
}

void Game::renderTileMap(){
	this->tileMap->render(this->window);
}

void Game::render(){
	sf::Color color(122, 134, 151);
	this->window.clear(color);

	this->renderTileMap();
	this->renderPlayer();

	this->window.display();
}

const sf::RenderWindow& Game::getWindow() const{
	return this->window;
}
