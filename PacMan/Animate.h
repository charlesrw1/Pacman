#ifndef ANIMATE_H
#define ANIMATE_H
#include "Pacman.h"

struct Animation
{
	int pacman_frame=0;
	int pacman_timer=0;
	bool assending = true;
	
	bool ghost_frame_2 = false;
	int ghost_timer = 0;

	int fright_flash = false;
	int energrizer_timer = 0;

	bool death_animation = false;

	// genertic pulse for win condition and energizer
	int pulse = true;
	int pulse_timer = 0;

	int pulse_limit = 200;
};

void AnimateUpdate(int ms_elapsed);
void StartPacManDeath();
void ResetAnimation();
void SetPacManMenuFrame();

sf::IntRect GetGhostFrame(GhostType type, TargetState state, Dir dir);
sf::IntRect GetPacManFrame(Dir dir);

void PulseUpdate(int ms_elapsed);
bool IsPulse();
void SetPulseFrequency(int ms);

#endif // !ANIMATE_H
