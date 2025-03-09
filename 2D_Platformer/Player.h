#pragma once

enum PLAYER_ANIMATION_STATE {IDLE = 0, MOVING_LEFT, MOVING_RIGHT, JUMPING, FALLING, HURT};

class Player{
private:
	sf::Sprite sprite;
	sf::Texture textureSheet;
	sf::Clock animationTimer;

	//Animation
	short animState;
	sf::IntRect currentFrame;
	bool animationSwitch;

	//Physics
	sf::Vector2f velocity;
	float velocityMax;
	float velocityMin;
	float acceleration;
	float drag;
	float gravity;
	float jumpForce;
	float jumpGravityMod;
	float fallGravityMod;
	float velocityMaxY;
	bool canJump;
	bool isJumping;

	void initVars();
	void initTexture();
	void initSprite();
	void initAnimations();
	void initPhysics();

	

public:
	Player();
	virtual ~Player();

	//Accessors
	const bool& getAnimationSwitch();
	const sf::Vector2f getPosition() const;
	const sf::Vector2f getVelocity();
	const sf::FloatRect getGlobalBounds() const;

	inline const bool& getCanJump() const { return this->canJump; }
	inline void setCanJump(const bool canJump) { this->canJump = canJump; }

	//Modifiers
	void setPosition(const float x, const float y);
	void resetVelocityX();
	void resetVelocityY();

	//Functions
	void resetAnimationTimer();
	void move(const float dir_x, const float dir_y);
	void jump();
	void shortjump();
	void updatePhysics();
	void move();
	void updateMovement();
	void updateAnimation();
	void update();
	void render(sf::RenderTarget& target);
};

