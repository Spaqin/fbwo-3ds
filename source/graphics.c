#include "graphics.h"
#include <sf2d.h>
#include <sfil.h>
#include <stdio.h>
#include <stdbool.h>


void render_frame()
{
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        switch(mode)
        {
            case MODE_TETRIS:
                //stuff that happens always first
                sf2d_draw_texture(background.texture, background.posx, background.posy);
                sf2d_draw_texture(grid.texture, grid.posx, grid.posy);
                sf2d_draw_texture(score_text.texture, score_text.posx, score_text.posy);
                s32 score_temp = score;
                s32 i = 9;
                do{ //"do" so it can display '0' too
                    s32 digit = score_temp % 10;
                    score_temp /= 10;
                    sf2d_draw_texture(score_num[digit], score_text.posx + score_text.texture->width+score_num[digit]->width*i, score_text.posy);
                    i--;
                }
                while(score_temp);

                //level
                sf2d_draw_texture(level_frame.texture, level_frame.posx, level_frame.posy);
                if(level/10)
                {
                    sf2d_draw_texture(
                                        misc_num[level/10],
                                        level_frame.posx + (level_frame.texture->width>>1) - misc_num[level/10]->width,
                                        level_frame.posy + digit_offset_levely
                                     );
                    sf2d_draw_texture(
                                        misc_num[level%10],
                                        level_frame.posx + (level_frame.texture->width>>1),
                                        level_frame.posy + digit_offset_levely
                                     );
                }
                else
                    sf2d_draw_texture(
                                        misc_num[level],
                                        level_frame.posx + ((level_frame.texture->width>>1) - (misc_num[level]->width>>1)),
                                        level_frame.posy + digit_offset_levely
                                     );
                //lines
                sf2d_draw_texture(lines_frame.texture, lines_frame.posx, lines_frame.posy);
                if(total_lines/100)
                {
		    int currentnum = total_lines;
		//last digit
		    sf2d_draw_texture(
                                        misc_num[currentnum%10],
                                        lines_frame.posx + (lines_frame.texture->width>>1) + (misc_num[currentnum%10]->width>>1),
                                        lines_frame.posy + digit_offset_linesy
                                     );
		    currentnum = currentnum/10;
		//middle digit
		    sf2d_draw_texture(
                                        misc_num[currentnum%10],
                                        lines_frame.posx + (lines_frame.texture->width>>1) - (misc_num[currentnum%10]->width>>1),
                                        lines_frame.posy + digit_offset_linesy
                                     );
		    currentnum = currentnum/10;
		//last digit (actually this way it won't crash after getting >999, just won't display the number of thousands)
		    sf2d_draw_texture(
                                        misc_num[currentnum%10],
                                        lines_frame.posx + (lines_frame.texture->width>>1) - (misc_num[currentnum%10]->width>>1) - misc_num[currentnum%10]->width,
                                        lines_frame.posy + digit_offset_linesy
                                     );
                    
                }
                else if(total_lines/10)
                {
                    sf2d_draw_texture(
                                        misc_num[total_lines/10],
                                        lines_frame.posx + (lines_frame.texture->width>>1) - misc_num[total_lines/10]->width,
                                        lines_frame.posy + digit_offset_linesy
                                     );
                    sf2d_draw_texture(
                                        misc_num[total_lines%10],
                                        lines_frame.posx + (lines_frame.texture->width>>1),
                                        lines_frame.posy + digit_offset_linesy
                                     );
                }
                else
                    sf2d_draw_texture(
                                        misc_num[total_lines],
                                        lines_frame.posx + ((lines_frame.texture->width>>1) - (misc_num[total_lines]->width>>1)),
                                        lines_frame.posy + digit_offset_linesy
                                     );
                if(!paused && !gameover)
                {
		    if(controllable)
		    { //don't render the block while we're clearing lines.
		            if(cfg.ghost_piece)
		                render_block(get_ghost_piece(), true, false);

		            //display the block
		            render_block(*in_play, false, false);
		    }
                    if(cfg.hold)
                    {
                        sf2d_draw_texture(hold_frame.texture, hold_frame.posx, hold_frame.posy);
			if(hold)
                        sf2d_draw_texture(tetriminos[hold->type], hold_frame.posx + block_offset_holdx, hold_frame.posy + block_offset_holdy);
                    }
                    if(cfg.next_displayed)
                    {
                        sf2d_draw_texture(next_frame[0].texture, next_frame[0].posx, next_frame[0].posy);
                        sf2d_draw_texture(next_text.texture, next_text.posx, next_text.posy);
                        sf2d_draw_texture(
                                            tetriminos[next_blocks->tetrimino->type],
                                            next_frame[0].posx + block_offset_nextx,
                                            next_frame[0].posy + block_offset_nexty
                                         );
                        Tetrimino_list* element = next_blocks;
                        for(int i = 1; i < cfg.next_displayed; ++i)
                        {
                            element = element->next;
                            sf2d_draw_texture(next_frame[i].texture, next_frame[i].posx, next_frame[i].posy);
                            sf2d_draw_texture(
                                            tetriminos[element->tetrimino->type],
                                            next_frame[i].posx + block_offset_nextx,
                                            next_frame[i].posy + block_offset_nexty
                                             );
                        }
                    }

                    if(last_deployed)
                    {
                        render_block(*last_deployed, false, true);
                        free(last_deployed);
                        last_deployed = NULL;
                    }
                    if(!cfg.invisimode)
                    {
			render_grid_blocks();
                    }
                    if(!render_line_clear)
                        remove_line_count = 0;
                    else
                    {
                        if(remove_line_count == cfg.line_clear_frames)
                        {
                            render_line_clear = 0;
                            update_level();
                            controllable = 1;
                        }
                        else
                        {
                            controllable = 0;
                        //placeholder animation
                            for(int i = 4; i < DIM_Y; ++i)
                            {
                                if(full_lines[i])
                                {
                                    sf2d_draw_rectangle(
                                                        grid.posx,
                                                        grid.posy + (i-4) * blocks[0]->height,
                                                        grid.texture->width,
                                                        blocks[0]->height,
                                                        RGBA8(0xFF, rand() % 255, rand() % 255, rand() % 255)
                                                       );
                                }
                            }
                            remove_line_count++;
                        }
                    }
                }//end not paused/gameover if
                else if(paused)
                {
                    sf2d_draw_texture(paused_text, 200 - (paused_text->width>>1), 120 - (paused_text->height>>1));
                }
                else //must be game over
                {
		    render_grid_blocks();
                    sf2d_draw_texture(gameover_text, 200 - (gameover_text->width>>1), 120 - (gameover_text->height>>1));

                }


        }
    sf2d_end_frame();
}

