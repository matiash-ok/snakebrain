#include "snakebrain.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#if 0
//
//FULLSCREEN
#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#else

//STREAM
#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define VELOCITY 200
#endif

#define GRID_SIZE 19
#define GRID_DIM 800

enum {
    SNAKE_UP,
    SNAKE_RIGHT,
    SNAKE_DOWN,
    SNAKE_LEFT
};

typedef struct snake {
    int x ;
    int y ;
    int dir;

    struct snake *next;
}snake_t;

typedef struct apple_struct {
    int x;
    int y;
}apple_t;

apple_t *apple;
snake_t *head; snake_t *tail;
int snake_length = 1;

void init_snake();
void init_apple();
void eat_apple();
void increase_snake();
void move_snake();
void collision_check();
void change_dir(int dir);
void render_snake(SDL_Renderer *renderer, int x , int y );
void render_grid(SDL_Renderer *renderer, int x , int y );
void render_apple(SDL_Renderer *renderer, int x , int y );

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
        SDL_WINDOW_RESIZABLE
    );

    if(!window){
        fprintf(stderr, "ERROR: !window");
    }

    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    if(!renderer){
        fprintf(stderr, "ERROR: !renderer");
    }

    int grid_x = (WINDOW_WIDTH / 2) - (GRID_DIM / 2);
    int grid_y = 100 + (WINDOW_HEIGHT / 2) - (GRID_DIM / 2);

    bool quit = false;
    int prev_dir_event;

    SDL_Event event;
    init_snake();
    init_apple();

    while(!quit){
        prev_dir_event = 0 ;
        SDL_Delay(VELOCITY);
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYUP:
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym){
                        case SDLK_ESCAPE:
                            quit=true;
                            break;
                        case SDLK_LEFT:
                            if(prev_dir_event == -1) break;
                            prev_dir_event = -1;
                            change_dir(-1);
                            break;
                        case SDLK_RIGHT:
                            if(prev_dir_event == 1) break;
                            prev_dir_event = 1;
                            change_dir(1);
                            break;
                    }
                    break;
            }
        }

    SDL_RenderClear(renderer);

    // RENDER LOOP START
    render_grid(renderer,grid_x,grid_y);
    render_apple(renderer,grid_x,grid_y);
    render_snake(renderer,grid_x,grid_y);
    move_snake();
    // RENDER LOOP END
    SDL_SetRenderDrawColor( renderer, 0x00, 0x00 ,0x00,255);
    SDL_RenderPresent(renderer);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


void init_snake(){
    snake_t *new = malloc(sizeof(snake_t));
    new -> x = GRID_SIZE / 2 ;
    new -> y = GRID_SIZE / 2 ;
    new -> dir = SNAKE_UP;
    new ->next = NULL ;

    head = new ;
    tail = new ;

    increase_snake();
    increase_snake();
    return;
}

void increase_snake(){
    snake_length++;
    snake_t *new = malloc(sizeof(snake_t));
    switch(tail -> dir){
        case SNAKE_UP:
            new -> x = tail -> x;
            new -> y = tail -> y +1;
            break;
        case SNAKE_LEFT:
            new -> x = tail -> x +1;
            new -> y = tail -> y;
            break;
        case SNAKE_DOWN:
            new -> x = tail -> x;
            new -> y = tail -> y - 1;
            break;
        case SNAKE_RIGHT:
            new -> x = tail -> x -1;
            new -> y = tail -> y;
            break;
    }
    new -> dir = tail -> dir;
    new -> next = NULL;

    tail -> next = new ;
    tail = new ;
    return;
}

void render_snake(SDL_Renderer *renderer, int x , int y ){

    SDL_Rect snake_cell;
    bool change_color = false;
    int cell_size = GRID_DIM / GRID_SIZE;
    snake_cell.w = cell_size;
    snake_cell.h = cell_size;

    snake_t *next = head;

    do{
        if(change_color){
            SDL_SetRenderDrawColor(renderer, 0x00 , 0xcc, 0x00 ,  255);
        }else{
            SDL_SetRenderDrawColor(renderer, 0xff , 0xcc, 0x00 ,  255);
        }

        snake_cell.x  = x + ( next -> x * cell_size);
        snake_cell.y  = y + ( next -> y * cell_size);

        SDL_RenderFillRect(renderer,&snake_cell);

        next = next-> next;
        change_color = !change_color;

    } while(next != NULL);
}

