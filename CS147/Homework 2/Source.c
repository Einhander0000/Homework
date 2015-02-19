/* #includes left out */

#include <SDL\SDL.h>
#include <GL\glew.h>
#include <stdio.h>
#include <Windows.h>
#include <math.h>


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

	/*Level Set up*/
	int level[40][40];
	for (int i = 0; i < 40; i++){
		for (int j = 0; j < 40; j++){
			if (i == 0 || j == 0 || i == 39 || j == 39){
				level[i][j] == 0;
			}
			else
			{
				level[i][j] == 1;
			}
		}
	}



	
	 




	/* Art to draw */
	GLuint vining;

	/*Drawing Assets*/
	vining = glTexImageTGAFile("vining.tga", NULL, NULL);

	int *x_axis = 0;
	int *y_axis = 0;


	float lastFrameMs = 0.0;
	float currentFrameMs = SDL_GetTicks();
	/* The game loop */
	while (!shouldExit) {
		lastFrameMs = currentFrameMs;
		/* kbState is updated by the message pump. Copy over the old state before the pump! */
		memcpy(kbPrevState, kbState, sizeof(kbPrevState));


		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		/* Game logic goes here */
		//SDL_GL_SwapWindow(window);
		
		// Handle OS message pump
		SDL_Event event;  
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				shouldExit = 1;
			}
		}

		currentFrameMs = SDL_GetTicks();
		float deltaTime = (currentFrameMs - lastFrameMs) / 1000.0f;		printf("%d", x_axis);		glDrawSprite(vining, x_axis, y_axis, 100, 100); //mike V

		if (keys[SDL_SCANCODE_RIGHT]){
			if (x_axis > 0 && x_axis < 640){
				x_axis += (int)ceil(deltaTime);
			}
		}
		else if (keys[SDL_SCANCODE_LEFT]){
			if (x_axis > 0 && x_axis < 640){
				x_axis -= (int)ceil(deltaTime);
			}
		}
		else if (keys[SDL_SCANCODE_DOWN]){
			if (y_axis > 0 && y_axis < 480){
				y_axis += (int)ceil(deltaTime);
			}
		}
		else if (keys[SDL_SCANCODE_UP]){
			if (y_axis > 0 && y_axis < 480){
				y_axis -= (int)ceil(deltaTime);
			}
		}

		/* Present to the player */
		SDL_GL_SwapWindow(window);
	}
	SDL_Quit();
	return 0;
}