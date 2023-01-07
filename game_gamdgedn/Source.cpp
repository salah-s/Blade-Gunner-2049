#include <raylib.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAX_ENEMIES 200
#define MAX_BULLETS 20

// A function that generates a random number between a min dist from the player and a max dist from the player
float GetRandomSafeValue(float playerPos, float minDist, float maxDist) {

	int distance = GetRandomValue(minDist, maxDist);

	/* 
		Randomly decide whether to add distance to or subtract distance from player's position.
		This allows spawning in all directions(right/left/top/bottom).
	*/
	
	return GetRandomValue(0, 1) ? playerPos + distance : playerPos - distance;
	
	/* ^ Same as:
	
		int decision = GetRandomValue(0, 1);

		if(decision == 0)
			return playerPos + distance;
		else if(decision == 1)
			return playerPos - distance;
			*/
}

//  define a timer 
struct Timer {
	float Lifetime;
};

// a fucntion to start a timer with specific lifetime
void StartTimer(Timer* timer, float lifetime) {
	if (timer != NULL)
		timer->Lifetime = lifetime;
}
// update a timer with the current frame time
void UpdateTimer(Timer* timer) {
	if (timer != NULL && timer->Lifetime > 0) // subtract this frame from the timer if the timer is not already expired
		timer->Lifetime -= GetFrameTime();
}
//check if the timer is done
bool isTimerDone(Timer* timer) {
	if (timer != NULL)
		return timer->Lifetime <= 0; //if timer's lifetime is less than or equal 0 ,return 1      ,otherwise return 0
}

struct Enemy { 

	Texture2D texture;
	Vector2 position;
	Vector2 direction;
	Rectangle EnemyHitbox;
	int hyp;
	bool active;
	int enemySpeed;
	
};
struct Bullet {
	Texture2D texture;
	Vector2 position;
	Vector2 direction;
	Rectangle BulletHitbox;
	bool active;
	int bulletspeed;
};

