#include <raylib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#pragma comment(lib, "Winmm.lib")

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 800

#define PLANE_SIZE 50

#define ENEMY_NUM 8

#define ENEMY_SPEED 1.0

#define BULLET_NUM 10

typedef struct pos
{
	int x;
	int y;
}POS;

typedef struct plane
{
	POS planePos;
	POS planeBullets[BULLET_NUM];
	int bulletlen;
	int bulletSpeed;
}PLANE;

PLANE myPlane;
PLANE enemyPlanes[ENEMY_NUM];
int enemyPlanelen;
static time_t startTime, endTime;
Texture2D img[3];
Sound shootSound;

int score = 0;
bool gameOver = false;

void initGame();
void drawGame();
void drawScore();
void updateGame();
void initEnemyPlanes();
void destroyEnemyPlanes();
void destroyBullets();
bool areColliding(POS pos1, float radius1, POS pos2, float radius2);

int main()
{
	initGame();
	while (!WindowShouldClose()) {
        if (!gameOver) 
        {
            updateGame();
        }
		drawGame();
	}
	UnloadTexture(img[0]);
	UnloadTexture(img[1]);
	UnloadTexture(img[2]);
	CloseWindow();	
	return 0;
}

void initGame()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fighting Plane");
    SetTargetFPS(60);

    score = 0;

    // 背景
    Image background = LoadImage("img/background.png");
    ImageResize(&background, SCREEN_WIDTH, SCREEN_HEIGHT);
    img[0] = LoadTextureFromImage(background);
    UnloadImage(background);

    // 敌机
    Image enemy = LoadImage("img/enemy.png");
    ImageResize(&enemy, PLANE_SIZE, PLANE_SIZE);
    img[1] = LoadTextureFromImage(enemy);
    UnloadImage(enemy);

    // 玩家飞机
    Image player = LoadImage("img/plane.png");

    // 把白色背景变透明
    ImageColorReplace(&player, WHITE, BLANK);

    // 缩放到 50 × 50
    ImageResize(&player, PLANE_SIZE, PLANE_SIZE);

    img[2] = LoadTextureFromImage(player);
    UnloadImage(player);

    srand((unsigned int)time(NULL));

    myPlane.bulletlen = 0;
    myPlane.bulletSpeed = 3;

    myPlane.planePos = (POS){
        SCREEN_WIDTH / 2 - PLANE_SIZE / 2,
        SCREEN_HEIGHT - PLANE_SIZE
    };

    enemyPlanelen = 0;

    startTime = time(NULL);
}

void drawGame()
{
	BeginDrawing();
	DrawTexture(img[0], 0, 0, WHITE);
	DrawTexture(img[2], myPlane.planePos.x - PLANE_SIZE / 2, myPlane.planePos.y - PLANE_SIZE / 2, WHITE);

	for (int i = 0; i < enemyPlanelen; i++)
	{
		DrawTexture(img[1], enemyPlanes[i].planePos.x - PLANE_SIZE / 2, enemyPlanes[i].planePos.y - PLANE_SIZE / 2, WHITE);
	}

    for (int i = 0; i < myPlane.bulletlen; i++)
    {
		DrawCircleV((Vector2) { myPlane.planeBullets[i].x, myPlane.planeBullets[i].y }, PLANE_SIZE / 4, BLACK);
    }
    
    if (gameOver)
    {
        DrawRectangle(
            0, 0,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            (Color) {
            0, 0, 0, 150
        }
        );

        const char* text = "GAME OVER";
        int fontSize = 30;

        DrawText(
            text,
            (SCREEN_WIDTH - MeasureText(text, fontSize)) / 2,
            SCREEN_HEIGHT / 2,
            fontSize,
            RED
        );
    }
    
    drawScore();
	EndDrawing();
}

void drawScore()
{
    const char* text = TextFormat("Score: %d", score);

    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);

    DrawText(
        text,
        (SCREEN_WIDTH - textWidth) / 2,
        PLANE_SIZE,
        fontSize,
        BLACK
    );
}

