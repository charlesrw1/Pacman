#ifndef SOUND_H
#define SOUND_H
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Pacman.h"

struct Sounds
{
	sf::SoundBuffer munch1;
	sf::SoundBuffer munch2;

	sf::SoundBuffer game_start;
	sf::SoundBuffer death_1;
	sf::SoundBuffer death_2;

	sf::SoundBuffer eat_ghost;

	sf::SoundBuffer retreating;
	sf::SoundBuffer power_pellet;
	sf::SoundBuffer siren;

	sf::Sound munch;
	sf::Sound ghost_eat;
	sf::Sound background;

	sf::Sound death1;
	sf::Sound death2;
};
enum BkState
{
	NO_SOUND,
	SIREN,
	PPELLET,
	RETREAT,
};
struct SoundState
{
	bool first_munch = true;
	bool playing_death = false;
	int death_timer = 0;
	BkState bk_state = NO_SOUND;
};
void InitSounds();

void PlayMunch();
void PlayEatGhost();
void PlayDeathSound();
void PlayGameStart();

// automatically plays retreating/power pellet/siren sounds
void UpdateGameSounds(int ms_elapsed);
void StopSounds();

#endif // !SOUND_H

