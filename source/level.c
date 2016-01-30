#include "level.h"
#include "structs.h"
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//peanut variable init time
int render_line_clear = 0;

const int rotation_offsets[2][4][5][2] = //[JLSTZ/I][Rotation][Offsets1..5][x, y]
{
 {
    { {0, 0}, {0, 0},  {0, 0},  {0, 0},  {0, 0} }, //rotation 0
    { {0, 0}, {1, 0},  {1, 1},  {0, -2}, {1, -2} }, //rotation R - for Y there was a sign change because "-" means going up, and "+" is down
    { {0, 0}, {0, 0},  {0, 0},  {0, 0},  {0, 0} }, //rotation 2
    { {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2} } //rotation L
 },
 {
    { {0, 0},   {-1, 0}, {2, 0},  {-1, 0}, {2, 0} }, //rotation 0
    { {-1, 0},  {0, 0},  {0, 0},  {0, -1}, {0, 2} }, //rotation R - for Y there was a sign change because "-" means going up, and "+" is down
    { {-1, -1}, {1, -1}, {-2, 1}, {1, 0},  {-2, 0} }, //rotation 2
    { {0, -1},   {0, -1}, {0, -1}, {0, 1},  {0, -2} } //rotation L
 }
};
//type rotation y x
const int rotations[6][4][3][3] =
{
    { //O-block
        {
         { 1, 1, 0 },
         { 1, 1, 0 },
         { 0, 0, 0 },
        },
        {
         { 1, 1, 0 },
         { 1, 1, 0 },
         { 0, 0, 0 },
        },
        {
         { 1, 1, 0 },
         { 1, 1, 0 },
         { 0, 0, 0 },
        },
        {
         { 1, 1, 0 },
         { 1, 1, 0 },
         { 0, 0, 0 },
        }
    },
    { //T-block
        {
         { 0, 2, 0 },
         { 2, 2, 2 },
         { 0, 0, 0 }
        },
        {
         { 0, 2, 0 },
         { 0, 2, 2 },
         { 0, 2, 0 }
        },
        {
         { 0, 0, 0 },
         { 2, 2, 2 },
         { 0, 2, 0 }
        },
        {
         { 0, 2, 0 },
         { 2, 2, 0 },
         { 0, 2, 0 }
        }
    },
    { //S-block
        {
         { 0, 3, 3 },
         { 3, 3, 0 },
         { 0, 0, 0 }
        },
        {
         { 0, 3, 0 },
         { 0, 3, 3 },
         { 0, 0, 3 }
        },
        {
         { 0, 0, 0 },
         { 0, 3, 3 },
         { 3, 3, 0 },
        },
        {
         { 3, 0, 0 },
         { 3, 3, 0 },
         { 0, 3, 0 }
        }
    },
    { //Z-block
        {
         { 4, 4, 0 },
         { 0, 4, 4 },
         { 0, 0, 0 }
        },
        {
         { 0, 0, 4 },
         { 0, 4, 4 },
         { 0, 4, 0 }
        },
        {
         { 0, 0, 0 },
         { 4, 4, 0 },
         { 0, 4, 4 }
        },
        {
         { 0, 4, 0 },
         { 4, 4, 0 },
         { 4, 0, 0 }
        }
    },
    { //J-block
        {
         { 5, 0, 0 },
         { 5, 5, 5 },
         { 0, 0, 0 }
        },
        {
         { 0, 5, 5 },
         { 0, 5, 0 },
         { 0, 5, 0 }
        },
        {
         { 0, 0, 0 },
         { 5, 5, 5 },
         { 0, 0, 5 }
        },
        {
         { 0, 5, 0 },
         { 0, 5, 0 },
         { 5, 5, 0 }
        }

    },
    { //L-block
        {
         { 0, 0, 6 },
         { 6, 6, 6 },
         { 0, 0, 0 }
        },
        {
         { 0, 6, 0 },
         { 0, 6, 0 },
         { 0, 6, 6 }
        },
        {
         { 0, 0, 0 },
         { 6, 6, 6 },
         { 6, 0, 0 }
        },
        {
         { 6, 6, 0 },
         { 0, 6, 0 },
         { 0, 6, 0 }
        }

    }
};
//rotation y x
const int rotation_I[4][5][5] =
{
    {
     { 0, 0, 0, 0, 0},
     { 0, 0, 0, 0, 0},
     { 0, 7, 7, 7, 7},
     { 0, 0, 0, 0, 0},
     { 0, 0, 0, 0, 0}
    },
    {
     { 0, 0, 0, 0, 0},
     { 0, 0, 7, 0, 0},
     { 0, 0, 7, 0, 0},
     { 0, 0, 7, 0, 0},
     { 0, 0, 7, 0, 0}
    },
    {
     { 0, 0, 0, 0, 0},
     { 0, 0, 0, 0, 0},
     { 7, 7, 7, 7, 0},
     { 0, 0, 0, 0, 0},
     { 0, 0, 0, 0, 0}
    },
    {
     { 0, 0, 7, 0, 0},
     { 0, 0, 7, 0, 0},
     { 0, 0, 7, 0, 0},
     { 0, 0, 7, 0, 0},
     { 0, 0, 0, 0, 0}
    }
};

