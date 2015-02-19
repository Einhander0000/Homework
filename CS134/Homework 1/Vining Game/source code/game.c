/* #includes left out */

#include <SDL\SDL.h>
#include <GL\glew.h>
#include <stdio.h>
#include <Windows.h>


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
	glViewport(0, 0, 640, 480);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, 640, 480, 0, 0, 100);
	glEnable(GL_TEXTURE_2D);

	/* The previous frame's keyboard state */
	unsigned char kbPrevState[SDL_NUM_SCANCODES] = { 0 };
	/* The current frame's keyboard state */
	const unsigned char* kbState = NULL;

	/* Keep a pointer to SDL's internal keyboard state */
	kbState = SDL_GetKeyboardState(NULL);
	unsigned char* keys = SDL_GetKeyboardState(&kbState);

	/* Art to draw */
	GLuint vining;

	vining = glTexImageTGAFile("vining.tga", NULL, NULL);

	int* x_axis = 0;
	int* y_axis = 0;

	/* The game loop */
	while (!shouldExit) {
		/* kbState is updated by the message pump. Copy over the old state before the pump! */
		memcpy(kbPrevState, kbState, sizeof(kbPrevState));


		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		/* Game logic goes here */
		//SDL_GL_SwapWindow(window);
		glDrawSprite(vining, x_axis, y_axis, 100, 100); //mike V
		// Handle OS message pump
		SDL_Event event;
		while (SDL_PollEvent(&event)){
			switch (event.type) {
			case SDL_QUIT:
				shouldExit = 1;
			}		}
	if (keys[SDL_SCANCODE_RIGHT]){
		x_axis++;
	} else if (keys[SDL_SCANCODE_LEFT]){
		x_axis--;
	} else if (keys[SDL_SCANCODE_DOWN]){
		y_axis++;
	} else if (keys[SDL_SCANCODE_UP]){
		y_axis--;
	}

	/* Present to the player */
	SDL_GL_SwapWindow(window);
	}
	SDL_Quit();
	return 0;
}