#include "Gameloop.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
GameState gState;

void OnStart()
{
	Init();
	LoadHighScore();
}
void OnQuit()
{
	SaveHighScore();
}

void LoadHighScore()
{
	std::ifstream infile;
	std::stringstream ss;
	std::string line;
	int hs = 0;
	infile.open("highscore.txt");
	if (!infile) {
		gState.high_score = 0;
		return;
	}
	getline(infile, line);
	ss << line;
	ss >> hs;

	gState.high_score = hs;
}
void SaveHighScore()
{
	std::ofstream outfile("highscore.txt");
	if (!outfile.is_open())
		printf("Cant open file!");
	
	outfile << gState.high_score;
	outfile.close();
}
void InitBoard()
{
	std::string line;
	std::ifstream infile("Map.txt");
	if (!infile)
		return;
	while (getline(infile, line))
	{
		gState.board.push_back(line);
	}

	infile.close();

}
void Init()
{
	Player* pl = new Player();
	pl->cur_dir = UP;
	pl->pos = { 14,23.5 };
	pl->stopped = true;
	gState.player = pl;

	// Ghosts init

	Ghost* temp = new Ghost();
	temp->type = RED;
	gState.ghosts.push_back(temp);

	temp = new Ghost();
	temp->type = PINK;
	gState.ghosts.push_back(temp);

	temp = new Ghost();
	temp->type = BLUE;
	gState.ghosts.push_back(temp);

	temp = new Ghost();
	temp->type = ORANGE;
	gState.ghosts.push_back(temp);

	InitBoard();
	InitRender();
	ResetGhostsAndPlayer();
	
	InitSounds();
	
	SetupMenu();
	gState.game_state = MENU;
	gState.pause_time = 2000;
}
void ResetGhostsAndPlayer()
{
	Ghost* temp = gState.ghosts[0];
	temp->pos = { 14, 11.5 };
	temp->cur_dir = LEFT;
	temp->target_state = CORNER;
	temp->in_house = false;
	temp->move_speed = ghost_speed;
	temp->dot_counter = 0;
	temp->enable_draw = true;

	temp = gState.ghosts[1];
	temp->pos = { 14, 14.5 };
	temp->cur_dir = UP;
	temp->target_state = HOMEBASE;
	temp->in_house = true;
	temp->move_speed = inhome_speed;
	temp->dot_counter = 0;
	temp->enable_draw = true;

	temp = gState.ghosts[2];
	temp->pos = { 12, 14.5 };
	temp->cur_dir = DOWN;
	temp->target_state = HOMEBASE;
	temp->in_house = true;
	temp->move_speed = inhome_speed;
	temp->dot_counter = 0;
	temp->enable_draw = true;

	temp = gState.ghosts[3];
	temp->pos = { 16, 14.5 };
	temp->cur_dir = DOWN;
	temp->target_state = HOMEBASE;
	temp->in_house = true;
	temp->move_speed = inhome_speed;
	temp->dot_counter = 0;
	temp->enable_draw = true;

	gState.player->cur_dir = UP;
	gState.player->pos = { 14,23.5 };
	gState.player->stopped = true;
	gState.player->enable_draw = true;

	ResetAnimation();
	gState.energizer_time = 0;
	gState.wave_counter = 0;
	gState.wave_time = 0;

	ResetPPelletFlash();
}
void ResetBoard()
{
	gState.board.clear();
	InitBoard();

	gState.pellets_left = 244;
}