/*
returns whether or not there will be a collision for a given tetrimino.
*/
int check_collision(const Tetrimino tetrimino)
{
    int type = tetrimino.type;
    int rotation = tetrimino.rotation;
    int posx = tetrimino.posx;
    int posy = tetrimino.posy;
    if(tetrimino.type != I_TYPE)
    {
        for(int i = 2; i >= 0; --i) //i - row
            for(int j = 0; j < 3; ++j) //j - col
            {
                if(rotations[type][rotation][i][j]) //if we have something in that field...
		{
		    if(posx + j < 0) //and that something is too much to the left
		    {
			return 1;
		    }
		    if(posx + j >= DIM_X) //or it's too much to the right
		    {
			return 1;
		    }
		    if(posy + i >= DIM_Y) //or it's a bit too low
		    {
			return 1;
		    }
		    if(level_grid[posx+j][posy+i]) //or simply something is already there
		    {
			return 1;
		    }
		}
            }
    }
    else
    {
        for(int i = 4; i >= 0; --i)
            for(int j = 0; j < 5; ++j)
            {
                if(rotation_I[rotation][i][j]) //so we have something here
		{
		    if(posx + j < 0) //and that something is too much to the left
		    {
			return 1;
		    }
		    if(posx + j >= DIM_X) //or it's too much to the right
		    {
			return 1;
		    }
		    if(posy + i >= DIM_Y) //or it's a bit too low
		    {
			return 1;
		    }
		    if(level_grid[posx+j][posy+i]) //or simply something is already there
		    {
			return 1;
		    }
		}
            }
    }
    return 0;

}

//sets default values.
void initialize_game()
{
    //clear the level
    for(int i = 0; i < DIM_X; ++i)
	for(int j = 0; j < DIM_Y; ++j)
            level_grid[i][j] = 0;

    //generate two sets
    srand(time(NULL));
    next_blocks = generate_bag();
    Tetrimino_list* element = next_blocks;
    while(element->next != NULL)
        element = element->next;
    element->next = generate_bag();

    //clear the values;
    hold = NULL;
    last_deployed = NULL;
    full_lines = NULL;
    gameover = 0;
    ticks_before_glue = 0;
    next_counter = 7; //is decreased by one each time a next block is deployed; when reaches 0, a new bag is generated, and it's set back to 7.
    total_lines = 0;
    gravity_frame_counter = 0;
    score = 0;

    deploy_next();
}

/*
Each tick the piece goes down; this function does that.
Returns if a drop was successful or not.
*/
int gravity_drop()
{
    Tetrimino new_pos = *in_play;
    new_pos.posy++;
    if(check_collision(new_pos)) //means that there's something below
    {
        if(ticks_before_glue >= cfg.glue_delay)
            glue();
        return 0;
    }
    else //nothing below - just go down
    {
        in_play->posy++;
	ticks_before_glue = 0;
        return 1;
    }
}

void increase_ticks()
{
    Tetrimino new_pos = *in_play;
    new_pos.posy++;
    if(check_collision(new_pos)) //means that there's something below, so we count down
        ticks_before_glue++;
}

