#include "Animate.h"

static Animation animate;

sf::IntRect GetGhostFrame(GhostType type, TargetState state, Dir dir)
{
	sf::IntRect ghost = { 0,128,32,32 };

	int offset = 0;
	if (state != FRIGHTENED) {
		switch (dir)
		{
		case UP:
			offset = 128;
			break;
		case DOWN:
			offset = 128 + 64;
			break;
		case LEFT:
			offset = 64;
			break;
		}
	}
	
	if (state == FRIGHTENED) {
		ghost.left = 256;
		if (animate.fright_flash)
			ghost.left += 64;
	}
	else if (state == GOHOME || state == ENTERHOME) {
		ghost.left = 256 + (offset/2);
		ghost.top = 128 + 32;
	}
	else {
		ghost.top += 32 * type;
		ghost.left = offset;
	}
	if(state != GOHOME && state != ENTERHOME)
		ghost.left += animate.ghost_frame_2 * 32;

	return ghost;
}
void AnimateUpdate(int ms_elapsed)
{
	PulseUpdate(ms_elapsed);

	if (gState.player->stopped && !animate.death_animation) {
		animate.pacman_frame = 0;
		animate.assending = true;
	}
	else {
		animate.pacman_timer += ms_elapsed;
	}
	
	if (animate.death_animation) {
		if (animate.pacman_timer > 100) {
			animate.pacman_timer = 0;
			animate.pacman_frame++;
		}
		if (animate.pacman_frame > 10)
			gState.player->enable_draw = false;
	}
	else if (animate.pacman_timer > 25 && !gState.player->stopped) {
		animate.pacman_frame += (animate.assending) ? 1 : -1;
		animate.pacman_timer = 0;
	}
	if (!animate.death_animation && animate.pacman_frame > 2 || animate.pacman_frame < 0) {
		animate.assending = !animate.assending;
		animate.pacman_frame = (animate.pacman_frame > 2) ? 2 : 0;
	}

	animate.ghost_timer += ms_elapsed;
	if (animate.ghost_timer > 200) {
		animate.ghost_frame_2 = !animate.ghost_frame_2;
		animate.ghost_timer = 0;
	}

	// start flashing with 2 seconds to go
	if (gState.energizer_time > 0 && gState.energizer_time < 2000) {
		animate.energrizer_timer += ms_elapsed;
		if (animate.energrizer_timer > 200) {
			animate.fright_flash = !animate.fright_flash;
			animate.energrizer_timer = 0;
		}
	}
	else animate.fright_flash = false;
}
sf::IntRect GetPacManFrame(Dir dir)
{
	sf::IntRect rect = { 0,0,30,30 };
	rect.left = (2 - animate.pacman_frame) * 32;

	if (animate.death_animation) {
		rect.left = 96 + animate.pacman_frame * 32;

		return rect;
	}

	if (animate.pacman_frame == 0)
		return rect;

	switch (dir)
	{
	case UP:
		rect.top = 64;
		break;
	case DOWN:
		rect.top = 96;
		break;
	case LEFT:
		rect.top = 32;
	}

	return rect;
}
void StartPacManDeath()
{
	animate.death_animation = true;
	animate.pacman_frame = 0;
	animate.pacman_timer = -250;
}
void ResetAnimation()
{
	animate.pacman_frame = 0;
	animate.death_animation = false;
}
void SetPacManMenuFrame()
{
	animate.pacman_frame = 1;
	animate.death_animation = false;

	animate.ghost_frame_2 = false;
}
void PulseUpdate(int ms_elapsed)
{
	animate.pulse_timer += ms_elapsed;
	if (animate.pulse_timer > animate.pulse_limit) {
		animate.pulse = !animate.pulse;
		animate.pulse_timer = 0;
	}
}
void SetPulseFrequency(int ms)
{
	animate.pulse_limit = ms;
}
bool IsPulse()
{
	return animate.pulse;
}