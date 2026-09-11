#include <raylib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <Windows.h>
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

int main()
{
	InitWindow(800, 600, "Fighting Plane");
	while (!WindowShouldClose()) {

	}
	CloseWindow("Fighting Plane");
	return 0;
}