void do_gravity()
{
    increase_ticks();
    ++gravity_frame_counter;
    if(gravity_frame_counter == cfg.frames_per_drop[level-1])
    {
	for(int i = 0; i < cfg.rows_per_drop[level-1]; ++i) 
	    gravity_drop();
	gravity_frame_counter = 0;
    }

}

void soft_drop()
{
 //the only difference that it adds score if the drop was successful.
    if(gravity_drop())
        score++;
}

//returns a tetrimino that serves as the ghost piece.
Tetrimino get_ghost_piece()
{
    Tetrimino ghost_piece = *in_play;
    while(!check_collision(ghost_piece))
        ghost_piece.posy++;
    ghost_piece.posy--;
    return ghost_piece;
}
/*
Moves the tetrimino all the way down and glues it. Also, adds score for that.
Returns - how many rows were skipped
*/
int go_all_down()
{
    Tetrimino copy = *in_play;
    int rows = -1;
    while(!check_collision(copy))
    {
        copy.posy++;
        rows++;
    }
    in_play->posy = copy.posy - 1;
    score += rows << 1; //2 points per tile dropped
    glue();
    return rows;
}
/*
Moves the tetrimino to the left.
*/
void go_left()
{
    Tetrimino copy = *in_play;
    copy.posx--;
    if(!check_collision(copy))
    {
        in_play->posx--;
        ticks_before_glue = 0;
    }
}
/*
Moves the tetrimino to the right.
*/
void go_right()
{
    Tetrimino copy = *in_play;
    copy.posx++;
    if(!check_collision(copy))
    {
        in_play->posx++;
        ticks_before_glue = 0;
    }
}
/*
Rotates the tetrimino to the right (clockwise)
*/
void rotate_clockwise()
{
    Tetrimino copy = *in_play;
    copy.rotation = (copy.rotation + 1) % 4;
    if(in_play->type == O_TYPE)
    {
        ticks_before_glue = 0;
        return;
    }
    else if(in_play->type != I_TYPE) // J L S T Z types are not I type.
    {
        u8 rot_orig = in_play->rotation;
        u8 rot_copy = copy.rotation;
        u32 new_posx, new_posy;

        for(u8 i = 0; i < 5; i++)
        {
            new_posx = in_play->posx + rotation_offsets[0][rot_orig][i][0] - rotation_offsets[0][rot_copy][i][0];
            new_posy = in_play->posy + rotation_offsets[0][rot_orig][i][1] - rotation_offsets[0][rot_copy][i][1];
            //to do? recognize T-spins
            copy.posx = new_posx;
            copy.posy = new_posy;
            if(!(check_collision(copy)))
            {
                in_play->posx = new_posx;
                in_play->posy = new_posy;
                in_play->rotation = rot_copy;
                ticks_before_glue = 0;
                return;
            }
        }
    }
    else //must be I-type then!
    {
        u8 rot_orig = in_play->rotation;
        u8 rot_copy = copy.rotation;
        u32 new_posx, new_posy;

        for(u8 i = 0; i < 5; i++)
        {
            new_posx = in_play->posx + rotation_offsets[1][rot_orig][i][0] - rotation_offsets[1][rot_copy][i][0];
            new_posy = in_play->posy + rotation_offsets[1][rot_orig][i][1] - rotation_offsets[1][rot_copy][i][1];
            copy.posx = new_posx;
            copy.posy = new_posy;
            if(!(check_collision(copy)))
            {
                in_play->posx = new_posx;
                in_play->posy = new_posy;
                in_play->rotation = rot_copy;
                ticks_before_glue = 0;

            }
        }
    }
}
/*
Rotates the tetrimino to the left (counterclockwise)
*/
void rotate_counterclockwise()
{
    Tetrimino copy = *in_play;
    copy.rotation = (copy.rotation + 3) % 4; //3 is -1 % 4
    if(in_play->type == O_TYPE)
    {
        ticks_before_glue = 0;
        return;
    }
    else if(in_play->type != I_TYPE) // J L S T Z types are not I type.
    {
        u8 rot_orig = in_play->rotation;
        u8 rot_copy = copy.rotation;
        u32 new_posx, new_posy;

        for(u8 i = 0; i < 5; i++)
        {
            new_posx = in_play->posx + rotation_offsets[0][rot_orig][i][0] - rotation_offsets[0][rot_copy][i][0];
            new_posy = in_play->posy + rotation_offsets[0][rot_orig][i][1] - rotation_offsets[0][rot_copy][i][1];
            copy.posx = new_posx;
            copy.posy = new_posy;
            if(!(check_collision(copy)))
            {
                in_play->posx = new_posx;
                in_play->posy = new_posy;
                in_play->rotation = rot_copy;
                ticks_before_glue = 0;
                return;
            }
        }
    }
    else //must be I-type then!
    {
        u8 rot_orig = in_play->rotation;
        u8 rot_copy = copy.rotation;
        u32 new_posx, new_posy;

        for(u8 i = 0; i < 5; i++)
        {
            new_posx = in_play->posx + rotation_offsets[1][rot_orig][i][0] - rotation_offsets[1][rot_copy][i][0];
            new_posy = in_play->posy + rotation_offsets[1][rot_orig][i][1] - rotation_offsets[1][rot_copy][i][1];
            copy.posx = new_posx;
            copy.posy = new_posy;
            if(!(check_collision(copy)))
            {
                in_play->posx = new_posx;
                in_play->posy = new_posy;
                in_play->rotation = rot_copy;
                ticks_before_glue = 0;

            }
        }
    }
}


