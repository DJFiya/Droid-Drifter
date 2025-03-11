#include "stdafx.h"
#include "Tile.h"

Tile::Tile(const unsigned& grid_pos_x, const unsigned& grid_pos_y, const unsigned& tile_size, sf::Texture* texture_sheet, int tileType, bool damaging) : damaging(damaging), type(tileType){
	this->sprite.setTexture(*texture_sheet);
	int size = 8;
	int origin_x = size * (tileType % 2);
	int origin_y = size * (tileType / 2);
	this->sprite.setTextureRect(sf::IntRect(origin_x, origin_y, size, size));
	float scaleFactor = tile_size / 8.0f;
	this->sprite.setScale(scaleFactor, scaleFactor);
	this->sprite.setPosition(float(grid_pos_x * tile_size), float(grid_pos_y * tile_size));
}

const sf::FloatRect Tile::getGlobalBounds() const{
	return this->sprite.getGlobalBounds();
}

void Tile::update(){

}

void Tile::render(sf::RenderTarget& target){
	target.draw(this->sprite);
}
