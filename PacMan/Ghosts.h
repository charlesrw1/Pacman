#ifndef GHOST_H
#define GHOST_H
#include "Pacman.h"

bool InMiddleTile(sf::Vector2f pos, sf::Vector2f prev, Dir dir);
std::vector<Dir> GetAvailableSquares(sf::Vector2f pos, Dir dir, bool home_tiles);
float Distance(int x, int y, int x1, int y1);
Dir GetShortestDir(const std::vector<Dir>& squares, const Ghost& ghost, sf::Vector2f target);
Dir GetOppositeTile(Ghost& ghost);

sf::Vector2f BlinkyUpdate(Ghost& ghost);
sf::Vector2f PinkyUpdate(Ghost& ghost);
sf::Vector2f InkyUpdate(Ghost& ghost);
sf::Vector2f ClydeUpdate(Ghost& ghost);

bool PassedEntrence(Ghost& ghost);
void HouseUpdate(Ghost& ghost);

void UpdateDirection(std::vector<Dir> squares, Ghost& ghost);
void UpdateGhosts();

void SetAllGhostState(TargetState new_state);
void SetGhostState(Ghost& ghost, TargetState new_state);

const float starting_x[4] = { 14,14,12,16 };
const Dir enter_dir[4] = { UP, UP, LEFT, RIGHT };
const sf::Vector2f corners[4] = { {31,0},{0,0},{31,31}, {0,31} };
const int dot_counters[4] = { 0,0,30,60 };

const int global_dot_limit[4] = { 0,7,17,32 };

#endif // !GHOST_H

