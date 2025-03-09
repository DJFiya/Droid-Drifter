#include "stdafx.h"
#include "Player.h"
//#include <direct.h>

void Player::initTexture(){
	//Code for finding directories search path

	/*char buffer[FILENAME_MAX];
	_getcwd(buffer, FILENAME_MAX);
	std::string currentDirectory = std::string(buffer) + "\\Textures\\Robot_Player.png";
	std::cout << "Full path to the texture: " << currentDirectory << std::endl;*/


	if (!this->textureSheet.loadFromFile("Textures/Robot_Player.png")) {
		std::cout << "ERROR: Player Sheet Texture failed to load!\n";
	}
}

void Player::initVars() {
	this->animState = IDLE;
}

void Player::initSprite(){
	this->sprite.setTexture(this->textureSheet);
	this->currentFrame = sf::IntRect(0, 32, 32, 32);
	this->sprite.setTextureRect(currentFrame);
	this->sprite.setScale(2.f, 2.f);
}

void Player::initAnimations(){
	this->animationTimer.restart();
	this->animationSwitch = true;
}

void Player::initPhysics() {
	this->velocityMax = 7.f;
	this->velocityMin = 1.f;
	this->acceleration = 2.f;
	this->drag = 0.80f;
	this->gravity = 1.f;         // Minimum gravity to ensure falling
	this->jumpForce = 15.f;      // Initial jump force (smaller than before)
	this->jumpGravityMod = 0.5f; // Reduced gravity while ascending
	this->fallGravityMod = 0.8f; // Increased gravity while falling
	this->velocityMaxY = 100.f;
	this->canJump = false;
	this->isJumping = false;
}

Player::Player(){
	this->initVars();
	this->initTexture();
	this->initSprite();
	this->initAnimations();
	this->initPhysics();
}

Player::~Player(){
}

const bool& Player::getAnimationSwitch(){
	bool anim_switch = this->animationSwitch;
	if (this->animationSwitch) animationSwitch = false;
	return anim_switch;
}

const sf::Vector2f Player::getPosition() const{
	return this->sprite.getPosition();
}

const sf::Vector2f Player::getVelocity()
{
	return this->velocity;
}

const sf::FloatRect Player::getGlobalBounds() const
{
	return this->sprite.getGlobalBounds();
}

void Player::setPosition(const float x, const float y){
	this->sprite.setPosition(x, y);
}

void Player::resetVelocityX()
{
	this->velocity.x = 0.f;
}

void Player::resetVelocityY(){
	this->velocity.y = 0.f;
}

void Player::resetAnimationTimer(){
	this->animationTimer.restart();
	this->animationSwitch = true;
}

void Player::move(const float dir_x, const float dir_y){
	this->velocity.x += dir_x * this->acceleration;
	//this->velocity.y += dir_y * this->acceleration;

	if (std::abs(this->velocity.x) > this->velocityMax) {
		this->velocity.x = this->velocityMax * ((this->velocity.x < 0.f) ? -1.f : 1.f);
	}
}

void Player::jump()
{
	this->velocity.y = -this->jumpForce;
	this->canJump = false;
	this->isJumping = true;
}

void Player::updatePhysics(){
	float currentGravity = this->gravity;

	if (this->velocity.y < 0.f) {
		currentGravity *= this->jumpGravityMod;
	}
	else if (this->velocity.y > 0.f) {
		currentGravity *= this->fallGravityMod;
		this->isJumping = false;
	}
	this->velocity.y += currentGravity;
	if (std::abs(this->velocity.y) > this->velocityMaxY) {
		this->velocity.y = this->velocityMaxY * ((this->velocity.y < 0.f) ? -1.f : 1.f);
	}

	this->velocity.x *= this->drag;

	if (std::abs(this->velocity.x) < this->velocityMin) this->velocity.x = 0.f;
	if (std::abs(this->velocity.y) < this->velocityMin) this->velocity.y = 0.f;

	if (std::abs(this->velocity.x) <= 0.5f) this->velocity.x = 0.f;

	this->sprite.move(this->velocity);
}

void Player::updateMovement() {
	this->animState = IDLE;
	if (this->velocity.x < 0) {
		this->animState = MOVING_LEFT;
	}
	else if (this->velocity.x > 0) {
		this->animState = MOVING_RIGHT;
	}

	if (this->velocity.y < 0) {
		this->animState = JUMPING;
	}
	else if (this->velocity.y > 0) {
		this->animState = FALLING;
	}

}

void Player::updateAnimation(){
	if (this->animState == IDLE) {
		if (this->animationTimer.getElapsedTime().asSeconds() >= 0.2f || this->getAnimationSwitch()) {
			this->currentFrame.top = 32.f;
			this->currentFrame.left += 32.f;
			if (this->currentFrame.left >= 160.f) this->currentFrame.left = 0;

			this->sprite.setTextureRect(this->currentFrame);
			this->animationTimer.restart();
		}
	}
	else if (this->animState == MOVING_RIGHT) {
		if (this->animationTimer.getElapsedTime().asSeconds() >= 0.2f) {
			this->currentFrame.top = 0.f;
			this->currentFrame.left += 32.f;
			if (this->currentFrame.left >= 160.f) this->currentFrame.left = 0;

			this->sprite.setTextureRect(this->currentFrame);
			this->animationTimer.restart();
		}
		this->sprite.setScale(2.f, 2.f);
		this->sprite.setOrigin(0.f, 0.f);
	}
	else if (this->animState == MOVING_LEFT) {
		if (this->animationTimer.getElapsedTime().asSeconds() >= 0.2f) {
			this->currentFrame.top = 0.f;
			this->currentFrame.left += 32.f;
			if (this->currentFrame.left >= 160.f) this->currentFrame.left = 0;

			this->sprite.setTextureRect(this->currentFrame);
			this->animationTimer.restart();
		}
		this->sprite.setScale(-2.f, 2.f);
		this->sprite.setOrigin(this->sprite.getGlobalBounds().width / 2.f, 0.f);
	}
	else if (this->animState == JUMPING) {
		if (this->animationTimer.getElapsedTime().asSeconds() >= 0.2f) {
			this->currentFrame.top = 64.f;
			this->currentFrame.left += 32.f;
			if (this->currentFrame.left >= 64.f) this->currentFrame.left = 0;

			this->sprite.setTextureRect(this->currentFrame);
			this->animationTimer.restart();
		}
	}
	else if (this->animState == FALLING) {
		if (this->animationTimer.getElapsedTime().asSeconds() >= 0.2f) {
			this->currentFrame.top = 64.f;
			this->currentFrame.left += 32.f;
			if (this->currentFrame.left >= 160.f) this->currentFrame.left =96;

			this->sprite.setTextureRect(this->currentFrame);
			this->animationTimer.restart();
		}
	}
	else {
		this->animationTimer.restart();
	}
}

void Player::update(){
	this->updateMovement();
	this->updateAnimation();
	this->updatePhysics();
}

void Player::render(sf::RenderTarget& target){
	target.draw(this->sprite);
}
