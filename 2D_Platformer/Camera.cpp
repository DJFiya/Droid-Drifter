#include "stdafx.h"
#include "Camera.h"

const sf::View Camera::getView()
{
	return this->view;
}

Camera::Camera(sf::RenderWindow& window){
	view.setSize(window.getSize().x, window.getSize().y);
	view.setCenter(window.getSize().x / 2, window.getSize().y / 2);
	initVars();
}

Camera::~Camera(){}

void Camera::initVars(){
	smoothingFactor = 0.1f;    
	maxOffsetX = 100.0f;     
	maxOffsetY = 50.0f;
}

void Camera::update(const sf::Vector2f& position){
    sf::Vector2f currentPosition = view.getCenter();

    sf::Vector2f targetPosition = position;

    if (targetPosition.x - currentPosition.x > maxOffsetX) {
        targetPosition.x = currentPosition.x + maxOffsetX;
    }
    else if (currentPosition.x - targetPosition.x > maxOffsetX) {
        targetPosition.x = currentPosition.x - maxOffsetX;
    }

    if (targetPosition.y - currentPosition.y > maxOffsetY) {
        targetPosition.y = currentPosition.y + maxOffsetY;
    }
    else if (currentPosition.y - targetPosition.y > maxOffsetY) {
        targetPosition.y = currentPosition.y - maxOffsetY;
    }

    sf::Vector2f newPosition = currentPosition + (targetPosition - currentPosition) * smoothingFactor;
    view.setCenter(newPosition);
}

void Camera::apply(sf::RenderWindow& window) {
    window.setView(view);
}