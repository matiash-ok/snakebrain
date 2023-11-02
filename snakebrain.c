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

apple_t Apple;
snake_t *head; snake_t *tail;
int snake_length = 1;

void init_snake();
void init_apple();
void detect_apple();
void increase_snake();
void move_snake();
bool collision_check();
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
    detect_apple();
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

    SDL_SetRenderDrawColor(renderer, 0x00 , 0xcc, 0x00 ,  255);
    SDL_Rect snake_cell;
    int cell_size = GRID_DIM / GRID_SIZE;
    snake_cell.w = cell_size;
    snake_cell.h = cell_size;

    snake_t *next = head;

    do{
        snake_cell.x  = x + ( next -> x * cell_size);
        snake_cell.y  = y + ( next -> y * cell_size);

        SDL_RenderFillRect(renderer,&snake_cell);

        next = next-> next;

    } while(next != NULL);
}

void render_grid(SDL_Renderer *renderer, int x , int y ){

    SDL_SetRenderDrawColor( renderer, 0x15, 0x15 ,0x15,255 );

    int cell_size = GRID_DIM / GRID_SIZE;

    SDL_Rect cell;

    cell.w = cell_size;
    cell.h = cell_size;

    for(int i = 0 ; i < GRID_SIZE ; i++){
        for (int j = 0 ; j < GRID_SIZE ; j++){
            cell.x  = x + ( i * cell_size);
            cell.y  = y + ( j * cell_size);
            SDL_RenderFillRect(renderer,&cell);
        }
    }

    return;
}

void move_snake(){

    int prev_x = head -> x;
    int prev_y = head -> y;
    int prev_dir = head -> dir;

    switch(head -> dir){
        case SNAKE_UP:
            head -> y--;
            break;
        case SNAKE_LEFT:
            head -> x--;
            break;
        case SNAKE_DOWN:
            head -> y++;
            break;
        case SNAKE_RIGHT:
            head -> x++;
            break;
    }

    if(collision_check()) return;

    snake_t *snake_cell = head;

    if(snake_cell -> next != NULL) snake_cell = snake_cell -> next;

    while(snake_cell != NULL){
        int temp_x = snake_cell -> x ;
        int temp_y = snake_cell -> y ;
        int temp_dir = snake_cell -> dir ;

        snake_cell -> x = prev_x ;
        snake_cell -> y = prev_y ;
        snake_cell -> dir = prev_dir ;

        prev_x = temp_x;
        prev_y = temp_y;
        prev_dir = temp_dir;

        snake_cell = snake_cell -> next;
    }
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

void locate_apple(){

    int new_x , new_y;
    bool in_snake;
    do {
        in_snake = false;
        new_x =  rand() % GRID_SIZE;
        new_y =  rand() % GRID_SIZE;

        snake_t *snake_cell = head;
        while(snake_cell != NULL){
            if(snake_cell -> x == new_x && snake_cell -> y == new_y) in_snake = true;
            snake_cell = snake_cell -> next;
        };

    }while (in_snake);

    Apple.x = new_x;
    Apple.y = new_y;
}

void init_apple(){
    locate_apple();
}

void render_apple(SDL_Renderer *renderer, int x , int y ){
    SDL_SetRenderDrawColor(renderer, 0xcc,0x00,0x00,255);
    SDL_Rect apple_cell;

    int cell_size = GRID_DIM / GRID_SIZE;

    apple_cell.h =cell_size;
    apple_cell.w =cell_size;
    apple_cell.x = x + Apple.x * cell_size ;
    apple_cell.y = y + Apple.y * cell_size ;
    SDL_RenderFillRect(renderer,&apple_cell);
}

void detect_apple(){
    if(head -> x == Apple.x && head -> y == Apple.y){
        locate_apple();
        increase_snake();
    }
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
bool collision_check(){
    if( head -> x < 0 || head -> x == GRID_SIZE ||
        head -> y < 0 || head -> y == GRID_SIZE){
        reset();
        return true;
    }

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
    return did_reset;
}
