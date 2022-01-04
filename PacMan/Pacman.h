#ifndef PACMAN_H
#define PACMAN_H
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

enum Dir
{
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT,
};
enum GhostType
{
	RED,
	PINK,
	BLUE,
	ORANGE,
};
enum TargetState
{
	CHASE,
	CORNER,
	FRIGHTENED,
	GOHOME,		// inital state to pathfind to the square above the door
	HOMEBASE,	// indefinite state going up and down in home
	LEAVEHOME,
	ENTERHOME,
};
enum State
{
	MENU,
	GAMESTART,
	MAINLOOP,
	GAMEWIN,
	GAMELOSE,
	GAMEOVER,
};
const float TSIZE = 8;
//const float MSPEED = 0.3;
const int BSIZE = (28 + 1) * 31;
const int YOFFSET = 8 * 3;
const float win_ratio = 28.f / 36.f;

const Dir opposite_dir[5] = { NONE, DOWN, UP, RIGHT, LEFT };
const sf::Vector2f dir_addition[5] = { {0,0}, {0,-1},{0,1},{-1,0},{1,0} };

// total amount of pellets in maze
const int pellet_amt = 244;
const int pill_score = 10;
const int pow_score = 50;

const float fullmovement = 0.2;
const float player_speed = fullmovement * 0.8;
const float player_fright = fullmovement * 0.9;
const float ghost_speed = fullmovement * 0.75;
const float ghost_fright = fullmovement * 0.5;

const float reg_speed = 0.15;
const float gohome_speed = 0.4;
const float frightened_speed = 0.1;
const float inhome_speed = 0.05;

// seconds
const int fright_time = 6;

// scatter, chase repeating
const int wave_times[8] = { 7,20,7,20,5,20,5,-1 };

//
// Global states
//
struct Ghost
{
	GhostType type;
	sf::Vector2f pos;
	Dir cur_dir;
	TargetState target_state;
	float move_speed = ghost_speed;
	bool update_dir = false;
	bool in_house = false;
	bool enable_draw = true;

	int dot_counter = 0;
};
struct Player
{
	sf::Vector2f pos;
	Dir cur_dir;
	Dir correction;
	Dir try_dir;

	bool stopped;
	bool cornering;
	bool enable_draw = true;
};

struct GameState
{
	State game_state;

	int pellets_left = pellet_amt;
	int current_level = 1;
	int player_lives = 0;
	int game_score = 0;
	int high_score;
	int wave_counter = 0;

	bool pellet_eaten = false;

	bool first_life = true;
	bool using_global_counter = false;
	int global_dot_counter = 0;
	
	bool player_eat_ghost = false;
	int ghosts_eaten_in_powerup = 0;
	Ghost* recent_eaten = nullptr;

	std::vector<Ghost*> ghosts;
	Player* player;

	std::vector<std::string> board;

	// Timers in milliseconds
	int energizer_time = 0;
	int pause_time = 0;
	int wave_time = 0;

	sf::RenderWindow* window;
};

extern GameState gState;

//
// General Functions
//
bool TileCollision(sf::Vector2f pos, bool home_tiles = false);
bool PlayerTileCollision(Dir dir, sf::Vector2f pos);
void CenterObject(Dir dir, sf::Vector2f& pos);
inline bool FComp(float a, float b)
{
	float diff = a - b;
	return (diff < 0.01f) && (-diff < 0.01f);
}
inline bool InTunnel(sf::Vector2f pos)
{
	return pos.x < 0 || pos.x > 27;
}

inline char GetTile(int x, int y)
{
	// for tunnel, / not used for anything
	if (x < 0 || x >= gState.board.at(y).size())
		return '/';

	return gState.board.at(y).at(x);
}
inline void SetTile(int x, int y, char new_c)
{
	// for tunnel, / not used for anything
	if (x < 0 || x >= gState.board.at(y).size())
		return;

	 gState.board.at(y).at(x) = new_c;
}
inline sf::Vector2f operator * (sf::Vector2f vec, float num)
{
	return { vec.x * num, vec.y * num };
}

inline TargetState GetGlobalTarget()
{
	if (gState.wave_counter >= 7)
		return CHASE;

	return (gState.wave_counter % 2) ? CHASE : CORNER;
}
inline bool GhostRetreating()
{
	for (int i = 0; i < 4; i++) {
		if (gState.ghosts[i]->target_state == GOHOME)
			return true;
	}

	return false;
}
#endif // !PACMAN_H

