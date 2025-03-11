#pragma once

enum PLAYER_ANIMATION_STATE {IDLE = 0, MOVING_LEFT, MOVING_RIGHT, JUMPING, FALLING};

class Player{
private:
	sf::Sprite sprite;
	sf::Texture textureSheet;
	sf::Clock animationTimer;
	sf::Clock hurtTimer;

	//Animation
	short animState;
	sf::IntRect currentFrame;
	bool animationSwitch;
	bool isHurt;

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
	int health;
	int maxHealth;

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
	inline int getHealth() { return this->health; };
	inline const int getMaxHealth() { return this->maxHealth; };
	inline void setHealth(int h) { this->health = h; };

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
	void takeDamage(int amount);
	void move();
	void updateMovement();
	void updateAnimation();
	void update();
	void render(sf::RenderTarget& target);
	void kill();
	void reset();
};