/*
Glues the tetrimino to the playfield. Also, it sets some stuff and does basic game checks.
*/
void glue()
{
    u32 posx = in_play->posx;
    u32 posy = in_play->posy;
    u8 type = in_play->type;
    u8 rotation = in_play->rotation;
    if(type != I_TYPE)
        for(int i = 0; i < 3; ++i)
            for(int j = 0; j < 3; ++j)
            {
		if(posx + i >= 0 && posy+j <= DIM_Y && posx + i < DIM_X)
                level_grid[posx+i][posy+j] |= rotations[type][rotation][j][i];
            }
    else
        for(int i = 0; i < 5; ++i)
            for(int j = 0; j < 5; ++j)
            {
		if(posx + i >= 0 && posy+j <= DIM_Y && posx + i < DIM_X)
                level_grid[posx+i][posy+j] |= rotation_I[rotation][j][i];
            }
    last_deployed = in_play;
    u8 lines = check_lines();
    total_lines += lines;
    u32 score_to_add = 0;
    u8 back_to_back_flag_old = back_to_back_flag;
    switch(lines) //todo - if recognized t-spins, award points for line clears too
    {
        case 0: //no lines? try to deploy a new one
            deploy_next();
	    return;
            break;
        case 1:
            score_to_add += level * 100;
            back_to_back_flag = 0;
            break;
        case 2:
            score_to_add += level * 300;
            back_to_back_flag = 0;
            break;
        case 3:
            score_to_add += level * 500;
            back_to_back_flag = 0;
            break;
        case 4:
            score_to_add += level * 800;
            back_to_back_flag = 1;
            break;
    }
    if(back_to_back_flag_old)
    {
        score_to_add *= 3;
        score_to_add >>= 1;
    }
    score += score_to_add;

    int supposed_level = total_lines/cfg.lines_per_lvl + 1;
    if(supposed_level > level && supposed_level <= 20)
    {
        level = supposed_level;
	gravity_frame_counter = 0;
    }
    render_line_clear = 1;
}

/*
This function is to be called after rendering clearing lines effect. It will update the model and return to normal play.
*/
void update_level()
{
    render_line_clear = 0;
    clear_lines();
    deploy_next();
}

void do_gameover()
{
    gameover = 1;
    //clean up
    free(next_blocks); //i don't feel like freeing everything tbh ://
    free(in_play);
    free(hold);
    free(full_lines);

}

void recursive_list_cleanup(Tetrimino_list* element)
{
    if(element == NULL)
        return;
    recursive_list_cleanup(element->next);
    free(element);
}

void deploy_next()
{
    Tetrimino* to_deploy = next_blocks->tetrimino;
    Tetrimino_list* to_free = next_blocks;

    //set the values to default
    to_deploy->posx = 4;
    to_deploy->posy = 2;
    to_deploy->rotation = 0;
    hold_last = 0;
    //clean up
    next_blocks = next_blocks->next;
    free(to_free);
    //if the first bag is emptied, make a new one
    if(--next_counter == 0)
    {
        Tetrimino_list* element = next_blocks;
        while(element->next != NULL)
            element = element->next;
        element->next = generate_bag();
        next_counter = 7;
    }
    //finally, put it in play
    in_play = to_deploy;

    if(check_collision(*in_play)) //if it can't spawn, well, it's a game over.
    {
	do_gameover();
    }

}

