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

void Game::initFont(){
	if (!this->tileSheet.loadFromFile("Fonts/EmblemaOne-Regular.ttf")) {
		std::cout << "ERROR: Font failed to load!\n";
	}
}

void Game::initPlayer(){
	this->player = new Player();
}

void Game::initTileMap(){
	this->tileMap = new TileMap(100, 10, &this->tileSheet, 64);
}

void Game::initCamera(){
	this->camera = new Camera(this->window);
}

void Game::initAI(){
	this->ai = new AIPlayer(this->player, this->tileMap);
}


Game::Game(){
	this->initWindow();
	this->initInput();
	this->initTileSheet();
	this->initTileMap();
	this->initPlayer();
	this->initCamera();
	this->initAI();
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
	sf::Vector2i mousePositionWindow = sf::Mouse::getPosition(this->getWindow());
	sf::Vector2f mousePositionWorld = this->getWindow().mapPixelToCoords(mousePositionWindow, this->camera->getView());	
	const int mouseX = int(mousePositionWorld.x) / int(this->tileMap->getTileSize());
	const int mouseY = int(mousePositionWorld.y) / int(this->tileMap->getTileSize());

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

void Game::updateAI(){
	this->ai->update();
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

void Game::updatePlayerDamage(){
	//std::cout << this->player->getHealth() << "\n";
	sf::Vector2f position = player->getPosition();
	//std::cout << position.x << " " << position.y;
	if (position.x < -100 || position.x > tileMap->getWidth() * tileMap->getTileSize() +100 || position.y < -100 || position.y > tileMap->getHeight()*tileMap->getTileSize() + 100) {
		this->player->kill();
	}
	position.x += player->getGlobalBounds().width/2;
	position.y += int(player->getGlobalBounds().height*1.2);
	int tileX = int(position.x) / int(this->tileMap->getTileSize());
	int tileY = int(position.y) / int(this->tileMap->getTileSize());
	if (this->tileMap->isDamagingTile(tileX, tileY)) {
		this->player->takeDamage(1);
	}
}
bool Game::playerWin() {
	//std::cout << this->player->getHealth() << "\n";
	sf::Vector2f position = player->getPosition();
	//std::cout << position.x << " " << position.y;
	position.x += player->getGlobalBounds().width / 2;
	position.y += int(player->getGlobalBounds().height * 1.2);
	int tileX = int(position.x) / int(this->tileMap->getTileSize());
	int tileY = int(position.y) / int(this->tileMap->getTileSize());
	return (this->tileMap->isWinningTile(tileX, tileY));
}

bool Game::playerLost() {
	return player->getHealth() <= 0;
}


void Game::updateTileMap(){
	this->tileMap->update();
}

void Game::updateCamera(){
	sf::Vector2f playerPosition = this->player->getPosition();

	sf::Vector2f mapSize(this->tileMap->getWidth() * this->tileMap->getTileSize(),
		this->tileMap->getHeight() * this->tileMap->getTileSize());

	sf::Vector2f viewSize = this->camera->getView().getSize();

	float halfWidth = viewSize.x / 2.f;
	float halfHeight = viewSize.y / 2.f;

	sf::Vector2f targetPosition = playerPosition;

	float clampedX = this->clamp(targetPosition.x, halfWidth, mapSize.x - halfWidth);
	float clampedY = this->clamp(targetPosition.y, halfHeight, mapSize.y - halfHeight);

	this->camera->update(sf::Vector2f(clampedX, clampedY));
}

float Game::clamp(float value, float min, float max){
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

void Game::update() {
	// Polling Window events
	while (this->window.pollEvent(this->ev)) {
		if (this->ev.type == sf::Event::Closed) this->window.close();
		else if (this->ev.type == sf::Event::KeyPressed && this->ev.key.code == sf::Keyboard::Escape) this->window.close();
	}

	if (this->mode == GameMode::PLAYER_MODE) {
		// Use normal player input
		this->updateInput();
		this->updatePlayer();
		this->updateCollision();
		this->playerMove();
		this->updatePlayerDamage();
		this->updateCamera();
		this->updateTileMap();
	}
	else if (this->mode == GameMode::AI_MODE) {
		// Use AI player input
		this->updateAI();
		this->updatePlayer();
		this->updateCollision();
		this->playerMove();
		this->updatePlayerDamage();
		this->updateCamera();
		this->updateTileMap();
	}
	else if (this->mode == GameMode::TRAINING_MODE) {
		// Training mode logic - run a single step of training
		this->updateTrainingStep();
	}
}

void Game::updateTrainingStep() {
	try {
		// Your updateTrainingStep code
		static int episodeCount = 0;
		static int stepCount = 0;
		static float episodeReward = 0.0f;
		static float totalReward = 0.0f;
		static std::vector<float> episodeRewards;
		static const int MAX_STEPS_PER_EPISODE = 2000;
		static const int NUM_EPISODES = 100;
		static bool initialized = false;

		// Initialize training if needed
		if (!initialized) {
			std::cout << "Starting training for " << NUM_EPISODES << " episodes..." << std::endl;
			this->player->reset();
			episodeCount = 0;
			totalReward = 0.0f;
			episodeRewards.clear();
			initialized = true;
		}

		// Check if training is complete
		if (episodeCount >= NUM_EPISODES) {
			if (episodeCount == NUM_EPISODES) {
				// Calculate and display final statistics
				float avgReward = totalReward / NUM_EPISODES;
				std::cout << "Training complete. Average reward: " << avgReward << std::endl;

				// Save final model
				this->ai->saveModel("model_final.pt");
				std::cout << "Final model saved to model_final.pt" << std::endl;

				// Reset to AI mode or player mode after training
				this->mode = GameMode::AI_MODE;
				episodeCount++; // Increment to avoid repeating this code
			}
			return;
		}

		// Check if we need to start a new episode
		if (stepCount == 0) {
			this->player->reset();
			episodeReward = 0.0f;
		}

		// Update AI and game state for one step
		this->updateAI();
		this->updatePlayer();
		this->updateCollision();
		this->playerMove();
		this->updatePlayerDamage();
		this->updateCamera();
		this->updateTileMap();

		// Collect reward from this step
		float stepReward = this->ai->getLastReward();
		episodeReward += stepReward;

		// Increment step counter
		stepCount++;

		// Check if episode is complete
		bool episodeComplete = (player->getPosition().x >= 5336 || this->player->getHealth() <= 0 || stepCount >= MAX_STEPS_PER_EPISODE);

		if (episodeComplete) {
			// Record episode stats
			episodeRewards.push_back(episodeReward);
			totalReward += episodeReward;

			// Log progress periodically
			if ((episodeCount + 1) % 10 == 0) {
				std::cout << "Episode " << (episodeCount + 1) << "/" << NUM_EPISODES
					<< " completed. Reward: " << episodeReward
					<< " Steps: " << stepCount << std::endl;
			}

			// Save model periodically
			if ((episodeCount + 1) % 100 == 0) {
				std::string filename = "model_episode_" + std::to_string(episodeCount + 1) + ".pt";
				this->ai->saveModel(filename);
				std::cout << "Model saved to " << filename << std::endl;
			}

			// Reset for next episode
			episodeCount++;
			stepCount = 0;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in updateTrainingStep: " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown exception in updateTrainingStep" << std::endl;
	}
	
}

void Game::renderPlayer(){
	this->player->render(this->window);
}

void Game::renderTileMap(){
	this->tileMap->render(this->window);
}

void Game::renderHP(){
	float currentHealth = this->player->getHealth();
	float maxHealth = this->player->getMaxHealth();

	float barX = 20.f;
	float barY = 20.f;

	float barWidth = 200.f;  
	float barHeight = 20.f;  

	float currentWidth = (currentHealth / maxHealth) * barWidth;
	float borderWidth = 4.f;

	sf::RectangleShape barBackground(sf::Vector2f(barWidth + borderWidth * 2, barHeight + borderWidth * 2));
	barBackground.setPosition(barX-borderWidth, barY-borderWidth);
	barBackground.setFillColor(sf::Color(50, 50, 50));  

	sf::RectangleShape barForeground(sf::Vector2f(currentWidth, barHeight));
	barForeground.setPosition(barX, barY);

	if (currentHealth > maxHealth * 0.5f) {
		barForeground.setFillColor(sf::Color::Green);  
	}
	else if (currentHealth > maxHealth * 0.2f) {
		barForeground.setFillColor(sf::Color::Yellow); 
	}
	else {
		barForeground.setFillColor(sf::Color::Red);  
	}
	sf::View originalView = this->window.getView();

	this->window.setView(this->window.getDefaultView());

	this->window.draw(barBackground);

	this->window.draw(barForeground);

	this->window.setView(originalView);
}
//Function for real player, not for ai
void Game::renderGameOver(){
	/*this->window.clear();
	sf::View originalView = this->window.getView();
	this->window.setView(this->window.getDefaultView());
	sf::Text text;
	text.setFont(font);
	text.setString("Game Over");
	text.setCharacterSize(30);
	text.setFillColor(sf::Color::White);
	text.setPosition(480, 320);
	this->window.draw(text);
	this->window.setView(originalView);*/
}

void Game::render(){
	sf::Color color(122, 134, 151);
	this->window.clear(color);
	/*if (this->player->getHealth() > 0) {*/
	this->camera->apply(window);
	this->renderTileMap();
	this->renderPlayer();
	this->renderHP();
	/*}
	else {
		this->renderGameOver();
	}*/

	this->window.display();
}

const sf::RenderWindow& Game::getWindow() const{
	return this->window;
}
