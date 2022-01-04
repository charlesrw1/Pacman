#include "Sound.h"

static Sounds* sounds;
static SoundState sstate;
constexpr int total_death_time = 1500;
void InitSounds()
{
	sounds = new Sounds;

	sounds->death_1.loadFromFile("audio/death_1.wav");
	sounds->death_2.loadFromFile("audio/death_2.wav");
	sounds->eat_ghost.loadFromFile("audio/eat_ghost.wav");
	sounds->game_start.loadFromFile("audio/game_start.wav");
	sounds->munch1.loadFromFile("audio/munch_1.wav");
	sounds->munch2.loadFromFile("audio/munch_2.wav");
	sounds->power_pellet.loadFromFile("audio/power_pellet.wav");
	sounds->retreating.loadFromFile("audio/retreating.wav");
	sounds->siren.loadFromFile("audio/siren_3.wav");

	sounds->ghost_eat.setBuffer(sounds->eat_ghost);

	sounds->background.setLoop(true);

	sounds->death1.setBuffer(sounds->death_1);
	sounds->death2.setBuffer(sounds->death_2);
	sstate.death_timer = total_death_time;
}
void PlayMunch()
{
	if (sstate.first_munch) 
		sounds->munch.setBuffer(sounds->munch1);
	else 
		sounds->munch.setBuffer(sounds->munch2);

	sstate.first_munch = !sstate.first_munch;

	sounds->munch.play();
}
void PlayDeathSound()
{
	sstate.playing_death = true;
}
void PlayEatGhost()
{
	sounds->ghost_eat.play();
}
void PlayGameStart()
{
	sounds->background.stop();
	sounds->background.setPitch(1);
	sounds->background.setBuffer(sounds->game_start);
	sounds->background.play();
}
const float pitches[5] = { 0.75,0.87,1,1.13,1.25 };
void UpdateGameSounds(int ms_elapsed)
{
	if (sstate.playing_death) {
		if (sstate.death_timer <= total_death_time - 250 && sounds->death1.getStatus() != sf::SoundSource::Status::Playing) {
			sounds->death1.play();
		}
		sstate.death_timer -= ms_elapsed;
		if (sstate.death_timer <= 0) {
			sounds->death2.play();
			sstate.death_timer = total_death_time;
			sstate.playing_death = false;
		}
	}
	
	if (gState.game_state != MAINLOOP)
		return;

	bool update_sound = false;
	if (GhostRetreating()) {
		if (sstate.bk_state != RETREAT) {
			update_sound = true;
			sstate.bk_state = RETREAT;
		}
	}
	else if (gState.energizer_time > 0) {
		if (sstate.bk_state != PPELLET) {
			update_sound = true;
			sstate.bk_state = PPELLET;
		}
	}
	else if(gState.game_state == MAINLOOP){
		if (sstate.bk_state != SIREN) {
			update_sound = true;
			sstate.bk_state = SIREN;
		}
	}

	if (update_sound) {
		switch (sstate.bk_state)
		{
		case SIREN:
			sounds->background.setPitch(pitches[int((-4/244.f) * gState.pellets_left + 4)]);
			sounds->background.setBuffer(sounds->siren);
			break;
		case RETREAT:
			sounds->background.setPitch(1);
			sounds->background.setBuffer(sounds->retreating);
			break;
		case PPELLET:
			sounds->background.setPitch(1);
			sounds->background.setBuffer(sounds->power_pellet);
			break;
		}

		sounds->background.play();
	}
}
void StopSounds()
{
	sounds->background.stop();
	sstate.bk_state = NO_SOUND;
}
