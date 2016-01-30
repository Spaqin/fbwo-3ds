# fbwo-3ds

<b>Falling Blocks Watch Out</b> is a 3DS homebrew Tetris clone.

The aim was to create a homebrew game inspired by Tetris DS, seeing as it was one of the best Tetris installments on handhelds, without all the fanciness new games bring.

Therefore, features include:
* SRS (Super Rotation System) - which should allow 20G to still let the player play, not just cry in a corner.
* Bag-style Random Generator - sequence of all the 7 blocks is shuffled, then put into play, so no more than 4 Z/S in a row will occur, therefore unlimited play is possible.
* Marathon mode - of course. Limited to 20 levels (hardcoded, sorry, but be reasonable) with 10 line increments (by default).
* Many variables to tweak - as of now, only in a text file. But, the number of variables is another topic in itself!
* Theme support - no support for theme per level though, only one for the entire duration.

#Settings

With the redist, I included a sample configuration file (config.cfg).
Most of them are straightforward, but let's check them out anyway.
If any of these settings is not present in the file, a default value (hardcoded) will be used instead.
* droplvl<1-20> <number1> <number2> - <1-20> is obviously affected level, number1 is how many frames the game waits for a next drop, number2 means how many lines the block is dropped in one take. 
* DAS <number> - Delayed Auto Shift - how many frames the game waits from pressing a directional button to moving the block automatically.
* DAS_speed <number> - how many frames does the game wait until moving the block again after initial DAS delay, ie. with default values: frame 0 - player presses right, block moves right once; frame 11 - still pressed, block moves right again, frame 17 - still pressed, block moves, frame 23 - again...
* invisimode <0/1> - toggles rendering of the blocks on the grid. For people who like challenges, you can turn it off.
* hold <0/1> - enables HOLD functionality.
* ghost_piece <0/1> - enables displaying of the ghost piece.
* next_displayed <0-6> - number of NEXT pieces displayed.
* glue_delay <number> - how many frames the game waits from a block reaching the bottom, and actually locking it.
* line_clear_frms <number> - how many frames line clear animation lasts
* level <1-20> - level you start with.
* lines_per_lvl <number> - lines the player needs to clear before advancing to the next level.
* theme <name> - name of the folder with your theme.

#Theme settings
In the theme folder, theme.cfg file contains the configuration of the theme. And by that, I mean positions of the elements. These should be pretty straightforward - texture, then x and y coordinates (0,0 is in top left of the screen, and the coordinates show the top left pixel of the texture)
Worth noting are:
* offset_next/offset_hold - they set tetrimino position, relatively to the frames
* lines_dig_off/level_dig_off - set y position of the digits for level/lines counters, relatively to them

#Controls
Finally, how do we play this thing?
* D-pad left/right - move the block left/right
* A - rotate the block clockwise
* B - rotate the block counterclockwise
* L - hold, if enabled
* R - "boost" DAS - the game will act as if both DAS values were halved
* START - pause, when gameover - exit
* SELECT - normally - restart, when gameover and with START - restart

#Compiling
Just use the attached makefile, but you probably will need sf2dlib and sfillib installed.