void main() {

	// Initializing window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game title");
	SetTargetFPS(60);

	// Loading textures
	Texture2D bg = LoadTexture("resources/bg.png");
	Texture2D playerTex = LoadTexture("resources/p1r.png");
	Texture2D enemyTex = LoadTexture("resources/mob.png");

	// Vectors
	Vector2 playerPos = { 0, 0 };
	Vector2 enemyPos;

	// Rectangles
	Rectangle PlayerHitbox = { 00,00,20,45 };

	// Variables
	int playerSpeed = 3;
	int playerHealth = 10;



	int enemyTimer = 0;
	float enemySpawnRate = 1.0f;

	float flashingDuration = 2.0f;

	bool collided = false;
	bool gameOver = false;

	// Initiallizing camera
	Camera2D playerCam = { 0 };

	playerCam.target = playerPos;
	playerCam.offset = { (SCREEN_WIDTH / 2.0f) - (playerTex.width / 2), (SCREEN_HEIGHT / 2.0f) - (playerTex.height / 2) };
	playerCam.rotation = 0.0f;
	playerCam.zoom = 1.4f;

	// Initiallizing enemies

	Enemy enemies[MAX_ENEMIES] = { 0 };     // An array of "Enemy(s)"

	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		enemies[i].position = { Vector2{0,0} };
		enemies[i].active = false;
		enemies[i].EnemyHitbox.width = 26;
		enemies[i].EnemyHitbox.height = 26;
		enemies[i].enemySpeed = 2;
	}

	// intializing bullets
	Bullet bullets[MAX_BULLETS] = { 0 };   // An array of "Bullet(s)"
	for (int i = 0; i < MAX_BULLETS; i++) {
		bullets[i].position = Vector2{ 0,0 };
		bullets[i].active = false;
		bullets[i].BulletHitbox.width = 5;
		bullets[i].BulletHitbox.height = 5;
		bullets[i].bulletspeed = 4;

	}


	//initiallizing timers
	Timer flashingTimer;
	flashingTimer.Lifetime = 0.0f;

	// GAME LOOP
	while (!WindowShouldClose()) {

		// Toggling fullscreen mode
		if (IsKeyPressed(KEY_F))
			ToggleFullscreen();

		if (!gameOver)
		{


			//update playerhitbox pos
			PlayerHitbox.x = playerPos.x + 20;
			PlayerHitbox.y = playerPos.y + 5;

			playerCam.target = { playerPos.x + 20, playerPos.y + 20 };

			// Player movement
			if (IsKeyDown(KEY_RIGHT))
				playerPos.x += playerSpeed;

			if (IsKeyDown(KEY_LEFT))
				playerPos.x -= playerSpeed;

			if (IsKeyDown(KEY_UP))
				playerPos.y -= playerSpeed;

			if (IsKeyDown(KEY_DOWN))
				playerPos.y += playerSpeed;

			// Activating enemies and giving them positions
			enemyTimer++; // Number of frames

			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (!enemies[i].active && (enemyTimer % (int)(60 / enemySpawnRate)) == 0) {

					enemies[i].active = true;
					enemies[i].position.x = GetRandomSafeValue(playerPos.x, 200, 300);
					enemies[i].position.y = GetRandomSafeValue(playerPos.y, 200, 300);
					enemies[i].EnemyHitbox.x = enemies[i].position.x;
					enemies[i].EnemyHitbox.y = enemies[i].position.y;

					break;
				}
			}

			// Update enemies position to follow player
			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (enemies[i].active) {
					// Calculate the direction in which to follow player: player position - enemy position
					enemies[i].direction.x = (playerPos.x - enemies[i].position.x);
					enemies[i].direction.y = (playerPos.y - enemies[i].position.y);

					// Normalizing the direction vector (to move with the same speed diagonally)
					enemies[i].hyp = sqrt(enemies[i].direction.x * enemies[i].direction.x + enemies[i].direction.y * enemies[i].direction.y);
					if (enemies[i].hyp != 0) {
						enemies[i].direction.x /= enemies[i].hyp;
						enemies[i].direction.y /= enemies[i].hyp;
					}
					// Update the enemy's position (movement)
					enemies[i].position.x += enemies[i].direction.x * 1;
					enemies[i].position.y += enemies[i].direction.y * 1;

					enemies[i].EnemyHitbox.x = enemies[i].position.x + 15;
					enemies[i].EnemyHitbox.y = enemies[i].position.y + 10;

					// Check collision between player and enemy
					if (CheckCollisionRecs(PlayerHitbox, enemies[i].EnemyHitbox) && isTimerDone(&flashingTimer) && playerHealth > 0)
					{
						collided = true;
						playerHealth--;
						StartTimer(&flashingTimer, flashingDuration);
					}

				}
			}
			UpdateTimer(&flashingTimer);

			if (collided && isTimerDone(&flashingTimer))
				collided = false;

			if (playerHealth <= 0)
				gameOver = true;
		}

		//activate bullets
		for (int i = 0;i < MAX_BULLETS;i++) {
			if (!bullets[i].active && IsKeyPressed(MOUSE_BUTTON_LEFT)) {
				bullets[i].active = true;
				bullets[i].position = playerPos;
				bullets[i].BulletHitbox.x = bullets[i].position.x;
				bullets[i].BulletHitbox.y = bullets[i].position.y;
			break;
			}
		}

		//update bullets
		for (int i = 0;i < MAX_BULLETS;i++) {
			if (bullets[i].active) {
				bullets[i].direction.x = GetMousePosition().x - playerPos.x;
				bullets[i].direction.y = GetMousePosition().y - playerPos.y;

				bullets[i].position.x += bullets[i].direction.x * bullets[i].bulletspeed;
				bullets[i].position.y += bullets[i].direction.y * bullets[i].bulletspeed;
			}



			// DRAWING
			BeginDrawing();
			BeginMode2D(playerCam); // Showing camera

			ClearBackground(WHITE);

			DrawTexture(bg, 0, 0, WHITE);
			DrawTexture(playerTex, playerPos.x, playerPos.y, WHITE);

			//DrawRectangleRec(PlayerHitbox, BLUE);

			// Drawing active enemies
			for (int i = 0; i < MAX_ENEMIES; i++)
			{
				if (enemies[i].active) {
					DrawTexture(enemyTex, enemies[i].position.x, enemies[i].position.y, WHITE);
					//DrawRectangleRec(enemies[i].EnemyHitbox, RED);  
				}
			}
			//draw bullets
			for (int i = 0; i < MAX_BULLETS; i++)
			{
				if (bullets[i].active) {
					DrawRectangleRec(bullets[i].BulletHitbox, RED);

				}
			}

			//DrawRectangleRec(enemies[i].EnemyHitbox, RED);  

			//other drawings
			DrawText(TextFormat("Player's Health: %d", playerHealth), 10, 10, 24, BLACK);

			if (gameOver)
				DrawText("GAME OVER", 100, 100, 42, BLACK);

			if (collided)
				DrawText("colision", 50, 50, 42, BLACK);
			else
				DrawText("no colision", 50, 50, 42, BLACK);
			if (!(flashingTimer.Lifetime <= 0.0))
				DrawText("timer is running!", 50, 10, 42, BLACK);
			else
				DrawText("timer is not running", 50, 10, 42, BLACK);

			EndDrawing();

		}
	}
}