
/* #includes left out */
#include <SDL\SDL.h>
#include <GL\glew.h>
#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <stdbool.h>

#define VIEW_WIDTH 640 
#define VIEW_HEIGHT 480
#define TILE_WIDTH_X 100
#define TILE_HEIGHT_Y 66
#define HALF_TILE_WIDTH 50
#define HALF_TILE_HEIGHT 33
#define TREE_HEIGHT 44
#define TREE_WIDTH 11
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

char shouldExit = 0;

typedef struct AnimFrameDef {
	// combined with the AnimDef's name to make
	// the actual texture name
	int frameNum;
	float frameTime;
}AnimFrameDef;


typedef struct AnimDef {
	const char* name;
	struct AnimFrameDef frames[20];
	int numFrames;
} AnimDef;

// Define a single frame used in an animation

// Runtime state for an animation
typedef struct AnimData {
	struct AnimDef* def;
	int curFrame;
	float timeToNextFrame;
	bool isPlaying;
} AnimData;



void animTick(AnimData* data, float dt)
{
	if (!data->isPlaying) {
		return;
	}
	int numFrames = data->def->numFrames; //instead of frame.size(), use numFrames? 
	data->timeToNextFrame -= dt;
	if (data->timeToNextFrame < 0) {
		++data->curFrame;
		if (data->curFrame >= numFrames) {
			// end of the animation, stop it
			data->curFrame = numFrames - 1;
			data->timeToNextFrame = 0;
			data->isPlaying = false;
		}
		else {
			AnimFrameDef* curFrame = &data->def->frames[data->curFrame]; //The name of an array i === to the pointer to the first element in an array also ===
			data->timeToNextFrame += curFrame->frameTime;
		}
	}
}

void animSet(AnimData* anim, AnimDef* toPlay)
{
	anim->def = toPlay;
	anim->curFrame = 0;
	anim->timeToNextFrame = anim->def->frames[0].frameTime;
	anim->isPlaying = true;
}

void animReset(AnimData* anim)
{
	animSet(anim, anim->def);
}

void animDraw(AnimData* anim, GLuint textures[], int x, int y, int w, int h)
{
	int curFrameNum = anim->def->frames[anim->curFrame].frameNum;
	//printf("%d", anim->def->frames[anim->curFrame].frameNum);
	GLuint tex = textures[curFrameNum];
	glDrawSprite(tex, x, y, w, h);
}

typedef struct AABB {
	int x, y, w, h;
} AABB;

/*Struct for Player*/
typedef struct Player
{
	AnimData data;
	AnimDef def;
	float posX;
	float posY;
	AABB box;

} Player;

typedef struct enemyNPC
{
	AnimData data;
	AnimDef def;
	float posX;
	float posY;
	AABB box;

} enemyNPC;

/*Struct for FirstAid Box*/
typedef struct FirstAid
{
	struct AnimData data;
	struct AnimDef def;
	float posX;
	float posY;
	bool collided;
	AABB box;

} FirstAid;

/*Struct for Camera*/
typedef struct Camera
{
	float posX;
	float posY;
} Camera;

typedef struct Tile{
	int typeSet;
	bool collided;
	AABB box;
} Tile;

typedef struct standardTree{
	AABB box;
} standardTree;




/*AABB Intersect Code*/
bool AABBIntersect(const AABB* box1, const AABB* box2)
{
	// box1 to the right
	if (box1->x > box2->x + box2->w) {
		return false;
	}
	// box1 to the left
	if (box1->x + box1->w < box2->x) {
		return false;
	}
	// box1 below
	if (box1->y > box2->y + box2->h) {
		return false;
	}
	// box1 above
	if (box1->y + box1->h < box2->y) {
		return false;
	}
	return true;
}




/*Updates on enemy positional data*/
void enemyUpdate(enemyNPC* e, Player* p, float dt)
{
	//printf("I got called!\n");
	//printf("PosX is %f\n", p->posX);

	
	//animUpdate(e->data, dt);
	/* Pathfind to target, choose new target */
	/* when needed */
	/*if (e->posX == e->targetX && e->y == e->targetY) {
	requestNewTarget(e);
	}*/

	float ESPEED = 0.00000000001;

	float deltaX;
	float deltaY;
	if ((e->posX != p->posX) && (e->posY != p->posY)){
		if (e->posX < p->posX) {
			deltaX = min(ESPEED, p->posX - e->posX);

		}
		else {
			deltaX = -min(ESPEED, p->posX - e->posX);
		}
		if (e->posY < p->posY) {
			deltaY = min(ESPEED, p->posX - e->posX);
		}
		else {
			deltaY = -min(ESPEED, p->posX - e->posX);
		}
	}

	

	/* Actually move! */
	e->posX += deltaX * dt;
	e->posY += deltaY * dt;

	printf("enemy posX is currently %f\n", e->posX);
	printf("enemy posY is currently %f\n", e->posY);
	printf("deltaTime is %f", dt);

}