void render_grid_blocks()
{
	for(int i = 4; i < DIM_Y; ++i)
	{
	    if(render_line_clear && full_lines[i])
		continue;
	    for(int j = 0; j < DIM_X; ++j)
	    {
		int type;
		if((type = level_grid[j][i]))
		    sf2d_draw_texture(blocks[type-1], grid.posx + blocks[type-1]->width * j, grid.posy + blocks[type-1]->height * (i-4));
	    }
	}
}


/*
Loads necessary textures given the template in "printf" format.
Must include "%s" at the end.
Returns if the load was successful or not.j
*/
int load_textures(const char* str_template)
{
    char buffer[80];
    //load background
    sprintf(buffer, str_template, "background.png");
    background.texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    background.posx = 0;
    background.posy = 0;
    if(!background.texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }
    sprintf(buffer, str_template, "grid.png");
    grid.texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    grid.posx = DEFAULT_GRIDX;
    grid.posy = DEFAULT_GRIDY;
    if(!grid.texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "next_text.png");
    next_text.texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    next_text.posx = DEFAULT_NEXT_TEXTX;
    next_text.posy = DEFAULT_NEXT_TEXTY;
    if(!next_text.texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "next_frame0.png");
    next_frame[0].texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    next_frame[0].posx = DEFAULT_NEXT_FRAME_1X;
    next_frame[0].posy = DEFAULT_NEXT_FRAME_1Y;
    if(!next_frame[0].texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "next_frame1.png");
    next_frame[1].texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    next_frame[1].posx = DEFAULT_NEXT_FRAME_2X;
    next_frame[1].posy = DEFAULT_NEXT_FRAME_2Y;
    if(!next_frame[1].texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "next_frame2.png");
    next_frame[2].texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    next_frame[2].posx = DEFAULT_NEXT_FRAME_3X;
    next_frame[2].posy = DEFAULT_NEXT_FRAME_3Y;
    if(!next_frame[2].texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "next_frame3.png");
    next_frame[3].texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    next_frame[3].posx = DEFAULT_NEXT_FRAME_4X;
    next_frame[3].posy = DEFAULT_NEXT_FRAME_4Y;
    if(!next_frame[3].texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "next_frame4.png");
    next_frame[4].texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    next_frame[4].posx = DEFAULT_NEXT_FRAME_5X;
    next_frame[4].posy = DEFAULT_NEXT_FRAME_5Y;
    if(!next_frame[4].texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "next_frame5.png");
    next_frame[5].texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    next_frame[5].posx = DEFAULT_NEXT_FRAME_6X;
    next_frame[5].posy = DEFAULT_NEXT_FRAME_6Y;
    if(!next_frame[5].texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "score_text.png");
    score_text.texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    score_text.posx = DEFAULT_SCORE_TEXTX;
    score_text.posy = DEFAULT_SCORE_TEXTY;
    if(!score_text.texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "lines_frame.png");
    lines_frame.texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    lines_frame.posx = DEFAULT_LINES_FRAMEX;
    lines_frame.posy = DEFAULT_LINES_FRAMEY;
    if(!lines_frame.texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "level_frame.png");
    level_frame.texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    level_frame.posx = DEFAULT_LEVEL_FRAMEX;
    level_frame.posy = DEFAULT_LEVEL_FRAMEY;
    if(!level_frame.texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "hold_frame.png");
    hold_frame.texture = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    hold_frame.posx = DEFAULT_HOLD_FRAMEX;
    hold_frame.posy = DEFAULT_HOLD_FRAMEY;
    if(!level_frame.texture)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }
    int i;

    char temp_buffer[80];
    for(i = 0; i < 7; ++i)
    {
        sprintf(temp_buffer, "tetrimino%d.png", i);
        sprintf(buffer, str_template, temp_buffer);
        tetriminos[i] = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
        if(!tetriminos[i])
        {
            printf("error loading %s\n", buffer);
            return 0;
        }
        sprintf(temp_buffer, "block%d.png", i);
        sprintf(buffer, str_template, temp_buffer);
        blocks[i] = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
        if(!blocks[i])
        {
            printf("error loading %s\n", buffer);
            return 0;
        }
        sprintf(temp_buffer, "score_num%d.png", i);
        sprintf(buffer, str_template, temp_buffer);
        score_num[i] = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
        if(!score_num[i])
        {
            printf("error loading %s\n", buffer);
            return 0;
        }
        sprintf(temp_buffer, "misc_num%d.png", i);
        sprintf(buffer, str_template, temp_buffer);
        misc_num[i] = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
        if(!misc_num[i])
        {
            printf("error loading %s\n", buffer);
            return 0;
        }
    }
    for (i = 7; i < 10; ++i)
    {
        sprintf(temp_buffer, "score_num%d.png", i);
        sprintf(buffer, str_template, temp_buffer);
        score_num[i] = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
        if(!score_num[i])
        {
            printf("error loading %s\n", buffer);
            return 0;
        }
        sprintf(temp_buffer, "misc_num%d.png", i);
        sprintf(buffer, str_template, temp_buffer);
        misc_num[i] = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
        if(!misc_num[i])
        {
            printf("error loading %s\n", buffer);
            return 0;
        }
    }

    sprintf(buffer, str_template, "gameover.png");
    gameover_text = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    if(!gameover_text)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    sprintf(buffer, str_template, "paused.png");
    paused_text = sfil_load_PNG_file(buffer, SF2D_PLACE_RAM);
    if(!paused_text)
    {
        printf("error loading %s\n", buffer);
        return 0;
    }

    block_offset_holdx = DEFAULT_OFFSET_HOLDX;
    block_offset_holdy = DEFAULT_OFFSET_HOLDY;
    block_offset_nextx = DEFAULT_OFFSET_NEXTX;
    block_offset_nexty = DEFAULT_OFFSET_NEXTY;
    digit_offset_linesy = DEFAULT_OFFSET_LNSY;
    digit_offset_levely = DEFAULT_OFFSET_LVLY;
    return 1;
}


