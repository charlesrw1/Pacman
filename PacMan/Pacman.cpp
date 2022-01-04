#include "Pacman.h"

bool TileCollision(sf::Vector2f pos, bool home_tiles)
{
	bool collided = false;
	char tile = '|';

	tile = GetTile(pos.x, pos.y);

	if (tile == '|')
		collided = true;
	if (!home_tiles && tile == '-')
		collided = true;
	return collided;
}
// REEEEEEEEE STOP BREAKING
bool PlayerTileCollision(Dir dir, sf::Vector2f pos)
{
	bool collided = false;
	char tile = '|';
	switch (dir)
	{
	case UP:
		tile = GetTile(pos.x, pos.y - 0.51);
		break;
	case DOWN:
		tile = GetTile(pos.x, pos.y + 0.51);
		break;
	case LEFT:
		tile = GetTile(pos.x - 0.51, pos.y);
		break;
	case RIGHT:
		tile = GetTile(pos.x + 0.51, pos.y);
		break;
	}
	if (tile == '|' || tile == '-')
		collided = true;
	return collided;
}
void CenterObject(Dir dir, sf::Vector2f& pos)
{
	switch (dir)
	{
	case UP:
	case DOWN:
		pos.x = (int)pos.x + 0.5;
		break;
	case LEFT:
	case RIGHT:
		pos.y = (int)pos.y + 0.5;
		break;
	}
}