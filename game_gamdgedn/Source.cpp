#include "raylib.h"
#define screenheight 450
#define screenwidth 800

void main() {

 InitWindow(screenwidth,screenheight,"game title");

 Texture2D player;
 player = LoadTexture("resources/fawzy.gif");
 
 Texture2D bg;
 bg = LoadTexture("resources/zarya.png");

 Vector2 characterpos = { 0, 0 };
 
 SetTargetFPS(60);
 while (!WindowShouldClose()) {

	 BeginDrawing();
	 ClearBackground(WHITE);

	 DrawTexture(bg,0, 0, WHITE);

	 DrawTexture(player,characterpos.x,characterpos.y,WHITE);

	 EndDrawing();

	  if (IsKeyDown(KEY_RIGHT))
		 characterpos.x += 3;
	 if (IsKeyDown(KEY_LEFT))
		 characterpos.x -= 3; 
	 if (IsKeyDown(KEY_UP))
		 characterpos.y -= 3;
	 if (IsKeyDown(KEY_DOWN))
		 characterpos.y += 3;
		 



 }


}