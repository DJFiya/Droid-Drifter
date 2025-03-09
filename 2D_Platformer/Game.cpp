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

void Game::updateCollision(){
	if (this->player->getPosition().y + this->player->getGlobalBounds().height > this->window.getSize().y) {
		this->player->setCanJump(true);
		this->player->resetVelocityY();
		this->player->setPosition(this->player->getPosition().x, this->window.getSize().y - this->player->getGlobalBounds().height);
	}
	std::cout << player->getPosition().x << " " << player->getPosition().y <<"\n";
	
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

	this->updateTileMap();
}

void Game::renderPlayer(){
	this->player->render(this->window);
}

void Game::renderTileMap(){
	this->tileMap->render(this->window);
}

void Game::render(){
	this->window.clear();

	this->renderTileMap();
	this->renderPlayer();

	this->window.display();
}

const sf::RenderWindow& Game::getWindow() const{
	return this->window;
}
