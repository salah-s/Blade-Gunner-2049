#include <raylib.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAX_ENEMIES 200

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

struct Timer
{
	float LifeTime;
};

void StartTimer(Timer* timer, float lifeTime)
{
	if (timer != NULL)
		timer->LifeTime = lifeTime;
}

float UpdateTimer(Timer* timer)
{
	if (timer != NULL && timer->LifeTime > 0)
		return timer->LifeTime -= GetFrameTime();
}

bool isTimerDone(Timer* timer)
{
	if (timer != NULL)
		return timer->LifeTime <= 0;
}

struct Enemy { 

	Texture2D texture;
	Vector2 position;
	Vector2 direction;
	Rectangle EnemyHitbox;
	int hyp;
	bool active;
	
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
	Rectangle PlayerHitbox = { 00,00,20,45};

	// Variables
	int playerSpeed = 3;
	int playerHealth = 10;
	int enemySpeed = 2;
	int enemyTimer = 0;
	float enemySpawnRate = 1.0f;
	float flashingDuration = 2.0f;

	bool collided = false;

	Timer flashingTimer;
	flashingTimer.LifeTime = 0.0f;

	// Initializing camera
	Camera2D playerCam = { 0 };

	playerCam.target = playerPos;
	playerCam.offset = { (SCREEN_WIDTH / 2.0f) - (playerTex.width / 2), (SCREEN_HEIGHT / 2.0f) - (playerTex.height / 2) };
	playerCam.rotation = 0.0f;
	playerCam.zoom = 1.4f;

	// An array of "Enemy(s)"
	Enemy enemies[MAX_ENEMIES] = { 0 };

	// Intitalizing enemies
	for (int i = 0; i < MAX_ENEMIES; i++)
	{

		enemies[i].position = { Vector2{0,0} };
		enemies[i].active = false;
		enemies[i].EnemyHitbox.width = 26;
		enemies[i].EnemyHitbox.height = 26;
	}

	// GAME LOOP
	while (!WindowShouldClose()) {

		// Toggling fullscreen mode
		if (IsKeyPressed(KEY_F))
			ToggleFullscreen();

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
				enemies[i].direction.x /= enemies[i].hyp;
				enemies[i].direction.y /= enemies[i].hyp;

				// Update the enemy's position (movement)
				enemies[i].position.x += enemies[i].direction.x * 1;
				enemies[i].position.y += enemies[i].direction.y * 1;

				enemies[i].EnemyHitbox.x = enemies[i].position.x + 15;
				enemies[i].EnemyHitbox.y = enemies[i].position.y + 10;

				// Check collision between player and enemy
				if (CheckCollisionRecs(PlayerHitbox, enemies[i].EnemyHitbox) && isTimerDone(&flashingTimer))
				{
					collided = true;
					playerHealth--;
					StartTime(&flashingTimer, flashingDuration);
				}
			}
		}

		UpdateTimer(&flashingTimer);

		// If collided, decrease the player's health
		if (collided && isTimerDone(&flashingTimer))
		{
			collided = false;
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

		DrawText(TextFormat("Player's Health: %d", playerHealth), 10, 10, 24, BLACK);

		if(collided)
			DrawText("collision",10, 25, 24, BLACK);

		if(flashing)
			DrawText("flashing", 10, 50, 24, BLACK);

		EndDrawing();

	}
}