void render_grid(SDL_Renderer *renderer, int x , int y ){

    SDL_SetRenderDrawColor( renderer, 0x55, 0x55 ,0x55,255 );

    int cell_size = GRID_DIM / GRID_SIZE;

    SDL_Rect cell;

    cell.w = cell_size;
    cell.h = cell_size;

    for(int i = 0 ; i < GRID_SIZE ; i++){
        for (int j = 0 ; j < GRID_SIZE ; j++){
            cell.x  = x + ( i * cell_size);
            cell.y  = y + ( j * cell_size);
            SDL_RenderDrawRect(renderer,&cell);
        }
    }

    return;
}

void move_snake(){

    snake_t *snake_cell      = head;
    int next_dir = 6;

    do{
        bool can_eat_apple = false;

        switch(snake_cell -> dir){
            case SNAKE_UP:
                snake_cell -> y = snake_cell -> y - 1;
                break;
            case SNAKE_LEFT:
                snake_cell -> x = snake_cell -> x - 1;
                break;
            case SNAKE_DOWN:
                snake_cell -> y = snake_cell -> y + 1;
                break;
            case SNAKE_RIGHT:
                snake_cell -> x = snake_cell -> x + 1;
                break;
        }

        if(next_dir == 6 ) collision_check();

        if(snake_cell -> x == apple -> x && snake_cell -> y == apple -> y){
            can_eat_apple = true;
        }

        int temp_dir = snake_cell -> dir;
        if(next_dir != 6) snake_cell -> dir = next_dir;
        next_dir  = temp_dir;

        snake_cell = snake_cell -> next ;

        if(can_eat_apple) eat_apple();
    }

    while(snake_cell != NULL);

}

void change_dir(int dir){
    if(dir == -1 && head -> dir == SNAKE_UP ) {
        head -> dir = SNAKE_LEFT;
        return;
    }
    if(dir == 1 && head -> dir == SNAKE_LEFT ){
        head -> dir = SNAKE_UP;
        return;
    }
    head -> dir = head -> dir + dir;
    return;
}

void locate_apple(apple_t *new_apple){
    new_apple -> x = rand() % GRID_SIZE/2;
    new_apple -> y = rand() % GRID_SIZE/2;
}

void init_apple(){
    apple_t *new_apple = malloc(sizeof(apple_t));
    locate_apple(new_apple);
    apple = new_apple;
}

void render_apple(SDL_Renderer *renderer, int x , int y ){
    SDL_SetRenderDrawColor(renderer, 0xcc,0x00,0x00,255);
    SDL_Rect apple_cell;

    int cell_size = GRID_DIM / GRID_SIZE;

    apple_cell.h =cell_size;
    apple_cell.w =cell_size;
    apple_cell.x = x + apple -> x * cell_size ;
    apple_cell.y = y + apple -> y * cell_size ;
    SDL_RenderFillRect(renderer,&apple_cell);
}

void eat_apple(){
    locate_apple(apple);
    increase_snake();
}

void reset(){
    snake_t *snake_cell = head;

    while(snake_cell != NULL){
       snake_t *snake_cell2 = snake_cell;
       snake_cell = snake_cell -> next;
       free(snake_cell2);
    }
    init_snake();
}
void collision_check(){
    if( head -> x < 0 || head -> x == GRID_SIZE ||
        head -> y < 0 || head -> y == GRID_SIZE) reset();

    snake_t *new_snake_cell = head -> next;
    bool did_reset = false;
    while(new_snake_cell != NULL || did_reset){
        if(head -> x == new_snake_cell -> x && head -> y == new_snake_cell -> y) {
            reset();
            did_reset = true;
            break;
        }

        new_snake_cell = new_snake_cell -> next;

    }
}
