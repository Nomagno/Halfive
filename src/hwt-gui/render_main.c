#include "../core/tracks.c"
#include "input_main.c"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
const uint16_t SCREEN_WIDTH = 640;
const uint16_t SCREEN_HEIGHT = 480;
uint16_t i;

SDL_Window *sdlWindow = NULL;
SDL_Renderer *sdlRenderer = NULL;
SDL_Texture *sdlTextures[1];

void sdlExit() {
  for (i = 0; i < (sizeof(sdlTextures) / sizeof(sdlTextures[0])); i++) {
    SDL_DestroyTexture(sdlTextures[i]);
    sdlTextures[i] = NULL;
  }

  SDL_DestroyRenderer(sdlRenderer);
  SDL_DestroyWindow(sdlWindow);
  sdlWindow = NULL;
  sdlRenderer = NULL;

  IMG_Quit();
  SDL_Quit();
}
void sdlError() {}

bool sdlLoadTexture(uint16_t sprite_num, char paths[sprite_num][100],
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
bool sdlLoadMedia() {
  bool success = true;
  char path_array[1][100];
  strcpy(path_array[0], "../../assets/graphics/forest_spritesheet.png");
  sdlLoadTexture(1, path_array, sdlTextures);
  if (sdlTextures[0] == NULL) {
    sdlError();
    success = false;
  }
  return success;
}

bool sdlInit() {
  bool success = true;

  sdlWindow = SDL_CreateWindow("Half-World Track Editor",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (sdlWindow == NULL) {
    sdlError();
    success = false;
  }

  sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
  if (sdlRenderer == NULL) {
    sdlError();
    success = false;
  }

  SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
  uint16_t imgInit = IMG_INIT_PNG;
  if (!(IMG_Init(imgInit) & imgInit)) {
    sdlError();
    success = false;
  }
  return success;
}

int main() {
  bool should_quit = false;
  bool could_init = sdlInit();
  SDL_Event event1;
  sdlLoadMedia();

  while (!should_quit) {
    while (SDL_PollEvent(&event1) != 0) {
      if (event1.type == SDL_QUIT) {
        should_quit = true;
      }
    }
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlTextures[0], NULL, NULL);
    SDL_RenderPresent(sdlRenderer);
  }
  sdlExit();
}
