#include "Player.h"

// these are pretty crappy, but they work enough so the
// player doesnt always snap to the sides when turning
Dir GetCorrection(Dir pdir, sf::Vector2f ppos)
{
	switch (pdir)
	{
	case UP:
	case DOWN:
		if (ppos.x - (int)ppos.x >= 0.5)
			return LEFT;
		return RIGHT;
		break;
	case LEFT:
	case RIGHT:
		if (ppos.y - (int)ppos.y >= 0.5)
			return UP;
		return DOWN;
		break;
	}
}
void Cornering()
{
	gState.player->pos += dir_addition[gState.player->correction] * player_speed;
	bool done = false;
	switch (gState.player->correction)
	{
	case UP:
		done = (gState.player->pos.y - (int)gState.player->pos.y <= 0.5);
		break;
	case DOWN:
		done = (gState.player->pos.y - (int)gState.player->pos.y >= 0.5);
		break;
	case LEFT:
		done = (gState.player->pos.x - (int)gState.player->pos.x <= 0.5);
		break;
	case RIGHT:
		done = (gState.player->pos.x - (int)gState.player->pos.x >= 0.5);
		break;
	}
	if (done) {
		CenterObject(gState.player->cur_dir, gState.player->pos);
		gState.player->cornering = false;
	}
}
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

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		try_dir = UP;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		try_dir = DOWN;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		try_dir = RIGHT;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		try_dir = LEFT;

	if (try_dir != NONE &&
		!PlayerTileCollision(try_dir, gState.player->pos) &&
		!InTunnel(gState.player->pos)) {
		gState.player->cur_dir = try_dir;
		// dont need to corner if its opposite direction
		if (gState.player->cur_dir != opposite_dir[gState.player->cur_dir]) {
			gState.player->cornering = true;
			gState.player->correction = GetCorrection(try_dir, gState.player->pos);
		}
	}

	if (!gState.player->stopped) {
		gState.player->pos += dir_addition[gState.player->cur_dir] * player_speed;
	}

	if (gState.player->cornering) {
		Cornering();
	}

	if (PlayerTileCollision(gState.player->cur_dir, gState.player->pos)) {
		ResolveCollision();
		gState.player->stopped = true;
	}
	else {
		gState.player->stopped = false;
	}


	// tunneling
	if (gState.player->pos.x < -1) {
		gState.player->pos.x += 29;
	}
	else if (gState.player->pos.x >= 29) {
		gState.player->pos.x -= 29;
	}
}