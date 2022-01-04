#ifndef PLAYER_H
#define PLAYER_H
#include "Pacman.h"

Dir GetCorrection(Dir pdir, sf::Vector2f ppos);
void Cornering();
void ResolveCollision();
void PlayerMovement();

#endif // !PLAYER_H