void IncrementGhostHouse()
{
	for (int i = 0; i < 4; i++) {
		if (gState.ghosts[i]->target_state == HOMEBASE) {
			gState.ghosts[i]->dot_counter++;
			return;
		}
	}
}
void CheckPelletCollision()
{
	char tile = GetTile(gState.player->pos.x, gState.player->pos.y);
	bool collided = false;
	if (tile == '.') {
		collided = true;
		// play sound
		gState.game_score += 10;
		PlayMunch();
	}
	else if (tile == 'o') {
		collided = true;
		gState.game_score += 50;
		gState.energizer_time = fright_time*1000;

		gState.ghosts_eaten_in_powerup = 0;

		SetAllGhostState(FRIGHTENED);
	}

	if (collided) {
		RemovePellet(gState.player->pos.x, gState.player->pos.y);
		SetTile(gState.player->pos.x, gState.player->pos.y, ' ');
		IncrementGhostHouse();
		gState.pellet_eaten = true;
		gState.pellets_left--;
	}
}
void CheckGhostCollision()
{
	int px = (int)gState.player->pos.x;
	int py = (int)gState.player->pos.y;

	for (int i = 0; i < 4; i++) {
		if ((int)gState.ghosts[i]->pos.x == px && (int)gState.ghosts[i]->pos.y == py) {
			if (gState.ghosts[i]->target_state == FRIGHTENED) {
				SetGhostState(*gState.ghosts[i], GOHOME);
				gState.recent_eaten = gState.ghosts[i];
				gState.ghosts_eaten_in_powerup++;
				gState.game_score += (pow(2,gState.ghosts_eaten_in_powerup) * 100);
				
				gState.player_eat_ghost = true;
				gState.pause_time = 500;

				gState.ghosts[i]->enable_draw = false;
				gState.player->enable_draw = false;

				PlayEatGhost();
			}
			else if (gState.ghosts[i]->target_state != GOHOME) {
				gState.game_state = GAMELOSE;
				gState.pause_time = 2000;
				gState.player_lives -= 1;
				StartPacManDeath();
				StopSounds();
				PlayDeathSound();
				printf("RESET\n");
			}
		}
	}
}
void UpdateWave(int ms_elapsed)
{
	// indefinte chase mode
	if (gState.wave_counter >= 7)
		return;

	gState.wave_time += ms_elapsed;
	if (gState.wave_time / 1000 >= wave_times[gState.wave_counter]) {
		gState.wave_counter++;
		printf("New wave\n");
		if(gState.energizer_time <= 0)
			SetAllGhostState(GetGlobalTarget());
		gState.wave_time = 0;
	}

}
void UpdateEnergizerTime(int ms_elasped)
{
	gState.energizer_time -= ms_elasped;
	if (gState.energizer_time <= 0) {
		SetAllGhostState(GetGlobalTarget());
	}
}
void CheckHighScore()
{
	if (gState.game_score > gState.high_score)
		gState.high_score = gState.game_score;
}
void CheckWin()
{
	if (gState.pellets_left <= 0) {
		gState.game_state = GAMEWIN;
		for (int i = 0; i < 4; i++) {
			gState.ghosts[i]->enable_draw = false;
		}
		gState.player->stopped = true;
		gState.pause_time = 2000;
		StopSounds();
		SetPulseFrequency(200);
	}
}
void MainLoop(int ms_elapsed)
{
	if (gState.player_eat_ghost) {
		gState.pause_time -= ms_elapsed;
		if (gState.pause_time < 0) {
			gState.recent_eaten->enable_draw = true;
			gState.player->enable_draw = true;
			gState.player_eat_ghost = false;
		}
		DrawFrame();

		return;
	}

	// pacman doesnt move for one frame if he eats a pellet
	// from the original game
	if (!gState.pellet_eaten)
		PlayerMovement();
	else gState.pellet_eaten = false;
	// check collision first so less funny stuff
	CheckGhostCollision();
	CheckPelletCollision();
	UpdateGhosts();
	UpdateWave(ms_elapsed);
	UpdateEnergizerTime(ms_elapsed);
	CheckHighScore();
	CheckWin();
	UpdateGameSounds(ms_elapsed);

	AnimateUpdate(ms_elapsed);
	DrawFrame();
}
void GameStart(int ms_elasped)
{
	gState.pause_time -= ms_elasped;
	if (gState.pause_time <= 0) {
		gState.game_state = MAINLOOP;
		SetPulseFrequency(150);
	}

	DrawFrame();
}
void GameLose(int ms_elapsed)
{
	gState.pause_time -= ms_elapsed;
	if (gState.pause_time <= 0) {
		if (gState.player_lives < 0) {
			gState.game_state = GAMEOVER;
			gState.pause_time = 5000;
			for (int i = 0; i < 4; i++)
				gState.ghosts[i]->enable_draw = false;
			gState.player->enable_draw = false;
		}
		else {
			gState.game_state = GAMESTART;
			gState.pause_time = 2000;
			
			ResetGhostsAndPlayer();
		}
	}
	UpdateGameSounds(ms_elapsed);
	AnimateUpdate(ms_elapsed);
	DrawFrame();
}
void GameWin(int ms_elapsed)
{
	gState.pause_time -= ms_elapsed;
	if (gState.pause_time <= 0) {
		ResetPellets();
		ResetBoard();
		ResetGhostsAndPlayer();
		gState.pause_time = 2000;
		gState.game_state = GAMESTART;
	}
	AnimateUpdate(ms_elapsed);
	DrawFrame();
}
void SetupMenu()
{
	for (int i = 0; i < 4; i++) {
		gState.ghosts[i]->enable_draw = true;
		gState.ghosts[i]->pos = { 6,5.5f + i * 3.f };
		gState.ghosts[i]->cur_dir = RIGHT;
		gState.ghosts[i]->target_state = CHASE;
		gState.ghosts[i]->in_house = false;
	}
	gState.player->enable_draw = true;
	gState.player->pos = { 6,17.5f };
	gState.player->cur_dir = RIGHT;
	SetPacManMenuFrame();
	SetPulseFrequency(200);
}
void Menu(int ms_elapsed)
{
	PulseUpdate(ms_elapsed);
	DrawMenuFrame();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
	{
		ResetPellets();
		ResetBoard();
		ResetGhostsAndPlayer();
		gState.game_score = 0;
		gState.player_lives = 3;
		PlayGameStart();
		gState.pause_time = 4000;
		gState.game_state = GAMESTART;
	}
}
void GameLoop(int ms_elapsed)
{
	switch (gState.game_state)
	{
	case MAINLOOP:
		MainLoop(ms_elapsed);
		break;
	case GAMESTART:
		GameStart(ms_elapsed);
		break;
	case GAMELOSE:
		GameLose(ms_elapsed);
		break;
	case GAMEOVER:
		gState.pause_time -= ms_elapsed;
		if (gState.pause_time < 0) {
			SetupMenu();
			gState.game_state = MENU;
		}
			//gState.game_state = MENU;
		DrawFrame();
		break;
	case GAMEWIN:
		GameWin(ms_elapsed);
		break;
	case MENU:
		Menu(ms_elapsed);
		break;
	}
}
