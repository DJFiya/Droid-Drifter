#include "stdafx.h"
#include "TileMap.h"

TileMap::TileMap(){
	this->tileSheet = NULL;
	this->tileSize = 0;
}

TileMap::TileMap(unsigned width, unsigned height, sf::Texture* tile_sheet, unsigned tile_size): width(width), height(height) {
	this->tileSheet = tile_sheet;
	this->tileSize = tile_size;
	this->tiles.resize(width);
	for (int i = 0; i < this->tiles.size(); i++) {
		this->tiles[i].resize(height, nullptr);
	}
	std::ifstream file("Maps/map.txt");
	if (!file) {
		std::cout << "map.txt could not be opened";
		for (int i = 0; i < this->tiles.size(); i++) {
			addTile(i, 0, 3);
		}
		for (int i = 0; i < this->tiles.size(); i++) {
			addTile(i, height - 1, 3);
		}
		for (int i = 0; i < this->tiles[0].size(); i++) {
			addTile(0, i, 3);
		}
		for (int i = 0; i < this->tiles[0].size(); i++) {
			addTile(width - 1, i, 3);
		}
	}
	else {
		//std::cout << "Map file opened successfully\n";
		std::string line;
		int lineNum = 0;
		while (std::getline(file, line) && lineNum < height) {
			//std::cout << "Reading line " << lineNum << ": " << line << std::endl;
			int index = 0;
			for (char c : line) {
				if (index < width && c >= '0' && c <= '7') {
					int tileType = c - '0';
					//std::cout << "Adding tile type " << tileType << " at position (" << index << ", " << lineNum << ")" << std::endl;
					addTile(index, lineNum, tileType);
				}
				index += 1;
			}
			lineNum += 1;
		}

		if (lineNum < height) {
			//std::cout << "Map file was shorter than expected height. Adding default boundaries." << std::endl;
			for (int i = 0; i < width; i++) {
				addTile(i, 0, 3);
				addTile(i, height - 1, 3);
			}
			for (int i = 0; i < height; i++) {
				addTile(0, i, 3);
				addTile(width - 1, i, 3);
			}
		}
	}
}

TileMap::~TileMap(){
	for (int i = 0; i < this->tiles.size(); i++) {
		for (int j = 0; j < this->tiles[i].size(); j++) {
			delete this->tiles[i][j];
			this->tiles[i][j] = nullptr;
		}
	}
}

bool TileMap::isDamagingTile(unsigned x, unsigned y){
	if (x < this->tiles.size() && x >= 0) {
		if (y < this->tiles[x].size() && y >= 0) {
			if (this->tiles[x][y] != nullptr) return this->tiles[x][y]->getDamaging();
		}
	}
	return false;
}

bool TileMap::isWinningTile(unsigned x, unsigned y)
{
	if (x < this->tiles.size() && x >= 0) {
		if (y < this->tiles[x].size() && y >= 0) {
			if (this->tiles[x][y] != nullptr) return this->tiles[x][y]->getType()==6;
		}
	}
	return false;
}


void TileMap::addTile(unsigned x, unsigned y, int tileType){
	if (x < this->tiles.size() && x >= 0) {
		if (y < this->tiles[x].size() && y >= 0) {
			if (this->tiles[x][y] == nullptr && 0<=tileType && tileType<=7) {
				this->tiles[x][y] = new Tile(x, y, this->tileSize, this->tileSheet, tileType, tileType==6);
			}
		}
	}
}

void TileMap::removeTile(unsigned x, unsigned y){
	if (x < this->tiles.size()) {
		if (y < this->tiles[x].size()) {
			if (this->tiles[x][y] != nullptr) {
				delete this->tiles[x][y];
				this->tiles[x][y] = nullptr;
			}
		}
	}
}

void TileMap::update(){
	for (int i = 0; i < this->tiles.size(); i++) {
		for (int j = 0; j < this->tiles[i].size(); j++) {
			if (this->tiles[i][j] != nullptr)this->tiles[i][j]->update();
		}
	}
}

void TileMap::render(sf::RenderTarget& target){
	for (int i = 0; i < this->tiles.size(); i++) {
		for (int j = 0; j < this->tiles[i].size(); j++) {
			if(this->tiles[i][j]!=nullptr)this->tiles[i][j]->render(target);
		}
	}
}

