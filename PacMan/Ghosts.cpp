#include "Ghosts.h"

bool InMiddleTile(sf::Vector2f pos, sf::Vector2f prev, Dir dir)
{
	if ((int)pos.x != (int)prev.x || (int)pos.y != (int)prev.y)
		return false;
	float x = pos.x - (int)pos.x;
	float y = pos.y - (int)pos.y;

	float px = prev.x - (int)prev.x;
	float py = prev.y - (int)prev.y;

	switch (dir)
	{
	case UP:
	case DOWN:
		return std::min(y, py) <= 0.5 && std::max(y, py) >= 0.5;
		break;

	case LEFT:
	case RIGHT:
		return std::min(x, px) <= 0.5 && std::max(x, px) >= 0.5;
		break;
	}
	return false;
}
// this should be cached/hardcoded, but its performant enough
std::vector<Dir> GetAvailableSquares(sf::Vector2f pos, Dir dir, bool home_tile)
{
	std::vector<Dir> squares;
	// this is the reverse order of precedence that
	// the original pacman game used
	// up is picked before left if the distances are tied etc.
	if (!TileCollision(pos + sf::Vector2f{ 0.9, 0 },home_tile ))
		squares.push_back(RIGHT);
	if (!TileCollision(pos + sf::Vector2f{ 0, 0.9 }, home_tile))
		squares.push_back(DOWN);
	if (!TileCollision(pos + sf::Vector2f{ -0.9, 0 }, home_tile))
		squares.push_back(LEFT);
	if (!TileCollision(pos + sf::Vector2f{ 0, -0.9 }, home_tile))
		squares.push_back(UP);
	for (int i = 0; i < squares.size(); i++) {
		if (squares[i] == opposite_dir[dir])
			squares.erase(squares.begin() + i);
	}
	return squares;
}
// returns squared distance
float Distance(int x, int y, int x1, int y1)
{
	return ((x - x1) * (x - x1)) + ((y - y1) * (y - y1));
}

