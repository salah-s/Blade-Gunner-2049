#include "raylib.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void main() {
	//initializing window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game title"); 
	SetTargetFPS(60);

	// loading textures
	Texture2D player;
	player = LoadTexture("resources/p1r.png");

	Texture2D bg;
	bg = LoadTexture("resources/bg.png");
 
	Vector2 characterPos = { 0, 0 };
	int playerspeed = 3;
	
	Rectangle src;
	src.width = player.width;
	src.height = player.height;
	src.x = characterPos.x;
	src.y = characterPos.y;

	//initializing camera
	Camera2D playerCam = {0};
  
	playerCam.target = characterPos;
	playerCam.offset = { (SCREEN_WIDTH / 2.0f) - (player.width / 2), (SCREEN_HEIGHT / 2.0f) - (player.height / 2) };
	playerCam.rotation = 0.0f;
	playerCam.zoom = 1.4f;

	// GAME LOOP
	while (!WindowShouldClose()) {
		
		//enabling fullscreen mode
		if (IsKeyPressed(KEY_F))
			ToggleFullscreen();

		playerCam.target = { characterPos.x + 20, characterPos.y + 20 };

		//character movement
		if (IsKeyDown(KEY_RIGHT)) {
			characterPos.x += playerspeed;	
			src.width = player.width; 	      //changing texture to facing right
		}
		
		if (IsKeyDown(KEY_LEFT)) {
			characterPos.x -= playerspeed;
			src.width = -player.width; 		 //changing texture to facing right
		}
		
		if (IsKeyDown(KEY_UP))
			characterPos.y -= playerspeed;
		if (IsKeyDown(KEY_DOWN))
			characterPos.y += playerspeed;

		//drawing
		BeginDrawing();
			BeginMode2D(playerCam); //showing camera
	
			ClearBackground(WHITE);

			DrawTexture(bg,0, 0, WHITE);

			DrawTextureRec(player, src,characterPos, WHITE);  

		EndDrawing();
	 }
}