void graphics_init()
{
    sf2d_init();
    sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0xFF));
    consoleInit(GFX_BOTTOM, NULL);
}




void graphics_parse_config(char* theme_template)
{
    char buffer[80];
    char command[32];
    s32 valx, valy;

    sprintf(buffer, theme_template, "theme.cfg");
    FILE* cfgf = fopen(buffer, "r");
    if(!cfgf)
    {
	printf("error opening %s", buffer);
	return;
    }
    while(fgets(buffer, 79, cfgf))
    {
        
        if(sscanf(buffer, "%s %ld %ld", command, &valx, &valy) == 3)
        {
            if(!strcmp(command, "grid"))
            {
                printf("grid = [%ld, %ld]\n", valx, valy);
                grid.posx = valx;
                grid.posy = valy;
            }

            else if(!strcmp(command, "next_text"))
            {
                printf("next_text = [%ld, %ld]\n", valx, valy);
                next_text.posx = valx;
                next_text.posy = valy;
            }
            else if(!strcmp(command, "next_frame0"))
            {
                printf("next_frame0 = [%ld, %ld]\n", valx, valy);
                next_frame[0].posx = valx;
                next_frame[0].posy = valy;
            }
            else if(!strcmp(command, "next_frame1"))
            {
                printf("next_frame1 = [%ld, %ld]\n", valx, valy);
                next_frame[1].posx = valx;
                next_frame[1].posy = valy;
            }
            else if(!strcmp(command, "next_frame2"))
            {
                printf("next_frame2 = [%ld, %ld]\n", valx, valy);
                next_frame[2].posx = valx;
                next_frame[2].posy = valy;
            }
            else if(!strcmp(command, "next_frame3"))
            {
                printf("next_frame3 = [%ld, %ld]\n", valx, valy);
                next_frame[3].posx = valx;
                next_frame[3].posy = valy;
            }
            else if(!strcmp(command, "next_frame4"))
            {
                printf("next_frame4 = [%ld, %ld]\n", valx, valy);
                next_frame[4].posx = valx;
                next_frame[4].posy = valy;
            }
            else if(!strcmp(command, "next_frame5"))
            {
                printf("next_frame5 = [%ld, %ld]\n", valx, valy);
                next_frame[5].posx = valx;
                next_frame[5].posy = valy;
            }
            else if(!strcmp(command, "score_text"))
            {
                printf("score_text = [%ld, %ld]\n", valx, valy);
                score_text.posx = valx;
                score_text.posy = valy;
            }
            else if(!strcmp(command, "lines_frame"))
            {
                printf("lines_frame = [%ld, %ld]\n", valx, valy);
                lines_frame.posx = valx;
                lines_frame.posy = valy;
            }
            else if(!strcmp(command, "level_frame"))
            {
                printf("level_frame = [%ld, %ld]\n", valx, valy);
                level_frame.posx = valx;
                level_frame.posy = valy;
            }
            else if(!strcmp(command, "hold_frame"))
            {
                printf("hold_frame = [%ld, %ld]\n", valx, valy);
                hold_frame.posx = valx;
                hold_frame.posy = valy;
            }
            else if(!strcmp(command, "offset_next"))
            {
                printf("offset_next = [%ld, %ld]\n", valx, valy);
                block_offset_nextx = valx;
                block_offset_nexty = valy;
            }
            else if(!strcmp(command, "offset_hold"))
            {
                printf("offset_hold = [%ld, %ld]\n", valx, valy);
                block_offset_holdx = valx;
                block_offset_holdy = valy;
            }
        }
        else if(sscanf(buffer, "%s %ld", command, &valx) == 2)
        {
	    if(!strcmp(command, "lines_dig_off"))
            {
                printf("lines digit offset = %ld\n", valx);
                digit_offset_linesy = valx;
            }
	    else if(!strcmp(command, "level_dig_off"))
            {
                printf("level digit offset = %ld\n", valx);
                digit_offset_levely = valx;
            }
	}
    }
}

