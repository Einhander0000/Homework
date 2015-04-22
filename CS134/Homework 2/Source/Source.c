/* #includes left out */

#include <SDL\SDL.h>
#include <GL\glew.h>
#include <stdio.h>
#include <Windows.h>
#include <math.h>


#define VIEW_WIDTH 640 
#define VIEW_HEIGHT 480
#define TILE_WIDTH 40
#define TILE_HEIGHT 40

char shouldExit = 0;
int main(void)

{
	/* Initialize SDL *l
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
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

	/* The previous frame's keyboard state */
	unsigned char kbPrevState[SDL_NUM_SCANCODES] = { 0 };
	/* The current frame's keyboard state */
	const unsigned char* kbState = NULL;  //use this instead?

	/* Keep a pointer to SDL's internal keyboard state */
	/*Calling this twice? Ask.*/

	kbState = SDL_GetKeyboardState(NULL);

	/*Level Set up*/
	int level[40][40];
	for (int i = 0; i < 40; i++){
		for (int j = 0; j < 40; j++){
			if (i == 0 || j == 0 || i == 39 || j == 39){
				level[i][j] = 0;
			}
			else
			{
				level[i][j] = 1;
			}
		}
	}


	/* Art to draw */
	GLuint vining;
	GLuint greenSquare;
	GLuint redSquare;


	/*Drawing Assets*/
	greenSquare = glTexImageTGAFile("greenSquare.tga", NULL, NULL);
	redSquare = glTexImageTGAFile("stop.tga", NULL, NULL);
	vining = glTexImageTGAFile("vining.tga", NULL, NULL);

	int x_axis = VIEW_WIDTH / 2;
	int y_axis = VIEW_HEIGHT / 2;

	int camX = 0;
	int camY = 0;

	/*Timing elements*/
	Uint32 start;
	const int FPS = 60;

	/* The game loop */
	while (!shouldExit) {
		start = SDL_GetTicks();
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


		/*NEEDS TO BE FIXED*/
		if (kbState[SDL_SCANCODE_RIGHT]){
			x_axis++;
			if (camX <= 680){
				camX++;
			}
		}
		else if (kbState[SDL_SCANCODE_LEFT]){
			x_axis--;
			if (camX >= 0){
				camX--;
			}
		}
		else if (kbState[SDL_SCANCODE_DOWN]){
			y_axis++;
			if (camY <= 480){
				camY++;
			}
		}
		else if (kbState[SDL_SCANCODE_UP]){
			y_axis--;
			if (camY >= 0){
				camY--;
			}
		}

		if (y_axis > 380){
			y_axis = 380;
		}
		else if (y_axis < 0){
			y_axis = 0;
		}

		if (x_axis > 540){
			x_axis = 540;
		}
		else if (x_axis < 0){
			x_axis = 0;
		}

		for (int i = 0; i < 40; i++) {
			for (int j = 0; j < 40; j++) {

				int tempCamX = camX / TILE_WIDTH;
				int tempCamY = camY / TILE_WIDTH;


				if (i == 0 || j == 0 || i == 39 || j == 39){

					/*For Usage later for actual bordering/Tracking */
					//glDrawSprite(redSquare, ((tempCamX * i) * (VIEW_WIDTH)), ((tempCamY * j) * (VIEW_WIDTH)-camY), 40, 40); //Active tracking of blocks
					glDrawSprite(redSquare, (i * 40) - camX, (j * 40) - camY, 40, 40);
				}
				else {
					//glDrawSprite(greenSquare, ((tempCamX * i) * (VIEW_WIDTH)-camX), ((tempCamY * j) * (VIEW_WIDTH)-camY), 40, 40); //Active tracking of blocks

					glDrawSprite(greenSquare, (i * 40) - camX, (j * 40) - camY, 40, 40);
				}
			}
		}
		glDrawSprite(vining, x_axis, y_axis, 100, 100); //mike 


		if (1000 / FPS > SDL_GetTicks() - start){
			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
		}
		/* Present to the player */
		SDL_GL_SwapWindow(window);
	}
	SDL_Quit();
	return 0;
}