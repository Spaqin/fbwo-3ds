#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "level.h"
#include "structs.h"
#include <sf2d.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_GRIDX           150
#define DEFAULT_GRIDY           20

#define DEFAULT_NEXT_TEXTX      260
#define DEFAULT_NEXT_TEXTY      55

#define DEFAULT_NEXT_FRAME_1X   260
#define DEFAULT_NEXT_FRAME_1Y   20

#define DEFAULT_NEXT_FRAME_2X   295
#define DEFAULT_NEXT_FRAME_2Y   20

#define DEFAULT_NEXT_FRAME_3X   330
#define DEFAULT_NEXT_FRAME_3Y   20

#define DEFAULT_NEXT_FRAME_4X   365
#define DEFAULT_NEXT_FRAME_4Y   20

#define DEFAULT_NEXT_FRAME_5X   365
#define DEFAULT_NEXT_FRAME_5Y   60

#define DEFAULT_NEXT_FRAME_6X   365
#define DEFAULT_NEXT_FRAME_6Y   100

#define DEFAULT_SCORE_TEXTX     280
#define DEFAULT_SCORE_TEXTY     210

#define DEFAULT_LINES_FRAMEX    115
#define DEFAULT_LINES_FRAMEY    140

#define DEFAULT_LEVEL_FRAMEX    115
#define DEFAULT_LEVEL_FRAMEY    190

#define DEFAULT_HOLD_FRAMEX     115
#define DEFAULT_HOLD_FRAMEY     20

#define DEFAULT_OFFSET_HOLDX    5
#define DEFAULT_OFFSET_HOLDY    5

#define DEFAULT_OFFSET_NEXTX    5
#define DEFAULT_OFFSET_NEXTY    5

#define DEFAULT_OFFSET_LVLY	10
#define DEFAULT_OFFSET_LNSY	10

void render_block(Tetrimino, bool, bool);
void render_frame();
void graphics_parse_config(char*);
void graphics_fini();
void graphics_init();
int load_textures(const char*);

extern u8 mode;
extern int level_grid[DIM_X][DIM_Y];
extern int score;
extern int total_lines;
extern u8 paused;
extern u8 controllable;

extern Tetrimino* in_play;
extern Tetrimino* hold;

extern u8* full_lines;

extern const int rotations[6][4][3][3];
extern const int rotation_I[4][5][5];

extern Tetrimino_list* next_blocks;

image background;
image next_text;
image next_frame[6];
image grid;
image score_text;
image lines_frame;
image level_frame;
image hold_frame;

sf2d_texture* tetriminos[7];
sf2d_texture* blocks[7];
sf2d_texture* score_num[10]; //by not using image struct but bare sf2d_texture* I just saved 304 bytes! well, that's kinda disappointing.
sf2d_texture* misc_num[10];
sf2d_texture* gameover_text;
sf2d_texture* paused_text;

s32 block_offset_holdx;
s32 block_offset_holdy;

s32 block_offset_nextx;
s32 block_offset_nexty;

s32 remove_line_count;

s32 digit_offset_linesy;
s32 digit_offset_levely;

#endif // GRAPHICS_H
