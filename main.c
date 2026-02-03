#include "raylib.h"
#define WIDTH 2000
#define HEIGHT 1500
#define ROWS 10
#define COLS 10
#define MAX_TAIL 100
#define CELLSIZE 100
#define dt GetFrameTime()

typedef enum direction{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
}direction;
typedef struct cell_st{
    float x;
    float y;
    float w;
    float h;
}cell_st;
typedef struct food_st{
    float x;
    float y;
    float r;
    int count;
}food_st;
typedef struct player_st{
    float x;
    float y;
    float w;
    float h;
    float move_speed;
    float move_timer;
    float perv_pos_x;
    float perv_pos_y;
    direction dir;
} player_st;
typedef struct tail_st{
    float x;
    float y;
    float perv_pos_x;
    float perv_pos_y;
}tail_st;

void draw_grid(cell_st *c){

    float total_w = COLS * CELLSIZE;
    float total_h = ROWS * CELLSIZE;

    float start_x = WIDTH/2 - total_w/2;
    float start_y = HEIGHT/2 - total_h/2;

    for(int i=0; i < ROWS;i++){
        for(int j=0; j < COLS;j++){

            float pos_x = start_x + i * CELLSIZE;
            float pos_y = start_y + j * CELLSIZE;

            if((i + j)% 2 == 0){DrawRectangle(pos_x,pos_y,CELLSIZE,CELLSIZE,GREEN);}
            else{DrawRectangle(pos_x,pos_y,CELLSIZE,CELLSIZE,DARKGREEN);}
        }
    }

}
bool draw_player(player_st *p){

    float total_w = COLS * CELLSIZE;
    float total_h = ROWS * CELLSIZE;

    float start_x = WIDTH/2 - total_w/2;
    float start_y = HEIGHT/2 - total_h/2;

    if(IsKeyPressed(KEY_W)){p->dir = DIR_UP;}
    if(IsKeyPressed(KEY_S)){p->dir = DIR_DOWN;}
    if(IsKeyPressed(KEY_A)){p->dir = DIR_LEFT;}
    if(IsKeyPressed(KEY_D)){p->dir = DIR_RIGHT;}

    p->move_timer += dt;
    bool moved = false;

    p->perv_pos_x = p->x;
    p->perv_pos_y = p->y;

    if (p->move_timer >= p->move_speed) {
        switch (p->dir) {
            case DIR_UP:    p->y -= CELLSIZE; break;
            case DIR_DOWN:  p->y += CELLSIZE; break;
            case DIR_LEFT:  p->x -= CELLSIZE; break;
            case DIR_RIGHT: p->x += CELLSIZE; break;
        }
        p->move_timer = 0.0f;
        moved = true;
    }

    if(p->x < start_x||p->x >= start_x+total_w||p->y < start_y||p->y >= start_y+total_h){
        DrawRectangle(0,0,100,100,RED);
    }
    
    DrawRectangle(p->x,p->y,p->w,p->h,BLUE);
    return moved;
}
void draw_tail(tail_st tail[MAX_TAIL],player_st p,int count, bool moved) {
    if (moved) {
        // Shift all segments: segment 2 moves to segment 1, etc.
        for (int i = count - 1; i > 0; i--) {
            tail[i] = tail[i - 1];
        }
        // Segment 0 takes the spot where the head just was
        tail[0].x = p.perv_pos_x;
        tail[0].y = p.perv_pos_y;
    }

    // Draw all active segments
    for (int i = 0; i < count; i++) {
        DrawRectangle(tail[i].x, tail[i].y, CELLSIZE, CELLSIZE, DARKBLUE);
    }
}
void draw_food(food_st *f,player_st p,float total_w,float total_h){

    Vector2 center = {f->x,f->y};
    Rectangle player = {p.x,p.y,CELLSIZE,CELLSIZE};

    int random_x = GetRandomValue(1,20);
    if(random_x % 2==0){random_x -= 1;}
    int random_y = GetRandomValue(1,20);
    if(random_y % 2==0){random_y -= 1;}

    if(CheckCollisionCircleRec(center,f->r,player)){
        f->x = WIDTH/2 - (total_w/2) + (CELLSIZE/2)* random_x;
        f->y = HEIGHT/2 - (total_h/2) + (CELLSIZE/2)* random_y;
        f->count ++;
    }
    
    DrawCircle(f->x,f->y,f->r,RED);
}
int main(void)
{
    InitWindow(WIDTH, HEIGHT, "raylib snake");

    float total_w = COLS * CELLSIZE;
    float total_h = ROWS * CELLSIZE;

    int random_x = GetRandomValue(1,20);
    if(random_x % 2==0){random_x -= 1;}
    int random_y = GetRandomValue(1,20);
    if(random_y % 2==0){random_y -= 1;}

    cell_st cell = {
        .x = WIDTH/2,
        .y = HEIGHT/2,
        .w = CELLSIZE,
        .h = CELLSIZE
    };
    food_st food = {
        .x = WIDTH/2 - (total_w/2) + (CELLSIZE/2)* random_x,
        .y = HEIGHT/2 - (total_h/2) + (CELLSIZE/2)* random_y,
        .r = 30,
        .count = 0
    };
    player_st player = {
        .x = WIDTH/2 - (total_w/2) + (total_w/2),
        .y = HEIGHT/2 - (total_h/2) + (total_h/2),
        .w = CELLSIZE,
        .h = CELLSIZE,
        .move_speed = 0.5f,
        .move_timer = 0.0f,
        .dir = DIR_RIGHT
    };
    tail_st tail[MAX_TAIL];
    for(int i=0;i<MAX_TAIL;i++){
        tail[i].x = player.x;
        tail[i].y = player.y;
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(BLACK);
            draw_grid(&cell);
            bool moved = draw_player(&player);
            draw_food(&food,player,total_h,total_w);
            draw_tail(tail,player,food.count,moved);
            
        EndDrawing();
    }

    CloseWindow();

    return 0;
}