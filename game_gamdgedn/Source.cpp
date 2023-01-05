#include "raylib.h"
#include <math.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define max_enemies 200

//this function is not working 100% correctly: (it only spawns enemies at the starting position of the player,its not following the player position)
// a function that generates a random number between a min dist from the player and a max dist from the player
float GetRandomSafeValue(float playerposx, float mindis, float maxdis) {
	int x;

	for (int i = 0;i < 500;i++) {
		x = GetRandomValue(-maxdis, maxdis);
		if ((x< playerposx + maxdis && x>playerposx + mindis) || x > playerposx - maxdis && x < playerposx - mindis)
			return x;
	}
}


struct Enemy { //making enemies struct 
	Texture2D texture;
	Vector2 position;
	Vector2 direction;
	int hyp;
	bool active;
};


void main() {
	//initializing window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game title");
	SetTargetFPS(60);



	// loading textures
	Texture2D player = LoadTexture("resources/p1r.png");

	Texture2D enemytex = LoadTexture("resources/mob.png");

	Texture2D bg = LoadTexture("resources/bg.png");


	//vectors
	Vector2 playerpos = { 0, 0 };
	Vector2 enemypos;

	//variables
	int playerspeed = 3;
	int enemyspeed = 2;
	int etimer = 0;
	int enemyspawnrate = 3;

	//initializing camera
	Camera2D playerCam = { 0 };
	playerCam.target = playerpos;
	playerCam.offset = { (SCREEN_WIDTH / 2.0f) - (player.width / 2), (SCREEN_HEIGHT / 2.0f) - (player.height / 2) };
	playerCam.rotation = 0.0f;
	playerCam.zoom = 1.4f;

	//making an array of "Enemy(s)"
	struct Enemy e[max_enemies] = { 0 };


	//intitalizing enemies
	for (int i = 0; i < max_enemies; i++)
	{

		e[i].position = { Vector2{0,0} };
		e[i].active = false;
	}

	// GAME LOOP
	while (!WindowShouldClose()) {

		//enabling fullscreen mode
		if (IsKeyPressed(KEY_F))
			ToggleFullscreen();

		playerCam.target = { playerpos.x + 20, playerpos.y + 20 };

		//character movement
		if (IsKeyDown(KEY_RIGHT)) {
			playerpos.x += playerspeed;

		}

		if (IsKeyDown(KEY_LEFT)) {
			playerpos.x -= playerspeed;

		}

		if (IsKeyDown(KEY_UP))
			playerpos.y -= playerspeed;
		if (IsKeyDown(KEY_DOWN))
			playerpos.y += playerspeed;

		//activating enemies and giving them positions
		etimer++;
		for (int i = 0;i < max_enemies;i++) {

			if (!e[i].active && etimer % (60 / enemyspawnrate) == 0) {
				e[i].active = true;
				e[i].position.x = GetRandomSafeValue(playerpos.x, 100, 200);
				e[i].position.y = GetRandomSafeValue(playerpos.y, 100, 200);

				break;
			}
		}


		//update enemies
		for (int i = 0;i < max_enemies;i++) {

			if (e[i].active) {
				//enemy direction to follow player = player position - enemy position 
				e[i].direction.x = (playerpos.x - e[i].position.x);
				e[i].direction.y = (playerpos.y - e[i].position.y);

				//normalizing the direction vector
				e[i].hyp = sqrt(e[i].direction.x * e[i].direction.x + e[i].direction.y * e[i].direction.y);
				e[i].direction.x /= e[i].hyp;
				e[i].direction.y /= e[i].hyp;

				// update enemy position (movement)
				e[i].position.x += e[1].direction.x * 1;
				e[i].position.y += e[1].direction.y * 1;
			}
		}



		//drawing
		BeginDrawing();
		BeginMode2D(playerCam); //showing camera

		ClearBackground(WHITE);

		DrawTexture(bg, 0, 0, WHITE);

		DrawTexture(player, playerpos.x, playerpos.y, WHITE);



		//drawing active enemies

		for (int i = 0; i < max_enemies; i++)
		{
			if (e[i].active)
				DrawTexture(enemytex, e[i].position.x, e[i].position.y, WHITE);
		}



		EndDrawing();
	}
}