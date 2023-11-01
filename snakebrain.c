#include "snakebrain.h"
#include <stdio.h>

#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

int main(){

    SDL_Window *window;
    SDL_Renderer *renderer;

    if(SDL_INIT_VIDEO < 0 ){
        fprintf(stderr, "ERROR: SDL_INIT_VIDEO");
    }

    window =  SDL_CreateWindow(
        "Snake",
        WINDOW_X,
        WINDOW_Y,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );

    if(!window){
        fprintf(stderr, "ERROR: !window");
    }

    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    if(!renderer){
        fprintf(stderr, "ERROR: !renderer");
    }

    SDL_RenderClear( renderer );
    SDL_SetRenderDrawColor( renderer, 0x00, 0xff ,0x00,255 );
    SDL_RenderPresent( renderer );

    SDL_Delay(200);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("hello world\n");

    return 0;
}
