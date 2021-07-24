#include "../core/tracks.c"
#include "input_main.c"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int i;

SDL_Window *sdlWindow = NULL;
SDL_Renderer *sdlRenderer = NULL;

void sdlExit();
void sdlError();

bool sdlLoadTexture(int sprite_num, char paths[sprite_num][100],
                    SDL_Texture *returnArray[sprite_num]) {

  bool any_errors = false;
  SDL_Texture *arrayTexture[sprite_num];
  for (i = 0; i < sprite_num; i++) {
    arrayTexture[i] = NULL; 
    }

  SDL_Surface *loadedSurfaces[sprite_num];

  for (i = 0; i < sprite_num; i++) {
    loadedSurfaces[i] = IMG_Load(paths[i]);
    if (loadedSurfaces[i] == NULL) {
      sdlError();
      any_errors = true;
    }
    arrayTexture[i] =
        SDL_CreateTextureFromSurface(sdlRenderer, loadedSurfaces[i]);
    if (arrayTexture[i] == NULL) {
      sdlError();
      any_errors = true;
    }
    SDL_FreeSurface(loadedSurfaces[i]);
  }

  for (i = 0; i < sprite_num; i++) {
    returnArray[i] = arrayTexture[i];
  }

  return any_errors;
}
bool sdlLoadMedia() {}

bool sdlInit() {
  bool success = true;

  sdlWindow = SDL_CreateWindow("Half-World Track Editor",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (sdlWindow == NULL) {
    sdlError();
    success = false;
  }

  SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

  int imgInit = IMG_INIT_PNG;
  if (!(IMG_Init(imgInit) & imgInit)) {
    sdlError();
    success = false;
  }
  return success;
}

bool loadMedia() {

}

void sdlRender() {
  bool should_quit;
  bool could_init = sdlInit();
  SDL_Event event1;

  while (!should_quit) {

    while (SDL_PollEvent(&event1) != 0) {
      if (event1.type == SDL_QUIT) {
        should_quit = true;
      }
    }
  }
}
