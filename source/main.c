
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <stdbool.h>
#include "structs.h"
#include "level.h"
#include "graphics.h"


#define STACKSIZE 4096



Configuration cfg;

u8 paused = 0; //doesn't necessarily mean that the game is paused
//could be that we're waiting for removal animation to finish
u8 controllable = 1; //ie. during line removal it's set to 0 so stuff won't break;
u8 playable = 1;

u8 mode = MODE_TETRIS;

extern u8 level;

extern u32 high_score;

//controls related variables
u8 start_held = 0;
u8 A_held = 0;
u8 B_held = 0;
u8 L_held = 0;
u8 UP_held = 0;
u8 RIGHT_pressed = 0;
u8 LEFT_pressed = 0;
u32 RIGHT_DAS_count;
u32 LEFT_DAS_count;
u32 RIGHT_DAS_speed_count = 0;
u32 LEFT_DAS_speed_count = 0;

u8 restartpls = 0;
u8 config_lvl = 1;

char theme_template[64] = "fbwodata/default/%s";

void parse_config(FILE* config_file)
{
    char buffer[80];
    char theme_folder_name[60];
    char command[20];

    int val, val2;
    while(fgets(buffer, 79, config_file))
    {
	if(sscanf(buffer, "%20s %d %d", command, &val, &val2) == 3)
	{
	    if(!strcmp(command, "droplvl1\0"))
	    {
		printf("drop for lvl 1: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[0] = val;
		cfg.rows_per_drop[0] = val2;
	    }
	    else if(!strcmp(command, "droplvl2\0"))
	    {
		printf("drop for lvl 2: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[1] = val;
		cfg.rows_per_drop[1] = val2;
	    }
	    else if(!strcmp(command, "droplvl3\0"))
	    {
		printf("drop for lvl 3: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[2] = val;
		cfg.rows_per_drop[2] = val2;
	    }
	    else if(!strcmp(command, "droplvl4\0"))
	    {
		printf("drop for lvl 4: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[3] = val;
		cfg.rows_per_drop[3] = val2;
	    }
	    else if(!strcmp(command, "droplvl5\0"))
	    {
		printf("drop for lvl 5: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[4] = val;
		cfg.rows_per_drop[4] = val2;
	    }
	    else if(!strcmp(command, "droplvl6\0"))
	    {
		printf("drop for lvl 6: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[5] = val;
		cfg.rows_per_drop[5] = val2;
	    }
	    else if(!strcmp(command, "droplvl7\0"))
	    {
		printf("drop for lvl 7: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[6] = val;
		cfg.rows_per_drop[6] = val2;
	    }
	    else if(!strcmp(command, "droplvl8\0"))
	    {
		printf("drop for lvl 8: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[7] = val;
		cfg.rows_per_drop[7] = val2;
	    }
	    else if(!strcmp(command, "droplvl9\0"))
	    {
		printf("drop for lvl 9: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[8] = val;
		cfg.rows_per_drop[8] = val2;
	    }
	    else if(!strcmp(command, "droplvl10\0"))
	    {
		printf("drop for lvl 10: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[9] = val;
		cfg.rows_per_drop[9] = val2;
	    }
	    else if(!strcmp(command, "droplvl11\0"))
	    {
		printf("drop for lvl 11: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[10] = val;
		cfg.rows_per_drop[10] = val2;
	    }
	    else if(!strcmp(command, "droplvl12\0"))
	    {
		printf("drop for lvl 12: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[11] = val;
		cfg.rows_per_drop[11] = val2;
	    }
	    else if(!strcmp(command, "droplvl13\0"))
	    {
		printf("drop for lvl 13: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[12] = val;
		cfg.rows_per_drop[12] = val2;
	    }
	    else if(!strcmp(command, "droplvl14\0"))
	    {
		printf("drop for lvl 14: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[13] = val;
		cfg.rows_per_drop[13] = val2;
	    }
	    else if(!strcmp(command, "droplvl15\0"))
	    {
		printf("drop for lvl 15: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[14] = val;
		cfg.rows_per_drop[14] = val2;
	    }
	    else if(!strcmp(command, "droplvl16\0"))
	    {
		printf("drop for lvl 16: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[15] = val;
		cfg.rows_per_drop[15] = val2;
	    }
	    else if(!strcmp(command, "droplvl17\0"))
	    {
		printf("drop for lvl 17: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[16] = val;
		cfg.rows_per_drop[16] = val2;
	    }
	    else if(!strcmp(command, "droplvl18\0"))
	    {
		printf("drop for lvl 18: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[17] = val;
		cfg.rows_per_drop[17] = val2;
	    }
	    else if(!strcmp(command, "droplvl19\0"))
	    {
		printf("drop for lvl 19: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[18] = val;
		cfg.rows_per_drop[18] = val2;
	    }
	    else if(!strcmp(command, "droplvl20\0"))
	    {
		printf("drop for lvl 20: %d frames, %d rows at once\n", val, val2);
		cfg.frames_per_drop[19] = val;
		cfg.rows_per_drop[19] = val2;
	    }
	}
        else if(sscanf(buffer, "%20s %d", command, &val) == 2)
        {
            if(!strcmp(command, "DAS\0"))
            {
                printf("DAS delay = %d frames\n", val);
                cfg.DAS = val;
            }
            else if(!strcmp(command, "invisimode\0"))
            {
                printf("invisimode: ");
                if(val)
                    printf("on\n");
                else
                    printf("off\n");
                cfg.invisimode = val;
            }
            else if(!strcmp(command, "hold\0"))
            {
                printf("hold: ");
                if(val)
                    printf("on\n");
                else
                    printf("off\n");
                cfg.hold = val;
            }
            else if(!strcmp(command, "ghost_piece\0"))
            {
                printf("ghost piece: ");
                if(val)
                    printf("on\n");
                else
                    printf("off\n");
                cfg.ghost_piece = val;
            }
            else if(!strcmp(command, "next_displayed\0"))
	    {
                if(val >= 0 && val < 7)
                {
                    printf("next displayed pieces no. %d\n", val);
                    cfg.next_displayed = val;
                }
	    }
            else if(!strcmp(command, "glue_delay\0"))
            {
                printf("Lock delay = %d frames\n", val);
                cfg.glue_delay = val;
            }
            else if(!strcmp(command, "DAS_speed\0"))
            {
                printf("DAS speed = %d frames\n", val);
                cfg.DAS_speed = val;
            }
            else if(!strcmp(command, "line_clear_frms\0"))
            {
                printf("Line clear lasts = %d frames\n", val);
                cfg.line_clear_frames = val;
            }
            else if(!strcmp(command, "level\0"))
	    {
                if(level > 0 && level <= 20)
                {
                    printf("level = %d\n", val);
                    config_lvl = val;
                }
	    }
            else if(!strcmp(command, "lines_per_lvl\0"))
	    {
                if(val > 0)
                {
                    printf("lines per level = %d\n", val);
                    cfg.lines_per_lvl = val;
		}
	    }
        }
        else if(sscanf(buffer, "%15s %60s", command, theme_folder_name) == 2)
        {
            if(!strcmp("theme", command));
            {
                printf("theme %s\n", theme_folder_name);
                sprintf(theme_template, "fbwodata/%s/%%s", theme_folder_name);
            }
        }
    }
    fclose(config_file);
}

void tetris_control(u32 kDown)
{
    if (kDown & KEY_START)
    {
        if(!start_held)
        {
	    if(gameover)
	    {
		playable = 0;
		return;
	    }
            paused = !paused;
            start_held = 1;

        }
    }

    else
        start_held = 0;
    if (kDown & KEY_SELECT)
    {
	if(!gameover && (kDown & KEY_START))
	    playable = 0;
	if(gameover && playable)
	    restartpls = 1;
    }
    if(!paused && controllable && !gameover)
    {
        if(kDown & KEY_A)
        {
            if(!A_held)
            {
                rotate_clockwise();
                A_held = 1;
            }
        }
        else
            A_held = 0;
        if(kDown & KEY_B)
        {
            if(!B_held)
            {
                rotate_counterclockwise();
                B_held = 1;
            }
        }
        else
            B_held = 0;
        if(kDown & KEY_L && cfg.hold)
        {
            if(!L_held)
            {
                do_hold();
                L_held = 1;
            }
        }
        else
            L_held = 0;
        if(kDown & KEY_UP)
        {
            if(!UP_held)
            {
                go_all_down();
                UP_held = 1;
            }
        }
        else
            UP_held = 0;
        if(kDown & KEY_DOWN)
        {
            soft_drop();
        }
        if(kDown & KEY_RIGHT)
        {
            if(!RIGHT_pressed)
            {
                go_right();
                RIGHT_DAS_count--;
                RIGHT_pressed = 1;
            }
            else
            {
                if(RIGHT_DAS_count <= 0) //yes, it can go below 0
                {
                    if(RIGHT_DAS_speed_count == 0)
                    {
                        go_right();
                    }
                    if(kDown & KEY_R)
			if(RIGHT_DAS_speed_count > (cfg.DAS_speed >> 1))
			    RIGHT_DAS_speed_count = 0;
			else
                            RIGHT_DAS_speed_count = (RIGHT_DAS_speed_count + 1) % (cfg.DAS_speed>>1); //boost the DAS!
                    else
                        RIGHT_DAS_speed_count = (RIGHT_DAS_speed_count + 1) % cfg.DAS_speed;
                }
                else
                {
                    if(kDown & KEY_R)
                        RIGHT_DAS_count--; //boost the DAS!
                    RIGHT_DAS_count--;
                } // end RIGHT_DAS_count if
            }//end right pressed if
        }
        else
        {
            //reset values
            RIGHT_DAS_count = cfg.DAS;
            RIGHT_DAS_speed_count = 0;
            RIGHT_pressed = 0;
        }//end right if
        if(kDown & KEY_LEFT)
        {
            if(!LEFT_pressed)
            {
                go_left();
                LEFT_DAS_count--;
                LEFT_pressed = 1;
            }
            else
            {
                if(LEFT_DAS_count <= 0)
                {
                    if(LEFT_DAS_speed_count == 0)
                    {
                        go_left();
                    }
                    if(kDown & KEY_R)
			if(LEFT_DAS_speed_count > (cfg.DAS_speed >> 1))
			    LEFT_DAS_speed_count = 0;
		        else
                            LEFT_DAS_speed_count = (LEFT_DAS_speed_count + 1) % (cfg.DAS_speed>>1); //boost the DAS!
                    else
                        LEFT_DAS_speed_count = (LEFT_DAS_speed_count + 1) % cfg.DAS_speed;
                }
                else
                {
                    if(kDown & KEY_R)
                        LEFT_DAS_count--; //boost the DAS!
                    LEFT_DAS_count--;
                } // end LEFT_DAS_count if
            }//end LEFT pressed if
        }
        else
        {
            //reset values
            LEFT_DAS_count = cfg.DAS;
            LEFT_DAS_speed_count = 0;
            LEFT_pressed = 0;
        }//end LEFT if


    }
}

int main()
{
    graphics_init();

    //init config w/ def. values
    cfg.DAS = 11;
    cfg.DAS_speed = 6;
    cfg.next_displayed = 5;
    cfg.invisimode = 0;
    cfg.glue_delay = 30;
    cfg.hold = 1;
    cfg.line_clear_frames = 40;
    cfg.lines_per_lvl = 10;
    level = 1;
	// level:             1   2   3   4   5   6   7   8  9  10 11 12 13 14 15 16 17 19 20 
    static const u32 fpd[] = {30, 28, 27, 24, 20, 15, 10, 8, 5, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1};
    memcpy(cfg.frames_per_drop, fpd, sizeof(u32)*20); 
	// level:             1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17  18  19  20
    static const u32 rpd[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 6, 8, 10, 12, 15, 20};
    memcpy(cfg.rows_per_drop, rpd, sizeof(u32)*20); 

    //load textures

    printf("reading config...\n");
    FILE* config = fopen("fbwodata/config.cfg", "r");
    if(config != NULL)
        parse_config(config);
    else
        printf("failed to read config! default values will be used.\n");
    if(!load_textures(theme_template))
	goto texture_error;
    RIGHT_DAS_count = cfg.DAS;
    LEFT_DAS_count = cfg.DAS;

    graphics_parse_config(theme_template);

    //game init
    init:
    initialize_game();
    paused = 1;
    playable = 1;
    restartpls = 0;
    level = config_lvl;
    load_highscore();

    printf("Game ready! Press START to uh.. start?\n");

    while(aptMainLoop() && playable)
    {
    //controls
        hidScanInput();
        u32 kDown = hidKeysHeld();
        switch(mode)
        {
            case MODE_TETRIS:
                tetris_control(kDown);
		if(!paused && controllable && !gameover)
		    do_gravity();
                render_frames();
		
                break;
            //the following are still in "to-do" - not critical to gameplay
            case MODE_MENU:
		break;
            case MODE_SETTINGS:
		break;
            
        }
	sf2d_swapbuffers();
    }
    if(restartpls)
	goto init;
    goto exit;


    texture_error:
    printf("error loading textures! missing files?\n");

    printf("press START to exit.\n");
    while(aptMainLoop())
    {
        hidScanInput();
        u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
    }

    exit:
    save_highscore();
    printf("exitting...\n");
    graphics_fini();
    return 0;
}
