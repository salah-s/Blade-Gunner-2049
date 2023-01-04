#include "raylib.h"
#define screenheight 720
#define screenwidth 1280

void main() {
	//initializing window
 InitWindow(screenwidth,screenheight,"game title");
 SetTargetFPS(60);

 // loading textures
 Texture2D playerr;
 playerr = LoadTexture("resources/p1r.png");

 Texture2D playerl;
 playerl = LoadTexture("resources/p1l.png");

 Texture2D player;
 player = LoadTexture("resources/p1r.png");

 Texture2D bg;
 bg = LoadTexture("resources/bg.png");

 
 Vector2 characterpos = { 0, 0 };
  int playerspeed = 3;

  //initializing camera
  Camera2D playercam = {0};
  playercam.target = characterpos;
  playercam.offset = { screenwidth / 2.0f, screenheight / 2.0f };
  playercam.rotation = 0.0f;
  playercam.zoom = 1.4f;

 // GAME LOOP
 while (!WindowShouldClose()) {

	 //enabling fullscreen mode
	 if (IsKeyPressed(KEY_F))
		 ToggleFullscreen();

	 playercam.target = { characterpos.x + 20, characterpos.y + 20 };

	 //character movement
	 if (IsKeyDown(KEY_RIGHT)) {
		 characterpos.x += playerspeed;	
		 player = playerr;       //changing texture to facing right
	 }
	 if (IsKeyDown(KEY_LEFT)) {
		 characterpos.x -= playerspeed;
		 player = playerl; 		 //changing texture to facing right
	 }
	 if (IsKeyDown(KEY_UP))
		 characterpos.y -= playerspeed;
	 if (IsKeyDown(KEY_DOWN))
		 characterpos.y += playerspeed;



		

	 //drawing
	 BeginDrawing();

	 BeginMode2D(playercam); //showing camera
	
	 ClearBackground(WHITE);

	 DrawTexture(bg,0, 0, WHITE);

		DrawTexture(player, characterpos.x, characterpos.y, WHITE);  

	 EndDrawing();



 }


}