void updateGame()
{
    // 玩家飞机移动
    if ((IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) && myPlane.planePos.x > PLANE_SIZE / 2)
    {
        myPlane.planePos.x -= 4;
    }
    if ((IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) && myPlane.planePos.x < SCREEN_WIDTH - PLANE_SIZE / 2)
    {
        myPlane.planePos.x += 4;
    }
    if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) && myPlane.planePos.y > PLANE_SIZE / 2)
    {
        myPlane.planePos.y -= 4;
    }
    if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && myPlane.planePos.y < SCREEN_HEIGHT - PLANE_SIZE / 2)
    {
        myPlane.planePos.y += 4;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (myPlane.bulletlen < BULLET_NUM)
        {
            myPlane.planeBullets[myPlane.bulletlen] = myPlane.planePos;
            myPlane.bulletlen++;
        }
        // 发射子弹
    }
    for (int i = 0; i < enemyPlanelen; i++)
    {
		enemyPlanes[i].planePos.y += ENEMY_SPEED;
    }

    for (int i = 0; i < myPlane.bulletlen; i++) 
    {
		myPlane.planeBullets[i].y -= myPlane.bulletSpeed;
    }
    initEnemyPlanes();
    destroyEnemyPlanes();
    destroyBullets();
}

void initEnemyPlanes()
{
    endTime = time(NULL);
	double elapsedTime = difftime(endTime, startTime);
    if (elapsedTime >= ENEMY_SPEED) 
    {
		int x = (rand() % (SCREEN_WIDTH - 2 * PLANE_SIZE)) + PLANE_SIZE;
		int y = -PLANE_SIZE;

        enemyPlanes[enemyPlanelen].planePos = (POS){ x, y };
        
        enemyPlanelen++;
    }
	startTime = endTime;
}

void destroyEnemyPlanes()
{
	for (int i = 0; i < enemyPlanelen; i++)
	{
        if(areColliding(myPlane.planePos, PLANE_SIZE / 2, enemyPlanes[i].planePos, PLANE_SIZE / 2))
        {
			gameOver = true;
            break;
        }
        if (enemyPlanes[i].planePos.y > SCREEN_HEIGHT + PLANE_SIZE)
		{
			for (int j = i; j < enemyPlanelen - 1; j++)
			{
				enemyPlanes[j] = enemyPlanes[j + 1];
			}
			enemyPlanelen--;
			i--;
		}
	}
}

void destroyBullets(void)
{
    for (int i = 0; i < myPlane.bulletlen;)
    {
        bool bulletRemoved = false;

        // 检查子弹是否击中敌机
        for (int j = 0; j < enemyPlanelen; j++)
        {
            if (areColliding(
                myPlane.planeBullets[i],
                PLANE_SIZE / 4.0f,
                enemyPlanes[j].planePos,
                PLANE_SIZE / 3.0f))
            {
                // 删除子弹
                for (int x = i; x < myPlane.bulletlen - 1; x++)
                {
                    myPlane.planeBullets[x] =
                        myPlane.planeBullets[x + 1];
                }

                myPlane.bulletlen--;

                // 删除敌机
                for (int x = j; x < enemyPlanelen - 1; x++)
                {
                    enemyPlanes[x] =
                        enemyPlanes[x + 1];
                }

                enemyPlanelen--;

                score += 100;

                bulletRemoved = true;
                break;
            }
        }

        if (bulletRemoved)
        {
            continue;
        }

        // 子弹飞出屏幕
        if (myPlane.planeBullets[i].y < 0)
        {
            for (int x = i; x < myPlane.bulletlen - 1; x++)
            {
                myPlane.planeBullets[x] =
                    myPlane.planeBullets[x + 1];
            }

            myPlane.bulletlen--;

            continue;
        }

        // 只有子弹没有被删除，才看下一颗
        i++;
    }
}

bool areColliding(POS pos1, float radius1, POS pos2, float radius2)
{
	float dx = pos1.x - pos2.x;
	float dy = pos1.y - pos2.y;
	float distance = sqrt(dx * dx + dy * dy);
	return distance < (radius1 + radius2);
}