#include "raylib.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct enemystruct {
	Texture2D texture;
	Vector2 position;
	int speed;
	texture = LoadTexture("resources/p1l.png");
};


void main() {
	//initializing window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game title"); 
	SetTargetFPS(60);

	// loading textures
	Texture2D player;
	player = LoadTexture("resources/p1r.png");

	

	Texture2D bg;
	bg = LoadTexture("resources/bg.png");
 

//vectors
	Vector2 playerpos = { 0, 0 };
	Vector2 enemypos = { GetRandomValue(100,300),GetRandomValue(100,300)};

	//variables
	int playerspeed = 3;
	int enemyspeed = 2;
	float timer = 0.0f;
	
	Rectangle src; //player rec
	src.width = player.width;
	src.height = player.height;
	src.x = playerpos.x;
	src.y = playerpos.y;

	//initializing camera
	Camera2D playerCam = {0};
	playerCam.target = playerpos;
	playerCam.offset = { (SCREEN_WIDTH / 2.0f) - (player.width / 2), (SCREEN_HEIGHT / 2.0f) - (player.height / 2) };
	playerCam.rotation = 0.0f;
	playerCam.zoom = 1.4f;

	// GAME LOOP
	while (!WindowShouldClose()) {

		//enabling fullscreen mode
		if (IsKeyPressed(KEY_F))
			ToggleFullscreen();

		playerCam.target = { playerpos.x + 20, playerpos.y + 20 };

		//character movement
		if (IsKeyDown(KEY_RIGHT)) {
			playerpos.x += playerspeed;
			src.width = player.width; 	      //changing texture to facing right
		}

		if (IsKeyDown(KEY_LEFT)) {
			playerpos.x -= playerspeed;
			src.width = -player.width; 		 //changing texture to facing right
		}

		if (IsKeyDown(KEY_UP))
			playerpos.y -= playerspeed;
		if (IsKeyDown(KEY_DOWN))
			playerpos.y += playerspeed;

		//spawn enemies
		timer += GetFrameTime();

			//drawing
			BeginDrawing();
		BeginMode2D(playerCam); //showing camera

		ClearBackground(WHITE);

		DrawTexture(bg, 0, 0, WHITE);

		DrawTexture(player, playerpos.x, playerpos.y, WHITE);
		if (timer>5.0f) {
		
		enemystruct enemy1;
		enemy1.position = { (float)GetRandomValue(100,300),(float)GetRandomValue(100,300) };
		enemy1.texture = 
			DrawTexture(enemy1, enemypos.x, enemypos.y, WHITE);
		
	}
		EndDrawing();
	 }
}