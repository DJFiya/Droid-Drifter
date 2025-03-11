#pragma once
class Tile
{
private:
	sf::Sprite sprite;
	const bool damaging;
	int type;
public:
	Tile(const unsigned& grid_pos_x, const unsigned& grid_pos_y, const unsigned& tile_size, sf::Texture* texture_sheet, int tileType, bool damaging = false);

	const sf::FloatRect getGlobalBounds() const;

	inline const bool getDamaging() const { return damaging; };
	inline const int getType() const { return type; };
	inline const bool isWin() const { return type == 7; };

	void update();
	void render(sf::RenderTarget& target);
};