void render_block(Tetrimino to_render, bool ghost_piece, bool lastdepl)
{
    if(to_render.type != I_TYPE)
    {
        int i, j;
        for(i = 0; i < 3; ++i)
            for(j = 0; j < 3; ++j)
            {
                if(rotations[to_render.type][to_render.rotation][j][i])
                {
		    if(to_render.posy + j < 4)
			continue;
                    s32 x = grid.posx + blocks[to_render.type]->width * (to_render.posx + i);
                    s32 y = grid.posy + blocks[to_render.type]->height * (to_render.posy - 4 + j);
                    if(ghost_piece)
                        sf2d_draw_texture_blend(blocks[to_render.type], x, y, 0x88AAAAAA);
                    else if(lastdepl)
                        sf2d_draw_texture_blend(blocks[to_render.type], x, y, 0xFFFFFFFF);
                    else
                        sf2d_draw_texture_blend(blocks[to_render.type], x, y, 0xFFEEEEEE);
                }
            }
    }
    else
    {
        int i, j;
        for(i = 0; i < 5; ++i)
            for(j = 0; j < 5; ++j)
            {
                if(rotation_I[to_render.rotation][j][i])
                {
		    if(to_render.posy + j < 4)
			continue;
                    s32 x = grid.posx + blocks[I_TYPE]->height * (to_render.posx + i);
                    s32 y = grid.posy + blocks[I_TYPE]->width * (to_render.posy - 4 + j);
                    if(ghost_piece)
                        sf2d_draw_texture_blend(blocks[I_TYPE], x, y, 0x88AAAAAA);
                    else if(lastdepl)
                        sf2d_draw_texture_blend(blocks[I_TYPE], x, y, 0xFFFFFFFF);
                    else
                        sf2d_draw_texture_blend(blocks[I_TYPE], x, y, 0xFFDDDDDD);
                }
            }
    }
}
void graphics_fini()
{
    sf2d_free_texture(background.texture);
    sf2d_free_texture(next_text.texture);
    sf2d_free_texture(grid.texture);
    sf2d_free_texture(score_text.texture);
    sf2d_free_texture(lines_frame.texture);
    sf2d_free_texture(level_frame.texture);
    sf2d_free_texture(hold_frame.texture);
    int i = 0;
    for (; i < 6; ++i)
    {
        sf2d_free_texture(next_frame[i].texture);
        sf2d_free_texture(tetriminos[i]);
        sf2d_free_texture(blocks[i]);
        sf2d_free_texture(score_num[i]);
        sf2d_free_texture(misc_num[i]);
    }
    sf2d_free_texture(tetriminos[6]);
    sf2d_free_texture(blocks[6]);
    for (; i < 10; ++i)
    {
        sf2d_free_texture(score_num[i]);
        sf2d_free_texture(misc_num[i]);
    }
    sf2d_free_texture(gameover_text);
    sf2d_free_texture(paused_text);
    sf2d_fini();
}
