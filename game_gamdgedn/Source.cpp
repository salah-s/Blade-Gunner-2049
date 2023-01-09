#include <raylib.h>
#include <stdio.h>
#include<stdlib.h>
#include <raymath.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define MAX_ENEMIES 100
#define MAX_BULLETS 30

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

//  timer struct 
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
	int health;

	float enemyFrametimer;
	int enemyframe;
	
};
struct Bullet {
	Texture2D texture;
	Rectangle BulletHitbox;

	Vector2 position;
	Vector2 direction;

	Timer bullettimer;
	float bulletlife;

	Vector2 targetpos;
	bool active;

	

};



void main() {

	// Initializing window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game title");
	SetTargetFPS(60);

	//audio
	InitAudioDevice();
	
	FILE* scores;

	// Loading textures
	Texture2D bg = LoadTexture("resources/bg2.png");
	Texture2D bg2 = LoadTexture("resources/bgfinal.png");
	Texture2D playerTex = LoadTexture("resources/p1r.png");
	Texture2D enemyTex = LoadTexture("resources/mob.png");
	Texture2D mcTex = LoadTexture("resources/mc_walk_with_gun.png");
	Texture2D enemy1tex = LoadTexture("resources/enemy_2.png");
	Texture2D bullet1tex = LoadTexture("resources/bullet3s.png");
	Texture2D heart = LoadTexture("resources/heart5.png");

	//loading audio
	SetMasterVolume(0.75f);

	Sound shootingsound = LoadSound("resources/shoot.wav");
	SetSoundVolume(shootingsound, 0.3f);
	Sound enemyhurt = LoadSound("resources/enemyhurt.ogg");
	SetSoundVolume(enemyhurt, 0.3f);
	Sound playerhurt = LoadSound("resources/playerhurt.mp3");
		SetSoundVolume(playerhurt, 1.0f);
	Music backgroundmusic = LoadMusicStream("resources/musicc.mp3");
	PlayMusicStream(backgroundmusic);
	SetMusicVolume(backgroundmusic, 0.2f);
	
	// 
	// Vectors
	Vector2 playerPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
	Vector2 GunPos = Vector2Add(playerPos, Vector2{ 20,20 });
	Vector2 enemyPos;

	// Rectangles
	Rectangle PlayerHitbox = { 00,00,20,45 };
	Rectangle PlayerAnimationRec;
	Rectangle EnemyAnimationRec;

	Rectangle borderup = { 200,100,1400,100 }; //borders
	Rectangle borderdown = { 300,825,1400,100 };
	Rectangle borderleft = { 290,200,100,800 };
	Rectangle borderright = { 1510,200,100,800 };
	
	// Variables
	int playerSpeed = 3;
	int playerHealth = 3;
	int bulletspeed = 5;

	// enemy timer (old timer system)
	int enemyTimer = 0;
	float enemySpawnRate = 1.0f;

	int currentScore = 200;
	int highScore = 0;

	float flashingDuration = 2.0f;

	bool collided = false;
	bool gameOver = false;

	// Initiallizing camera
	Camera2D playerCam = { 0 };

	playerCam.target = playerPos;
	playerCam.offset = { (SCREEN_WIDTH / 2.0f) - (playerTex.width / 2), (SCREEN_HEIGHT / 2.0f) - (playerTex.height / 2) };
	playerCam.rotation = 0.0f;
	playerCam.zoom = 3.5f;

	// Initiallizing enemies
	//Enemy enemies[MAX_ENEMIES] = { 0 };     // An array of "Enemy(s)"
	Enemy* enemies = (Enemy*)malloc(MAX_ENEMIES * sizeof(Enemy));

	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		enemies[i].position = { Vector2{0,0} };
		enemies[i].active = false;
		enemies[i].EnemyHitbox.width = 26;
		enemies[i].EnemyHitbox.height = 26;
		enemies[i].enemySpeed = 2;
		enemies[i].health = 2;
		enemies[i].enemyFrametimer = 0.0f;
		enemies[i].enemyframe = 0;
	}

	// intializing bullets
	// Bullet bullets[MAX_BULLETS] = { 0 };   // An array of "Bullet(s)"
	Bullet* bullets = (Bullet*)malloc(MAX_BULLETS * sizeof(Bullet));
	for (int i = 0; i < MAX_BULLETS; i++) {
		bullets[i].position = Vector2{ 0,0 };


		bullets[i].BulletHitbox.width = 10;
		bullets[i].BulletHitbox.height = 10;

		//initializing bullet timer
		bullets[i].bullettimer;
		bullets[i].bulletlife = 2.0f;
	}


	//initiallizing timers

	Timer flashingTimer;
	flashingTimer.Lifetime = 0.0f;
	Timer firerateTimer;
	firerateTimer.Lifetime = 0.0f;
	float firerate = 0.4;

	//animation related variables
	float mcFramewidth = (float)(mcTex.width / 5); //player animation
	int mcMaxFrames = (mcTex.width / (int)mcFramewidth);
	Timer mcframetimer;
	float mcFrameDuration = 0.2f;
	int mcframe = 0;


	float enemyFramewidth = (float)(mcTex.width / 5); //enemy animation
	int enemyMaxFrame = (enemy1tex.width / (int)enemyFramewidth);

	PlayerAnimationRec.width = mcFramewidth;
	PlayerAnimationRec.height = mcTex.height;
	PlayerAnimationRec.x = 0;
	PlayerAnimationRec.y = 0;

	// Read highscore from file
	scores = fopen("resources/Scores.txt", "r");
	if (scores != NULL)
	{
		fscanf_s(scores, "%d", &highScore);
	}
	fclose(scores);

	//***********************************************************************************************************************
	// GAME LOOP**************************************************************************************************************
	while (!WindowShouldClose()) {
		UpdateMusicStream(backgroundmusic);
		// Toggling fullscreen mode
		if (IsKeyPressed(KEY_F))
			ToggleFullscreen();

		

		if (!gameOver)
		{
		
			//update playerhitbox pos
			PlayerHitbox.x = playerPos.x + 20;
			PlayerHitbox.y = playerPos.y + 5;

			playerCam.target = { playerPos.x + 20, playerPos.y + 20 };
			GunPos = Vector2Add(playerPos, Vector2{ 20,20 });



			if (isTimerDone(&mcframetimer))
			{
				StartTimer(&mcframetimer, mcFrameDuration);
				mcframe++;
				PlayerAnimationRec.x = mcframe * mcFramewidth;
			}

			UpdateTimer(&mcframetimer);

			// Player movement
			if (IsKeyDown(KEY_D)&& !CheckCollisionRecs(PlayerHitbox,borderright) ) {
				playerPos.x += playerSpeed;
				PlayerAnimationRec.width = mcFramewidth;
			}

			if (IsKeyDown(KEY_A) && !CheckCollisionRecs(PlayerHitbox, borderleft)) {
				playerPos.x -= playerSpeed;
				PlayerAnimationRec.width = -(mcFramewidth);
			}

			if (IsKeyDown(KEY_W) && !CheckCollisionRecs(PlayerHitbox,borderup))
				playerPos.y -= playerSpeed;

			if (IsKeyDown(KEY_S) && !CheckCollisionRecs(PlayerHitbox, borderdown))
				playerPos.y += playerSpeed;



			//activate bullets
			for (int i = 0; i < MAX_BULLETS; i++) {
				if (isTimerDone(&bullets[i].bullettimer) && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && isTimerDone(&firerateTimer)) {
					bullets[i].active = true;
					PlaySound(shootingsound);

					bullets[i].position = GunPos;
					bullets[i].direction = Vector2Subtract(GetScreenToWorld2D(GetMousePosition(), playerCam), GunPos);

					bullets[i].BulletHitbox.x = GunPos.x;
					bullets[i].BulletHitbox.y = GunPos.y;

					StartTimer(&bullets[i].bullettimer, bullets[i].bulletlife); // a bullet is active if its bullet timer hasn't run out yet
					StartTimer(&firerateTimer, firerate); // the firerate timer determines the minimum  time between each bullet
					break;
				}
			}

			UpdateTimer(&firerateTimer);

			//update bullets
			for (int i = 0; i < MAX_BULLETS; i++) {
				UpdateTimer(&bullets[i].bullettimer);
				if (!isTimerDone(&bullets[i].bullettimer) && bullets[i].active == true) {

					bullets[i].position = Vector2Add(bullets[i].position, Vector2Scale(bullets[i].direction, GetFrameTime() * bulletspeed));

					bullets[i].BulletHitbox.x = bullets[i].position.x;
					bullets[i].BulletHitbox.y = bullets[i].position.y;

					//checking collision between 1)bullet and 2)enemies
					for (int j = 0; j < MAX_ENEMIES; j++) {
						if (CheckCollisionRecs(bullets[i].BulletHitbox, enemies[j].EnemyHitbox) && enemies[j].active == true && bullets[i].active == true) {
						
							enemies[j].health--;
							bullets[i].active = false;

							//throwing the bullet away so it disapears (not necessary for the hitbox but necessary for the texture)
							bullets[i].BulletHitbox.x = SCREEN_WIDTH;
							bullets[i].BulletHitbox.y = SCREEN_HEIGHT;

						}
					}
				}
			}

			// Activating enemies and giving them positions
			enemyTimer++; // Number of frames

			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (!enemies[i].active && (enemyTimer % (int)(60 / enemySpawnRate)) == 0) {

					enemies[i].active = true;
					enemies[i].position.x = GetRandomSafeValue(playerPos.x, 200, 300);
					enemies[i].position.y = GetRandomSafeValue(playerPos.y, 200, 300);
					enemies[i].EnemyHitbox.x = enemies[i].position.x;
					enemies[i].EnemyHitbox.y = enemies[i].position.y;
					enemies[i].health = 2;
					break;
				}
			}


			// Update enemies position to follow player
			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (enemies[i].active) {
					// Calculate the direction in which to follow player: player position - enemy position

					enemies[i].direction = Vector2Subtract(playerPos, enemies[i].position);

					//Normalizing the direction 
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

					// Check collision between enemy and player 
					if (CheckCollisionRecs(PlayerHitbox, enemies[i].EnemyHitbox) && isTimerDone(&flashingTimer) && playerHealth > 0)
					{
						collided = true;
						playerHealth--;
						PlaySound(playerhurt);
						StartTimer(&flashingTimer, flashingDuration);
					}

					if (enemies[i].health == 0) {//enemy's death condition
						PlaySound(enemyhurt);
						enemies[i].active = false;
					}
				}
			}

			UpdateTimer(&flashingTimer);
			if (collided && isTimerDone(&flashingTimer))
				collided = false;
			if (playerHealth <= 0)
				gameOver = true;
		}

		// Save new highscore
		if (currentScore > highScore)
		{
			scores = fopen("resources/Scores.txt", "w");
			highScore = currentScore;
			fprintf(scores, "%d", highScore);
		}
	
	fclose(scores);

	// DRAWING
	BeginDrawing();
	BeginMode2D(playerCam); // Showing camera

	DrawRectangle(-100, -100, 20, 20, RED);

	ClearBackground(WHITE);
	//drawing background
	DrawTexture(bg2, 0, 0, WHITE);
	//DrawTexture(bg, 0, 0, WHITE);

		//DrawRectangleRec(PlayerHitbox, BLUE);

	DrawTextureRec(mcTex, PlayerAnimationRec, playerPos, WHITE);


	// Drawing active enemies
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		if (enemies[i].active) {

			//drawing enemy animation
			enemies[i].enemyFrametimer += GetFrameTime();
			if (enemies[i].enemyFrametimer >= 0.3f) {
				enemies[i].enemyFrametimer = 0;
				enemies[i].enemyframe++;
			}
			enemies[i].enemyframe = enemies[i].enemyframe % enemyMaxFrame;

			DrawTextureRec(enemy1tex,
				Rectangle{ (enemyFramewidth * enemies[i].enemyframe),0,enemyFramewidth,(float)enemy1tex.height }
			, enemies[i].position, WHITE);

			// sssssssDrawRectangleRec(enemies[i].EnemyHitbox, RED);
		}
	}
	//draw bullets
	for (int i = 0; i < MAX_BULLETS; i++) {

		if (!isTimerDone(&bullets[i].bullettimer)) {
			//DrawCircleV(bullets[i].position, 10, RED);
				//DrawRectangleRec(bullets[i].BulletHitbox,GREEN);
			DrawTexture(bullet1tex, bullets[i].BulletHitbox.x, bullets[i].BulletHitbox.y, WHITE);
		}
	}



	
	//draw health
	if (playerHealth >=1) {
		DrawTextureV(heart, GetScreenToWorld2D(Vector2{ 0,20 }, playerCam), WHITE);
		if (playerHealth >=2) {
			DrawTextureV(heart, GetScreenToWorld2D(Vector2{ 190,20 }, playerCam), WHITE);
			if(playerHealth==3)
			DrawTextureV(heart, GetScreenToWorld2D(Vector2{ 380,20 }, playerCam), WHITE);
		}
	}
	


	//drawing borders
	//DrawRectangleRec(borderup,RED);               //up
	//DrawRectangleRec(borderdown,RED);                //down
	//DrawRectangleRec(borderleft,BLUE);                //left  
	//DrawRectangleRec(borderright,BLUE);                //right
	//other drawings
	if (gameOver)
		DrawText("GAME OVER", playerPos.x - 220, playerPos.y, 80, WHITE);


	if (!(flashingTimer.Lifetime <= 0.0) && playerHealth != 0)
		DrawText("INVINCIBLE", playerPos.x - 30, playerPos.y, 20, WHITE);
	else
		DrawText("currently not invincible", 50, 40, 20, BLACK);

	EndDrawing();

	}
	CloseAudioDevice();
}
