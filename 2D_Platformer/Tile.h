#pragma once
class Tile
{
private:
	sf::Sprite sprite;
	const bool damaging;
public:
	Tile(const unsigned& grid_pos_x, const unsigned& grid_pos_y, const unsigned& tile_size, sf::Texture* texture_sheet, int tileType, bool damaging = false);

	const sf::FloatRect getGlobalBounds() const;

	inline bool getDamaging() { return damaging; };

	void update();
	void render(sf::RenderTarget& target);
};