void do_hold()
{
    if(hold_last || !cfg.hold)
        return;
    hold_last = 1;
    Tetrimino* temp = in_play;
    if(hold == NULL)
        deploy_next();
    else
        deploy_hold();
    hold = temp;
}

void deploy_hold()
{
    hold->posx = 4;
    hold->posy = 2;
    hold->rotation = 0;
    in_play = hold;
    if(check_collision(*in_play)) //if it can't spawn, well, it's a game over.
    {
	do_gameover();
    }
}

/*
Sets the array of full_lines, but first it clears it.
Returns: number of lines to clear.
*/
int check_lines()
{
    free(full_lines);
    full_lines = NULL;
    int i, j;
    int lines = 0;
    for(i = 3; i < DIM_Y; ++i) //hidden lines will never be cleared.
    {
        for(j = 0; j < DIM_X; ++j)
        {
            if(!level_grid[j][i]) //if there's no block in this field
                break;  // no need to go further
        }
        if(j == 10) //line's full - there was no break before j got to 10
        {
            if(full_lines == NULL)
            {
                full_lines = calloc(DIM_Y, 1);
                //if(full_lines == NULL)
                 // panic? crash? hopefully won't happen, and if anything the system will tidy up after me
            }
            full_lines[i] = LINE_FULL;
            lines++;
        }
	if(lines == 4)
	    break;
    }
    return lines;
}



/*
Clears the full lines, if there are any.
*/
void clear_lines()
{
    if(full_lines == NULL) //nothing to clear here
        return;
    for(int i = 3; i < DIM_Y; ++i)
    {
        if(full_lines[i])
            move_down(i);
    }
}

/*
Does the actual work on the lines.
*/
void move_down(int line)
{
    int i, j;
    for(i = line; i > 0; --i)
    {
        for(j = 0; j < DIM_X; ++j)
            level_grid[j][i] = level_grid[j][i-1];
    }
    for(j = 0; j < DIM_X; ++j) //reset the top line
        level_grid[j][0] = 0;
}


Tetrimino_list* generate_bag()
{
    int types[7] = {0, 1, 2, 3, 4, 5, 6};

    for(int j = 0; j < 7; ++j)
    {
        int i = rand_lim(6);
        //do the swap
        int temp = types[j];
        types[j] = types[i];
        types[i] = temp;
    }
    //generate the bag from the info
    Tetrimino_list* first = malloc(sizeof(Tetrimino_list));
    first->tetrimino = malloc(sizeof(Tetrimino));
    first->tetrimino->type = types[0];

    Tetrimino_list* element = malloc(sizeof(Tetrimino_list));
    element->tetrimino = malloc(sizeof(Tetrimino));
    element->tetrimino->type = types[1];
    first->next = element;

    element->next = malloc(sizeof(Tetrimino_list));
    element->next->tetrimino = malloc(sizeof(Tetrimino));
    element->next->tetrimino->type = types[2];

    element = element->next;
    element->next = malloc(sizeof(Tetrimino_list));
    element->next->tetrimino = malloc(sizeof(Tetrimino));
    element->next->tetrimino->type = types[3];

    element = element->next;
    element->next = malloc(sizeof(Tetrimino_list));
    element->next->tetrimino = malloc(sizeof(Tetrimino));
    element->next->tetrimino->type = types[4];

    element = element->next;
    element->next = malloc(sizeof(Tetrimino_list));
    element->next->tetrimino = malloc(sizeof(Tetrimino));
    element->next->tetrimino->type = types[5];

    element = element->next;
    element->next = malloc(sizeof(Tetrimino_list));
    element->next->tetrimino = malloc(sizeof(Tetrimino));
    element->next->tetrimino->type = types[6];
    element->next->next = NULL;

    return first;
}

int rand_lim(int limit) {
/* return a random number between 0 and limit inclusive.
 */

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do {
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