int main(void)

{
	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return 1;
	}
	/* Create the window, OpenGL context */
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Window* window = SDL_CreateWindow(
		"TestSDL",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		640, 480,
		SDL_WINDOW_OPENGL);
	if (!window) {
		fprintf(stderr, "Could not create window.ErrorCode = %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	SDL_GL_CreateContext(window);
	/* Make sure we have a recent version of OpenGL */
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		fprintf(stderr, "Could not initialize glew.ErrorCode = %s\n", glewGetErrorString(glewError));
		SDL_Quit();
		return 1;
	}
	if (!GLEW_VERSION_3_0) {
		fprintf(stderr, "OpenGL max supported version is too low.\n");
		SDL_Quit();
		return 1;
	}

	/* Setup OpenGL state */
	glViewport(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, 640, 480, 0, 0, 100);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	/* The previous frame's keyboard state */
	unsigned char kbPrevState[SDL_NUM_SCANCODES] = { 0 };
	/* The current frame's keyboard state */
	const unsigned char* kbState = NULL;  //use this instead?

	/* Keep a pointer to SDL's internal keyboard state */
	/*Calling this twice? Ask.*/

	kbState = SDL_GetKeyboardState(NULL);

	Tile tileSet[40][40];
	//int x_tile_position = (i + j) * 50;
	//int y_tile_position = 200 + ((-1 - i + j) * 25);
	/*Level Set up*/
	int level[40][40];
	for (int i = 0; i < 40; i++){
		for (int j = 0; j < 40; j++){
			Tile currentTile;
			if (i == 0 || j == 0 || i == 39 || j == 39){
				currentTile.typeSet = 0; //water 
				currentTile.box.x = (i + j) * 50;
				currentTile.box.y = 200 + ((-1 - i + j) * 25);
				currentTile.box.w = TILE_WIDTH_X;
				currentTile.box.h = TILE_HEIGHT_Y;
				currentTile.collided = true;
			}
			else {
				currentTile.typeSet = 1; //ground
				currentTile.box.x = (i + j) * 50;
				currentTile.box.y = 200 + ((-1 - i + j) * 25);
				currentTile.box.w = TILE_WIDTH_X;
				currentTile.box.h = TILE_HEIGHT_Y;
				currentTile.collided = false;
			}
			tileSet[i][j] = currentTile;
		}
	}


	/*Instantation for HealtPack animation*/
	GLuint healthPack[4];
	healthPack[0] = glTexImageTGAFile("Redcross1.tga", NULL, NULL);
	healthPack[1] = glTexImageTGAFile("Redcross2.tga", NULL, NULL);
	healthPack[2] = glTexImageTGAFile("Redcross3.tga", NULL, NULL);
	healthPack[3] = glTexImageTGAFile("Redcross4.tga", NULL, NULL);

	/*instation for position for FirstAid*/
	FirstAid h_pack;
	h_pack.posX = 500;
	h_pack.posY = 100;
	h_pack.box.x = 500;
	h_pack.box.y = 100;
	h_pack.box.w = 50;
	h_pack.box.h = 50;
	h_pack.collided = false;
	h_pack.def.numFrames = 4;
	h_pack.def.name = "health pack";
	h_pack.data.def = &h_pack.def;
	h_pack.data.timeToNextFrame = 3;
	h_pack.data.isPlaying = true;

	/*Assigning Frame numbers*/
	for (int i = 0; i < 4; i++){
		h_pack.def.frames[i].frameNum = i;
		h_pack.def.frames[i].frameTime = 1;
	}

	/*Repeated Instatiation for First Aid*/
	FirstAid h_pack2;
	h_pack2.posX = 200;
	h_pack2.posY = 100;
	h_pack2.box.x = 200;
	h_pack2.box.y = 100;
	h_pack2.box.w = 50;
	h_pack2.box.h = 50;
	h_pack2.collided = false;
	h_pack2.def.numFrames = 4;
	h_pack2.def.name = "health pack";
	h_pack2.data.def = &h_pack.def;
	h_pack2.data.timeToNextFrame = 3;
	h_pack2.data.isPlaying = true;

	for (int i = 0; i < 4; i++){
		h_pack2.def.frames[i].frameNum = i;
		h_pack2.def.frames[i].frameTime = 1;
	}

	/*Beginning of player Instantiation*/
	GLuint playerTex[4];
	playerTex[0] = glTexImageTGAFile("p_frame1.tga", NULL, NULL);
	playerTex[1] = glTexImageTGAFile("p_frame2.tga", NULL, NULL);
	playerTex[2] = glTexImageTGAFile("p_frame3.tga", NULL, NULL);
	playerTex[3] = glTexImageTGAFile("p_frame4.tga", NULL, NULL);

	/*This makes the player.*/
	Player thePlayer;
	Player* thePlayerP = &thePlayer;
	thePlayer.posX = (VIEW_WIDTH / 2) + 200;
	thePlayer.posY = (VIEW_HEIGHT / 2);
	thePlayer.box.x = thePlayer.posX;
	thePlayer.box.y = thePlayer.posY;
	thePlayer.box.w = TILE_WIDTH_X;
	thePlayer.box.h = TILE_HEIGHT_Y;
	thePlayer.def.numFrames = 4;
	thePlayer.def.name = "player1";
	thePlayer.data.def = &thePlayer.def;
	thePlayer.data.timeToNextFrame = 3;
	thePlayer.data.isPlaying = true;
	float prevPlayerPosX = thePlayer.posX;
	float prevPlayerPosY = thePlayer.posY;

	for (int i = 0; i < 4; i++){
		/*This updates the frameNum and frameTime*/
		thePlayer.def.frames[i].frameNum = i;
		thePlayer.def.frames[i].frameTime = 1;
	}
		
	/*Beginning of enemy NPC Instantiation*/
	GLuint enemyTex[4];
	enemyTex[0] = glTexImageTGAFile("e_frame1.tga", NULL, NULL);
	enemyTex[1] = glTexImageTGAFile("e_frame2.tga", NULL, NULL);
	enemyTex[2] = glTexImageTGAFile("e_frame3.tga", NULL, NULL);
	enemyTex[3] = glTexImageTGAFile("e_frame4.tga", NULL, NULL);

	/*This makes the player.*/
	enemyNPC enemy1;
	enemyNPC* enemy1p = &enemy1;
	enemy1.posX = (VIEW_WIDTH / 2) + 1000;
	enemy1.posY = (VIEW_HEIGHT / 2) + 400;
	enemy1.box.x = enemy1.posX;
	enemy1.box.y = enemy1.posY;
	enemy1.box.w = TILE_WIDTH_X;
	enemy1.box.h = TILE_HEIGHT_Y;
	enemy1.def.numFrames = 4;
	enemy1.def.name = "enemy 1";
	enemy1.data.def = &enemy1.def;
	enemy1.data.timeToNextFrame = 5;
	enemy1.data.isPlaying = true;
	float prevEnemyPosX = enemy1.posX;
	float prevEnemyPosY = enemy1.posY;

	for (int i = 0; i < 4; i++){
		/*This updates the frameNum and frameTime*/
		enemy1.def.frames[i].frameNum = i;
		enemy1.def.frames[i].frameTime = 1;
	}

	standardTree regTree;
	regTree.box.x = 500;
	regTree.box.y = 300;
	regTree.box.h = TREE_HEIGHT;
	regTree.box.w = TREE_WIDTH;

	/*This makes camera */
	Camera theCamera;
	theCamera.posX = 0.0;
	theCamera.posY = 0.0;
	float prevCameraPosX = theCamera.posX;
	float prevCameraPosY = theCamera.posY;


	/* Art to draw */
	GLuint vining;
	GLuint greenSquare;
	GLuint redSquare;
	GLuint IsoGrass; //Isometric Tile Gress. Testing right now for isometric game
	GLuint water;
	GLuint tree;
	/*Drawing Assets*/
	/*greenSquare = glTexImageTGAFile("greenSquare.tga", NULL, NULL);
	redSquare = glTexImageTGAFile("stop.tga", NULL, NULL);
	vining = glTexImageTGAFile("vining.tga", NULL, NULL);*/


	IsoGrass = glTexImageTGAFile("grass.tga", NULL, NULL);
	water = glTexImageTGAFile("water.tga", NULL, NULL);
	tree = glTexImageTGAFile("treeTall.tga", NULL, NULL);

	/*Timing elements*/ //TURN THESE INTO FLOATING POINTS 
	int  start = 0;
	int  oldTime = 0;
	const int FPS = 60;
	float deltaTime = 0.0;


	/* The game loop */
	while (!shouldExit) {
		start = SDL_GetTicks();
		//printf("I was here");
		deltaTime = (float)((start - oldTime) / (float)(1000 / FPS));
		oldTime = start;


		prevPlayerPosX = thePlayer.posX;
		prevPlayerPosY = thePlayer.posY;
		prevCameraPosX = theCamera.posX;
		prevCameraPosY = theCamera.posY;
		prevEnemyPosX = enemy1.posX;
		prevEnemyPosY = enemy1.posY;


		/* kbState is updated by the message pump. Copy over the old state before the pump! */
		memcpy(kbPrevState, kbState, sizeof(kbPrevState));

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// Handle OS message pump
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				shouldExit = 1;
			}
		}

		
		if (kbState[SDL_SCANCODE_RIGHT]){
			thePlayer.posX += 1 * deltaTime;
			thePlayer.box.x = thePlayer.posX;
		}

		if (kbState[SDL_SCANCODE_LEFT]){
			thePlayer.posX -= 1 * deltaTime;
			thePlayer.box.x = thePlayer.posX;
		}

		if (kbState[SDL_SCANCODE_DOWN]){
			thePlayer.posY += 1 * deltaTime;
			thePlayer.box.y = thePlayer.posY;
		}

		if (kbState[SDL_SCANCODE_UP]){
			thePlayer.posY -= 1 * deltaTime;
			thePlayer.box.y = thePlayer.posY;
		}

		if (kbState[SDL_SCANCODE_DOWN] && kbState[SDL_SCANCODE_LEFT]){
			thePlayer.posY += 1 * deltaTime;
			thePlayer.box.y = thePlayer.posY;
			thePlayer.posX -= 1 * deltaTime;
			thePlayer.box.x = thePlayer.posX;
		}

		if (kbState[SDL_SCANCODE_UP] && kbState[SDL_SCANCODE_LEFT]){
			thePlayer.posY -= 1 * deltaTime;
			thePlayer.box.y = thePlayer.box.y;
			thePlayer.posX -= 1 * deltaTime;
			thePlayer.box.x = thePlayer.box.x;
		}


		if (kbState[SDL_SCANCODE_DOWN] && kbState[SDL_SCANCODE_RIGHT]){
			thePlayer.posY += 1 * deltaTime;
			thePlayer.box.y = thePlayer.box.y;
			thePlayer.posX += 1 * deltaTime;
			thePlayer.box.x = thePlayer.posX;
		}

		if (kbState[SDL_SCANCODE_UP] && kbState[SDL_SCANCODE_RIGHT]){
			thePlayer.posY -= 1 * deltaTime;
			thePlayer.box.y -= 1 * deltaTime;
			thePlayer.posX += 1 * deltaTime;
			thePlayer.box.x += 1 * deltaTime;
		}

		//printf("CameraX is: %d, CameraY is: %d\n", theCamera.posX, theCamera.posY);

		/*Manuel code for camera*/
		if (kbState[SDL_SCANCODE_D]){
			if (theCamera.posX >= 0){
				theCamera.posX += 1 * deltaTime;
			}
			else
			{
				theCamera.posX = 0;
			}

			if (theCamera.posX <= 3250){
				theCamera.posX += 1 * deltaTime;
			}
			else
			{
				theCamera.posX = 3250;
			}
		}
		else if (kbState[SDL_SCANCODE_A]){
			if (theCamera.posX >= 0){
				theCamera.posX -= 1 * deltaTime;
			}
			else
			{
				theCamera.posX = 0;
			}

			if (theCamera.posX <= 3250){
				theCamera.posX -= 1 * deltaTime;
			}
			else
			{
				theCamera.posX = 3250;
			}
		}
		else if (kbState[SDL_SCANCODE_W]){

			if (theCamera.posY >= -850){
				theCamera.posY += 1 * deltaTime;
			}
			else
			{
				theCamera.posY = -850;
			}

			if (theCamera.posY <= 750){
				theCamera.posY += 1 * deltaTime;
			}
			else
			{
				theCamera.posY = 750;
			}
		}
		else if (kbState[SDL_SCANCODE_S]){

			if (theCamera.posY >= -850){
				theCamera.posY -= 1 * deltaTime;
			}
			else
			{
				theCamera.posY = -850;
			}

			if (theCamera.posY <= 750){
				theCamera.posY -= 1 * deltaTime;
			}
			else
			{
				theCamera.posY = 750;
			}
		}

		/*Drawing of map
		/*Drawing for isometrics*/
		for (int i = 0; i < 40; i++){
			for (int j = 0; j < 40; j++){

				int x_tile_position = ((39 - i) + j) * 50;
				int y_tile_position = 200 + ((-1 - (39 - i) + j) * 25);
				if ((x_tile_position < (theCamera.posX + 540)) && (x_tile_position > theCamera.posX) && (y_tile_position < (theCamera.posY + 480)) && (y_tile_position > theCamera.posY)){
					if ((39 - i) == 0 || j == 0 || (39 - i) == 39 || j == 39) //conditionals for borders.
					{
						glDrawSprite(water, (int)(x_tile_position - theCamera.posX), (int)(y_tile_position - theCamera.posY), TILE_WIDTH_X, TILE_HEIGHT_Y);
					}
					else
					{
						glDrawSprite(IsoGrass, (int)(x_tile_position - theCamera.posX), (int)(y_tile_position - theCamera.posY), TILE_WIDTH_X, TILE_HEIGHT_Y);
					}
				}
			}
		}

		if (thePlayer.data.curFrame == 3){
			animReset(&thePlayer.data);
		}
		else
		{
			animTick(&thePlayer.data, deltaTime);
		}

		if (h_pack.data.curFrame == 3){
			animReset(&h_pack.data);
		}
		else
		{
			animTick(&h_pack.data, deltaTime);
		}

		if (h_pack2.data.curFrame == 3)
		{
			animReset(&h_pack2.data);
		}
		else
		{
			animTick(&h_pack2.data, deltaTime);
		}

		if (enemy1.data.curFrame == 3){
			animReset(&enemy1.data);
		}
		else
		{
			animTick(&enemy1.data, deltaTime);
		}


		/*if player reaches the border of the map*/
		for (int i = 0; i < 40; i++){
			for (int j = 0; j < 40; j++){
				if (i == 0 || j == 0 || i == 39 || j == 39){
					if (AABBIntersect(&thePlayer.box, &tileSet[i][j].box) && tileSet[i][j].collided == true)
					{
						thePlayer.posX = prevPlayerPosX;
						thePlayer.posY = prevPlayerPosY;
					}
				}
			}
		}

		/*Physics interaction*/
		if (AABBIntersect(&regTree.box, &thePlayer.box))
		{
			thePlayer.posX = prevPlayerPosX;
			thePlayer.posY = prevPlayerPosY;
		}

		/*If else block for h_pack*/
		if (AABBIntersect(&h_pack.box, &thePlayer.box) == false && h_pack.collided == false)
		{
			animDraw(&h_pack.data, healthPack, h_pack.posX - theCamera.posX, h_pack.posY - theCamera.posY, 50, 50);
		}
		else
		{
			h_pack.collided = true;
		}

		if (AABBIntersect(&h_pack2.box, &thePlayer.box) == false && h_pack2.collided == false)
		{
			animDraw(&h_pack2.data, healthPack, h_pack2.posX - theCamera.posX, h_pack2.posY - theCamera.posY, 50, 50);
		}
		else
		{
			h_pack2.collided = true;
		}

		//Draw animation
		/*animSet(&thePlayer.data, &thePlayer.def);*/

		
		//printf("Current player X is position %f\n", thePlayerP->posX);
		animDraw(&enemy1.data, enemyTex, enemy1.posX - theCamera.posX, enemy1.posY - theCamera.posY, 50, 50);
		enemyUpdate(&enemy1, &thePlayer, deltaTime);
		//printf("enemy posX is: %f\n", enemy1.posX);
		/*Draws the enemy*/
		animDraw(&thePlayer.data, playerTex, thePlayer.posX - theCamera.posX, thePlayer.posY - theCamera.posY, 50, 50);

		glDrawSprite(tree, (int)(regTree.box.x - theCamera.posX), (int)(regTree.box.y - theCamera.posY), TREE_WIDTH, TREE_HEIGHT);
		/* Present to the player */
		SDL_GL_SwapWindow(window);

	}
	SDL_Quit();
	return 0;
}




