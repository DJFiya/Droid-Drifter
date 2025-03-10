#pragma once
class Camera{
private:
	sf::View view;
	float smoothingFactor;
	float maxOffsetX;
	float maxOffsetY;
public:
	const sf::View getView();

	Camera(sf::RenderWindow& window);
	~Camera();
	void initVars();

	void update(const sf::Vector2f& playerPosition);
	void apply(sf::RenderWindow& window);
};

