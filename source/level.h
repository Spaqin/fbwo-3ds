#ifndef LEVEL_H
#define LEVEL_H

#include <3ds.h>
#include "structs.h"

int level_grid[DIM_X][DIM_Y]; //24 rows (4 hidden), 10 cols
/*
---------...--->
|0 1 2 3 ... 9
|1
|2
|3
...
|23
v

*/


Tetrimino* in_play;
Tetrimino* hold;
Tetrimino* last_deployed;
Tetrimino_list* next_blocks;


u32 ticks_before_glue;
u32 score;
u32 gravity_frame_counter;
u32 total_lines;
u32 high_score;
u32 ARE_frames;

u8 back_to_back_flag;
u8 render_line_clear;
u8 next_counter;
u8 level;
u8 gameover;
u8 hold_last; //flag to disallow infinite holding
u8 ARE_state;
u8 last_T_rotation;
u8 last_T_kick;
u8 back_to_back_flag_old;

Indicator_to_render indicator;

//function declaration
void save_highscore();
void load_highscore();
int check_collision(const Tetrimino);
void initialize_game();
u32 gravity_drop();
void increase_ticks();
void soft_drop();
Tetrimino get_ghost_piece();
int go_all_down();
void do_gravity();
void go_left();
void go_right();
void rotate_clockwise();
void rotate_counterclockwise();
void apply_rotation();
void glue();
void update_level();
void do_gameover();
void iterative_list_cleanup(Tetrimino_list*);
void deploy_next(bool);
void do_hold();
void deploy_hold();
int check_lines();
int check_gameover();
void clear_lines();
void move_down();
Tetrimino_list* generate_bag();
int rand_lim(int);
void ARE_tick();
void ARE_hold();
void ARE_cw();
void ARE_ccw();
void ARE_finish();
u32 T_corners_occupied();

extern Configuration cfg;

u8* full_lines; //array of 24 bytes that tells if any lines are cleared

const int rotation_offsets[2][4][5][2];

//now compliant with The Tetris Company guidelines!
//0-6 are all blocks except I-block, which requires 5x5 matrix, so it's in another variable.
const int rotations[6][4][3][3];
const int rotation_I[4][5][5];

//and now ARS variants
const int ARS_rotations[6][4][3][3];
const int ARS_rotation_I[4][4][4];

#endif // LEVEL_H