// the original game didnt use a pathfinding algorithm
// simple distance comparisons are fine enough
Dir GetShortestDir(const std::vector<Dir>& squares, const Ghost& ghost, sf::Vector2f target)
{
	int min_dist = 20000000;
	Dir min_dir = NONE;
	if (squares.size() == 0)
		printf("EMPTY\n");

	for (auto dir : squares) {
		sf::Vector2f square = ghost.pos + dir_addition[dir];
		float dir_dist = Distance(target.x, target.y, square.x, square.y);
		if (dir_dist <= min_dist) {
			min_dir = dir;
			min_dist = dir_dist;
		}
	}

	return min_dir;
}
sf::Vector2f BlinkyUpdate(Ghost& ghost)
{
	return gState.player->pos;
}
sf::Vector2f PinkyUpdate(Ghost& ghost)
{
	return gState.player->pos + dir_addition[gState.player->cur_dir] * 4;
}
sf::Vector2f InkyUpdate(Ghost& ghost)
{
	sf::Vector2f target = gState.player->pos + dir_addition[gState.player->cur_dir] * 2;
	sf::Vector2f offset = gState.ghosts[RED]->pos - target;
	target = target + offset * -1;
	return target;
}
sf::Vector2f ClydeUpdate(Ghost& ghost)
{
	sf::Vector2f target = gState.player->pos;

	// if clyde is 8 tiles away, target player, else target corner
	if (Distance(ghost.pos.x, ghost.pos.y, gState.player->pos.x, gState.player->pos.y) < 64)
		target = { 0,31 };
	return target;
}
bool PassedEntrence(Ghost& ghost)
{
	float prev_x = ghost.pos.x - dir_addition[ghost.cur_dir].x * ghost.move_speed;

	return FComp(ghost.pos.y, 11.5)
		&& std::min(ghost.pos.x, prev_x) <= 14.1
		&& std::max(ghost.pos.x, prev_x) >= 13.9;
}
void UpdateDirection(std::vector<Dir> squares, Ghost& ghost)
{
	sf::Vector2f target;
	bool update_dir = false;
	if (squares.size() == 1) {
		ghost.cur_dir = squares.at(0);
	}

	switch (ghost.target_state)
	{
	case FRIGHTENED:
		ghost.cur_dir = squares.at(rand() % (squares.size()));
		break;
	case GOHOME:
		target = { 14,11.5 };
		update_dir = true;
		break;
	case CHASE:
		switch (ghost.type)
		{
		case RED:
			target = BlinkyUpdate(ghost);
			break;
		case PINK:
			target = PinkyUpdate(ghost);
			break;
		case BLUE:
			target = InkyUpdate(ghost);
			break;
		case ORANGE:
			target = ClydeUpdate(ghost);
			break;
		}
		update_dir = true;
		break;
	case CORNER:
		target = corners[ghost.type];
		update_dir = true;
		break;
	}

	if (update_dir)
		ghost.cur_dir = GetShortestDir(squares, ghost, target);

	CenterObject(ghost.cur_dir, ghost.pos);
}
// held together with duct tape
// my waypoint system works on the assumption that movement is only
// made in the center of tiles, in the house, movement is made off center,
// which breaks it
void HouseUpdate(Ghost& ghost)
{
	switch (ghost.target_state)
	{
	case ENTERHOME:
		if (ghost.pos.y >= 15) {
			ghost.cur_dir = enter_dir[ghost.type];
			ghost.pos.y = 15;
			if (FComp(ghost.pos.x, starting_x[ghost.type])
				|| (ghost.cur_dir == LEFT && ghost.pos.x <= starting_x[ghost.type])
				|| (ghost.cur_dir == RIGHT && ghost.pos.x >= starting_x[ghost.type])) {
				ghost.target_state = HOMEBASE;
				ghost.move_speed = inhome_speed;
			}
		}
		break;
	case LEAVEHOME:
		if (FComp(ghost.pos.x, 14) || (ghost.cur_dir == LEFT && ghost.pos.x <= 14) || (ghost.cur_dir == RIGHT && ghost.pos.x >= 14)) {
			ghost.cur_dir = UP;
			ghost.pos.x = 14;
			if (ghost.pos.y <= 11.5) {
				ghost.move_speed = ghost_speed;
				ghost.target_state = GetGlobalTarget();
				ghost.pos.y = 11.5;
				ghost.in_house = false;
				ghost.cur_dir = LEFT;
			}
		}
		else {
			ghost.cur_dir = opposite_dir[enter_dir[ghost.type]];
		}
		break;
	case HOMEBASE:
		ghost.move_speed = inhome_speed;

		// check timer if its time to leave
		if (gState.using_global_counter) {
			if (gState.global_dot_counter >= global_dot_limit[ghost.type])
				ghost.target_state = LEAVEHOME;
		}
		else if (ghost.dot_counter >= dot_counters[ghost.type])
			ghost.target_state = LEAVEHOME;

		if (ghost.pos.y <= 14) {
			ghost.pos.y = 14;
			ghost.cur_dir = DOWN;
		}
		else if (ghost.pos.y >= 15) {
			ghost.pos.y = 15;
			ghost.cur_dir = UP;
		}
		break;
	}
}
void UpdateGhosts()
{
	for (int i = 0; i < 4; i++) {
		Ghost* ghost = gState.ghosts[i];
		sf::Vector2f prev_pos = ghost->pos;

		ghost->pos += dir_addition[ghost->cur_dir] * ghost->move_speed;
		if (ghost->in_house) {
			HouseUpdate(*ghost);
		}
		// if ghost pos is in the middle of the tile, didnt update last turn, and isnt in the tunnel, 
		// do waypoint calculations, else do nothing
		else if (!ghost->update_dir &&
			InMiddleTile(ghost->pos, prev_pos, ghost->cur_dir) &&
			!InTunnel(ghost->pos)) {
			UpdateDirection(GetAvailableSquares(ghost->pos, ghost->cur_dir, false), *ghost);
			ghost->update_dir = true;
		}
		else {
			ghost->update_dir = false;
		}

		// blech, at least it terminates quickly
		if (ghost->target_state == GOHOME &&
			PassedEntrence(*ghost)) {
			ghost->target_state = ENTERHOME;
			ghost->in_house = true;
			ghost->cur_dir = DOWN;
			//ghost->move_speed = 0.02;
			ghost->pos.x = 14;
		}

		// tunneling
		if (ghost->pos.x < -1) {
			ghost->pos.x += 29;
		}
		else if (ghost->pos.x >= 29) {
			ghost->pos.x -= 29;
		}
	}
}
Dir GetOppositeTile(Ghost& ghost)
{
	// this will be the case 99% of times
	if (!TileCollision(ghost.pos + dir_addition[opposite_dir[ghost.cur_dir]] * 0.9))
		return opposite_dir[ghost.cur_dir];


	auto squares = GetAvailableSquares(ghost.pos, ghost.cur_dir, false);
	if (!squares.empty())
		return squares.at(0);

	// last case scenario, dont reverse
	return ghost.cur_dir;

}
void SetAllGhostState(TargetState new_state)
{
	for (int i = 0; i < 4; i++) {
		SetGhostState(*gState.ghosts[i], new_state);
	}
}

void SetGhostState(Ghost& ghost, TargetState new_state)
{
	if (ghost.target_state == GOHOME)
		return;
	switch (new_state)
	{
	case LEAVEHOME:
		if (ghost.in_house)
			ghost.target_state = new_state;
	case ENTERHOME:
	case HOMEBASE:
		break;
	case FRIGHTENED:
		if (!ghost.in_house) {
			ghost.cur_dir = GetOppositeTile(ghost);
			ghost.move_speed = ghost_fright;
			ghost.target_state = new_state;
		}
		break;
	case GOHOME:
		if (!ghost.in_house) {
			ghost.move_speed = gohome_speed;
			ghost.target_state = new_state;
		}
		break;
	default:
		if (!ghost.in_house) {
			// ghost reverses direction whenever changing states,
			// except when coming from frightened
			if (ghost.target_state != FRIGHTENED)
				ghost.cur_dir = GetOppositeTile(ghost);
			ghost.move_speed = ghost_speed;
			ghost.target_state = new_state;
		}
		break;
	}
}
