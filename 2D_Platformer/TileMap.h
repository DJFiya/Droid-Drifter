#pragma once
#include "Tile.h"
class TileMap{
private:
	std::vector<std::vector<Tile*>> tiles;
	sf::Texture* tileSheet;
	unsigned tileSize;
	unsigned width;
	unsigned height;
public:
	TileMap();
	TileMap(unsigned width, unsigned height, sf::Texture* tile_sheet, unsigned tile_size);
	~TileMap();

	inline const unsigned& getTileSize() const { return this->tileSize; };
	inline const unsigned& getWidth() const { return this->width; };
	inline const unsigned& getHeight() const { return this->height; };
	inline Tile* getTile(int tileX, int tileY) { return tiles[tileX][tileY]; };

	//Functions
	bool isDamagingTile(unsigned x, unsigned y);
	bool isWinningTile(unsigned x, unsigned y);
	void addTile(unsigned x, unsigned y, int tileType);
	void removeTile(unsigned x, unsigned y);

	void update();
	void render(sf::RenderTarget& target);
};

