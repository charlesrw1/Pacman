#include "Player.h"

void ResolveCollision()
{
	switch (gState.player->cur_dir) {
	case UP:
		gState.player->pos.y = (int)gState.player->pos.y + 0.5;
		break;
	case DOWN:
		gState.player->pos.y = (int)gState.player->pos.y + 0.5;
		break;
	case LEFT:
		gState.player->pos.x = (int)gState.player->pos.x + 0.5;
		break;
	case RIGHT:
		gState.player->pos.x = (int)gState.player->pos.x + 0.5;
		break;
	}
}
void PlayerMovement()
{
	Dir try_dir = NONE;
	sf::Vector2f& p_pos = gState.player->pos;
	Dir& p_dir = gState.player->cur_dir;


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		try_dir = UP;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		try_dir = DOWN;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		try_dir = RIGHT;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		try_dir = LEFT;

	if (try_dir != NONE &&
		!PlayerTileCollision(try_dir, p_pos) &&
		!InTunnel(p_pos)) {
		p_dir = try_dir;
	}

	if (!gState.player->stopped) {
		p_pos += dir_addition[p_dir] * player_speed;
		
		CenterObject(p_dir, p_pos);
	}

	if (PlayerTileCollision(p_dir, p_pos)) {
		ResolveCollision();
		gState.player->stopped = true;
	}
	else {
		gState.player->stopped = false;
	}


	// tunneling
	if (p_pos.x < -1) {
		p_pos.x += 29;
	}
	else if (p_pos.x >= 29) {
		p_pos.x -= 29;
	}
}