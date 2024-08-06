/****************************************************************************
 *  Written by John M Dow
 *  
 *  This is Shareware. This means that you don't have to pay for it, 
 *  but can't change it, or say it's yours.
 *
 *  So there!
 *****************************************************************************
 * Warning: some of this code is really hideous, and is only to be studied
 * by the most paranoid. You and I would both be happier if you didn't read
 * this!
 *****************************************************************************
 */

#include<stdlib.h>
#include<stdio.h>
#include"svgalib_compat.h"
#include"soundIt.h"

#define NO_OF_LEVELS        19     /* 20 levels */

#define YES                 0
#define NO                  1
#define ON                  1
#define OFF                 0


/* Constants defining fontdata file */
#define f_A                 0
#define f_B                 1
#define f_C                 2
#define f_D                 3
#define f_I                 4
#define f_L                 5
#define f_O                 6
#define f_P                 7
#define f_Q                 8
#define f_R                 9
#define f_S                10
#define f_Z                11
#define f_UP               12
#define f_DOWN             13
#define f_RIGHT            14
#define f_LEFT             15
#define f_NOSOUND          16
#define f_SOUND            17
#define f_FALLING          18
#define f_NOTFALLING       19
#define f_X                20
#define f_RESET            21
#define f_ABORT            22
#define f_ESC              23
#define f_WRIT1            25
#define f_WRIT2            26
#define f_QUIT             27
#define f_PLAY             28
#define f_PLUS             29
#define f_MINUS            30
#define f_LEVEL            31
#define f_EDIT_ICON        32
#define f_MAXBLOCK         33

/*
 * 
 * Position of sound effect in array
 * 
 */
#define BOOM                0
#define ROAR                1
#define SPLAT               2
#define WHEEP               3
#define SCREAM              4
#define SLURP               5
#define CLICK               6
#define GETYE               7
#define MAPSOUND            8
#define EXPLODE             9
#define TOC                 10
#define END                 11
#define GRIND               12
#define OW                  13
#define WHINE               14
#define SLOOP               15
#define FLUSH               16

/* States for gravity map */
#define FALLING             -2
#define STATIONARY          -1

/* 
 * 
 * Positions of images in array.
 * 4 frames per image for animation
 * 
 * 
 */
#define SPACE               0
#define BRICK               4
#define SAND                8
#define ROCK                12
#define DIAMOND             16
#define BEAST               24
#define OUTSIDE             28
#define CHANGER             32
#define MORPHBLOCK          36
#define VIRUS               40
#define EGG                 44
#define MAP                 48
#define PRONGSLEFT          52
#define PRONGSRIGHT         56
#define PRONGSUP            60
#define PRONGSDOWN          20
#define BOMB                68
#define CUBE                72
#define SMILEY              76  /* 16 frames of smiley ! */
#define SNAKE               92
#define MAXBLOCK            96

/* Handy constants */
#define MAXX                32
#define MAXY                20
#define MAXSAMP             16
#define SAMPRATE            8000         /* kHz */
#define NO_OF_CHANNELS      8

typedef struct {
    int x, y, no_of_diamonds, no_of_morphers;
} start_t;

/* All this stuff is global (ooohhh) so leave alone */
unsigned char graphics[MAXBLOCK][32][32];
unsigned char map_graphics[MAXBLOCK][10][10];
unsigned char f_graphics[f_MAXBLOCK][32][32];
int levels[20][64][48];
int current_level[64][48];
int gravity[64][48];
int current = 0;
int animframe = 0;
int manimframe = 0;
int score = 0;
unsigned char score_block[4];
int diamondcount = 0;
int DIE = 0;
int prongsx, prongsy;
int LIVES = 3;
int CHANGE = OFF;
int Virusmoved = NO;
int Monstermoved = NO;
int Cubemoved = NO;
int current_channel = 1;
int HASMAP = NO;
int PRONGS = 20;
int SOUND = OFF;
int upto500 = 0;
int FALLING_DIAMONDS = ON;

Sample samps[20];

const int GAME_FPS = 8;
const int TITLE_FPS = 20;
const int EDITOR_FPS = 8;
const int frameDelayGame = 1000 / GAME_FPS;
const int frameDelayTitle = 1000 / TITLE_FPS;
const int frameDelayEditor = 1000 / EDITOR_FPS;

void sound(int snd_no) {

    /* This is so that important noises do not get blanketed */
    if (SOUND == ON) {
        if (snd_no == GETYE || snd_no == END || snd_no == EXPLODE) {
            Snd_effect(snd_no, 0);
        } else {
            /*
	  * NO_OF_CHANNELS ensures that each sample gets as long
	  * as possible to play
	  */
            Snd_effect(snd_no, current_channel);
            current_channel++;
            if (current_channel > NO_OF_CHANNELS - 1) {
                current_channel = 1;
            }
        }
    }
}


/*
 * Reads file "data32" into graphics array
 * Graphics are stored simply as array[i][j] where [i][j] is the color of
 * (i,j)
 */
void load_graphics(void) {

    int graphic, row, col;
    FILE *infile;
    printf("Loading graphics....");
    infile = fopen("data32", "rb");
    for (graphic = 0; graphic < MAXBLOCK; graphic++) {
        for (col = 0; col < 32; col++) {
            for (row = 0; row < 32; row++) {
                fread(&(graphics[graphic][col][row]), 1, 1, infile);
            }
        }
    }
    fclose(infile);
    printf("done\n");
}

/* Loads textdata for intro */
void load_textdata(void) {

    int graphic, row, col;
    FILE *infile;
    printf("Loading textdata....");
    infile = fopen("textdata", "rb");
    for (graphic = 0; graphic < f_MAXBLOCK; graphic++) {
        for (col = 0; col < 32; col++) {
            for (row = 0; row < 32; row++) {
                fread(&(f_graphics[graphic][col][row]), 1, 1, infile);
            }
        }
    }
    fclose(infile);
    printf("done\n");
}


/*
 *
 * Reads "data" into map graphics array
 *
 */
void load_map_graphics(void) {

    int graphic, row, col;
    FILE *infile;

    infile = fopen("data", "rb");
    for (graphic = 0; graphic < MAXBLOCK; graphic++) {
        for (col = 0; col < 10; col++) {
            for (row = 0; row < 10; row++) {
                fread(&(map_graphics[graphic][col][row]), 1, 1, infile);
            }
        }
    }
    fclose(infile);
}


/*
 * Reads file "levels" into levels array
 *
 */
void load_levels(void) {

    int level, row, col;
    FILE *infile;
    infile = fopen("levelshi", "rb");
    for (level = 0; level < 20; level++) {
        for (col = 0; col < 64; col++) {
            for (row = 0; row < 48; row++) {
                fread(&(levels[level][col][row]), 1, 1, infile);
            }
        }
    }
    fclose(infile);
}


/*
 *
 * Loads samples into sample bank. These are 8000Hz .ub's
 *
 */
void load_samples(void) {
    Snd_loadRawSample("boom.ub", &(samps[BOOM]));
    Snd_loadRawSample("roar.ub", &(samps[ROAR]));
    Snd_loadRawSample("splat.ub", &(samps[SPLAT]));
    Snd_loadRawSample("wheep.ub", &(samps[WHEEP]));
    Snd_loadRawSample("scream.ub", &(samps[SCREAM]));
    Snd_loadRawSample("slurp.ub", &(samps[SLURP]));
    Snd_loadRawSample("click.ub", &(samps[CLICK]));
    Snd_loadRawSample("getye.ub", &(samps[GETYE]));
    Snd_loadRawSample("mapsound.ub", &(samps[MAPSOUND]));
    Snd_loadRawSample("explode.ub", &(samps[EXPLODE]));
    Snd_loadRawSample("toc.ub", &(samps[TOC]));
    Snd_loadRawSample("end.ub", &(samps[END]));
    Snd_loadRawSample("grind.ub", &(samps[GRIND]));
    Snd_loadRawSample("ow.ub", &(samps[OW]));
    Snd_loadRawSample("whine.ub", &(samps[WHINE]));
    Snd_loadRawSample("sloop.ub", &(samps[SLOOP]));
    Snd_loadRawSample("flush.ub", &(samps[FLUSH]));
}


/*
 * Simply initialises gravity map
 */
void reset_gravity(void) {
    int i, j;
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 48; j++) {
            gravity[i][j] = STATIONARY;
        }
    }
}


/* Makes a duplicate of the current level to fart around with */
void copy_level(int level_no) {
    int col, row;
    for (col = 0; col < 64; col++) {
        for (row = 0; row < 48; row++) {
            current_level[col][row] = levels[level_no][col][row];
        }
    }
}


/*
 * Sets things up
 */
void initialise(void) {
    vga_init();
//    vga_screenoff();
    vga_setmode(10);
    load_samples();
}


/*
 * Finds start position in level and works out no of diamonds to b collected
 */
start_t find_start(int lev) {
    start_t coords;
    int col, row;
    coords.no_of_diamonds = 0;
    coords.no_of_morphers = 0;
    coords.x = 0;
    coords.y = 0;
    for (col = 0; col < 64; col++) {
        for (row = 0; row < 48; row++) {
            if (levels[lev][col][row] == DIAMOND) {
                coords.no_of_diamonds++;
            }
            if (levels[lev][col][row] == MORPHBLOCK) {
                coords.no_of_morphers++;
            }
            /* Yes, I know this isn't pretty, but it stops the amazing
          * Duplicated prongs gag!
          */
            if ((levels[lev][col][row] == PRONGS) ||
                (levels[lev][col][row] == PRONGSRIGHT) ||
                (levels[lev][col][row] == PRONGSLEFT) ||
                (levels[lev][col][row] == PRONGSUP) ||
                (levels[lev][col][row] == PRONGSDOWN)) {
                coords.x = col;
                coords.y = row;
            }
        }
    }
    return coords;
}


/* Does all the work */
void update_level(void) {
    int posx, posy;
    int col, row;
    for (col = 63; col > -1; col--) {
        for (row = 47; row > -1; row--) {


            /* If a morphblock and changer is on */
            if (*((*(current_level + col)) + row) == MORPHBLOCK) {
                if (CHANGE == ON) {
                    *((*(current_level + col)) + row) = DIAMOND;
                }
            }

                /* If a virus type thingy!  */
            else if (*((*(current_level + col)) + row) == VIRUS && Virusmoved == NO) {

                /* Check and expand in virusdir */
                if (*((*(current_level + (col - 1))) + row) == SPACE && animframe == 0) {
                    *((*(current_level + (col - 1))) + row) = VIRUS;
                    sound(SLOOP);
                    Virusmoved = YES;
                } else if (*((*(current_level + (col + 1))) + row) == SPACE && animframe == 1) {
                    *((*(current_level + (col + 1))) + row) = VIRUS;
                    sound(SLOOP);
                    Virusmoved = YES;
                } else if (current_level[col][row - 1] == SPACE && animframe == 2) {
                    current_level[col][row - 1] = VIRUS;
                    sound(SLOOP);
                    Virusmoved = YES;
                } else if (*((*(current_level + col)) + (row + 1)) == SPACE && animframe == 3) {
                    *((*(current_level + col)) + (row + 1)) = VIRUS;
                    sound(SLOOP);
                    Virusmoved = YES;
                }

            }


                /* If a MONSTER !!! oooohhhh */
            else if (*((*(current_level + col)) + row) == BEAST && Monstermoved == NO) {


                if (col < prongsx && *((*(current_level + (col + 1))) + row) == SPACE) {
                    *((*(current_level + (col + 1))) + row) = BEAST;
                    *((*(current_level + col)) + row) = SPACE;
                    Monstermoved = YES;
                } else if (*((*(current_level + (col + 1))) + row) == PRONGS) {
                    *((*(current_level + col)) + row) = SPACE;
                    *((*(current_level + (col + 1))) + row) = BEAST;
                    sound(ROAR);
                    DIE = 1;
                } else if (col > prongsx && *((*(current_level + (col - 1))) + row) == SPACE) {
                    *((*(current_level + (col - 1))) + row) = BEAST;
                    *((*(current_level + col)) + row) = SPACE;
                    Monstermoved = YES;
                } else if (*((*(current_level + (col - 1))) + row) == PRONGS) {
                    *((*(current_level + col)) + row) = SPACE;
                    *((*(current_level + (col - 1))) + row) = BEAST;
                    sound(ROAR);
                    DIE = 1;
                } else if (row < prongsy && *((*(current_level + col)) + (row + 1)) == SPACE) {
                    *((*(current_level + col)) + (row + 1)) = BEAST;
                    *((*(current_level + col)) + row) = SPACE;
                    Monstermoved = YES;
                } else if (*((*(current_level + col)) + (row + 1)) == PRONGS) {
                    *((*(current_level + col)) + row) = SPACE;
                    *((*(current_level + col)) + (row + 1)) = BEAST;
                    sound(ROAR);
                    DIE = 1;
                } else if (row > prongsy && (current_level[col][row - 1] == SPACE)) {
                    current_level[col][row - 1] = BEAST;
                    *((*(current_level + col)) + row) = SPACE;
                    Monstermoved = YES;
                } else if (current_level[col][row - 1] == PRONGS) {
                    *((*(current_level + col)) + row) = SPACE;
                    current_level[col][row - 1] = BEAST;
                    sound(ROAR);
                    DIE = 1;
                }
            }



                /* If a Cube */
            else if (*((*(current_level + col)) + row) == CUBE && Cubemoved == NO) {

                if (col < prongsx && *((*(current_level + (col + 1))) + row) == SPACE) {
                    *((*(current_level + (col + 1))) + row) = CUBE;
                    *((*(current_level + col)) + row) = SPACE;
                    sound(OW);
                    Cubemoved = YES;
                } else if (col > prongsx && *((*(current_level + (col - 1))) + row) == SPACE) {
                    *((*(current_level + (col - 1))) + row) = CUBE;
                    *((*(current_level + col)) + row) = SPACE;
                    sound(OW);
                    Cubemoved = YES;
                } else if (row < prongsy && *((*(current_level + col)) + (row + 1)) == SPACE) {
                    *((*(current_level + col)) + (row + 1)) = CUBE;
                    *((*(current_level + col)) + row) = SPACE;
                    sound(OW);
                    Cubemoved = YES;
                } else if (row > prongsy && (current_level[col][row - 1] == SPACE)) {
                    current_level[col][row - 1] = CUBE;
                    *((*(current_level + col)) + row) = SPACE;
                    sound(OW);
                    Cubemoved = YES;
                }
            }


                /* if an egg.... */
            else if (*((*(current_level + col)) + row) == EGG) {

                if (row > 47) {
                    row = 47;
                }
                /* iF UNDERNEATH IS DIAMOND OR ROCK */
                if (*((*(current_level + col)) + (row + 1)) == DIAMOND ||
                    *((*(current_level + col)) + (row + 1)) == ROCK) {
                    /* checks area to the right of the diamond or rock */
                    if (*((*(current_level + (col + 1))) + row) == SPACE &&
                        *((*(current_level + (col + 1))) + (row + 1)) == SPACE) {
                        *((*(current_level + col)) + row) = SPACE;
                        *((*(current_level + (col + 1))) + row) = EGG;
                        *((*(gravity + col)) + row) = STATIONARY;
                        gravity[col + 1][row] = FALLING;
                    }
                        /* otherwise checks to left ...*/
                    else if (*((*(current_level + (col - 1))) + row) == SPACE &&
                             *((*(current_level + (col - 1))) + (row + 1)) == SPACE) {
                        *((*(current_level + col)) + row) = SPACE;
                        *((*(current_level + (col - 1))) + row) = EGG;
                        *((*(gravity + col)) + row) = STATIONARY;
                        gravity[col - 1][row] = FALLING;
                    }
                }
                if (*((*(current_level + col)) + (row + 1)) == SPACE) {
                    *((*(current_level + col)) + row) = SPACE;
                    *((*(current_level + col)) + (row + 1)) = EGG;
                    *((*(gravity + col)) + row) = STATIONARY;
                    *((*(gravity + col)) + (row + 1)) = FALLING;
                }

                if (*((*(current_level + col)) + (row + 1)) != SPACE &&
                    *((*(gravity + col)) + row) == FALLING) {
                    *((*(gravity + col)) + row) = STATIONARY;
                    *((*(current_level + col)) + row) = BEAST;
                    sound(GETYE);
                }
            }



                /* if a bomb.... */
            else if (*((*(current_level + col)) + row) == BOMB) {

                if (row > 47) {
                    row = 47;
                }
                /* iF UNDERNEATH IS DIAMOND OR ROCK */
                if (*((*(current_level + col)) + (row + 1)) == DIAMOND ||
                    *((*(current_level + col)) + (row + 1)) == ROCK) {
                    /* checks area to the right of the diamond or rock */
                    if (*((*(current_level + (col + 1))) + row) == SPACE &&
                        *((*(current_level + (col + 1))) + (row + 1)) == SPACE) {
                        *((*(current_level + col)) + row) = SPACE;
                        *((*(current_level + (col + 1))) + row) = BOMB;
                        *((*(gravity + col)) + row) = STATIONARY;
                        gravity[col + 1][row] = FALLING;
                    }
                        /* otherwise checks to left ...*/
                    else if (*((*(current_level + (col - 1))) + row) == SPACE &&
                             *((*(current_level + (col - 1))) + (row + 1)) == SPACE) {
                        *((*(current_level + col)) + row) = SPACE;
                        *((*(current_level + (col - 1))) + row) = BOMB;
                        *((*(gravity + col)) + row) = STATIONARY;
                        gravity[col - 1][row] = FALLING;
                    }
                }
                if (*((*(current_level + col)) + (row + 1)) == SPACE) {
                    *((*(current_level + col)) + row) = SPACE;
                    *((*(current_level + col)) + (row + 1)) = BOMB;
                    *((*(gravity + col)) + row) = STATIONARY;
                    *((*(gravity + col)) + (row + 1)) = FALLING;
                }


                if (*((*(current_level + col)) + (row + 1)) != SPACE &&
                    *((*(gravity + col)) + row) == FALLING) {
                    *((*(gravity + col)) + row) = STATIONARY;
                    sound(EXPLODE);
                    for (posx = 1; posx > -2; posx--) {
                        for (posy = 1; posy > -2; posy--) {
                            if (col + posx > 0 && col + posx < 64) {
                                if (row + posy > 0 && row + posy < 48) {
                                    if (current_level[col + posx][col + posy] == DIAMOND
                                        || current_level[col + posx][row + posy] == MORPHBLOCK
                                        || current_level[col + posx][row + posx] == CHANGER) {
                                    }
                                    if (current_level[col + posx][row + posy] == CUBE) {
                                        sound(WHINE);
                                        current_level[col + posx][row + posy] = SNAKE;
                                    } else if (current_level[col + posx][row + posy] == BEAST) {
                                        sound(SPLAT);
                                        current_level[col + posx][row + posy] = SNAKE;
                                        diamondcount++;
                                    } else if (current_level[col + posx][row + posy] == PRONGS) {
                                        DIE = 1;
                                    } else {
                                        current_level[col + posx][row + posy] = SNAKE;
                                    }
                                }
                            }
                        }
                    }
                }
            }

                /* if a rock.....*/
            else if (*((*(current_level + col)) + row) == ROCK) {

                if (row > 47) {
                    row = 47;
                }

                /* if object underneath is a diamond or rock*/
                if (*((*(current_level + col)) + (row + 1)) == DIAMOND ||
                    *((*(current_level + col)) + (row + 1)) == ROCK) {
                    /* checks area to the right of the diamond or rock */
                    if (*((*(current_level + (col + 1))) + row) == SPACE &&
                        *((*(current_level + (col + 1))) + (row + 1)) == SPACE) {
                        *((*(current_level + col)) + row) = SPACE;
                        *((*(current_level + (col + 1))) + row) = ROCK;
                        *((*(gravity + col)) + row) = STATIONARY;
                        gravity[col + 1][row] = FALLING;
                        sound(BOOM);
                    }
                        /* otherwise checks to left ...*/
                    else if (*((*(current_level + (col - 1))) + row) == SPACE &&
                             *((*(current_level + (col - 1))) + (row + 1)) == SPACE) {
                        *((*(current_level + col)) + row) = SPACE;
                        *((*(current_level + (col - 1))) + row) = ROCK;
                        *((*(gravity + col)) + row) = STATIONARY;
                        gravity[col - 1][row] = FALLING;
                        sound(BOOM);
                    }
                }

                /* If object underneath is a SPACE */
                if (*((*(current_level + col)) + (row + 1)) == SPACE) {
                    if (current_level[col][row + 2] != SPACE) {
                        sound(BOOM);
                    }
                    *((*(current_level + col)) + row) = SPACE;
                    *((*(current_level + col)) + (row + 1)) = ROCK;
                    *((*(gravity + col)) + row) = STATIONARY;
                    *((*(gravity + col)) + (row + 1)) = FALLING;
                }

                /* If object underneath is beastie */
                if (*((*(current_level + col)) + (row + 1)) == BEAST) {
                    *((*(current_level + col)) + row) = SPACE;
                    *((*(current_level + col)) + (row + 1)) = ROCK;
                    sound(SPLAT);
                    score += 50;
                    upto500 += 50;
                }

                /* If object underneath is mannie */
                if (*((*(current_level + col)) + (row + 1)) == PRONGS) {
                    if (*((*(gravity + col)) + row) == FALLING) {
                        DIE = 1;
                        sound(EXPLODE);
                        *((*(current_level + col)) + row) = SPACE;
                    }
                }
            }

                /* if a SNAKE.....*/
            else if (*((*(current_level + col)) + row) == SNAKE) {

                if (row > 47) {
                    row = 47;
                }

                /* if object underneath is a diamond or rock*/
                if (*((*(current_level + col)) + (row + 1)) == DIAMOND ||
                    *((*(current_level + col)) + (row + 1)) == ROCK ||
                    *((*(current_level + col)) + (row + 1)) == SNAKE) {

                    /* checks area to the right of the diamond or rock */
                    if (*((*(current_level + (col + 1))) + row) == SPACE &&
                        *((*(current_level + (col + 1))) + (row + 1)) == SPACE) {
                        *((*(current_level + col)) + row) = SPACE;
                        *((*(current_level + (col + 1))) + row) = SNAKE;
                        *((*(gravity + col)) + row) = STATIONARY;
                        gravity[col + 1][row] = FALLING;
                        sound(WHEEP);
                    }
                        /* otherwise checks to left ...*/
                    else if (*((*(current_level + (col - 1))) + row) == SPACE &&
                             *((*(current_level + (col - 1))) + (row + 1)) == SPACE) {
                        *((*(current_level + col)) + row) = SPACE;
                        *((*(current_level + (col - 1))) + row) = SNAKE;
                        *((*(gravity + col)) + row) = STATIONARY;
                        gravity[col - 1][row] = FALLING;
                        sound(WHEEP);
                    }
                }

                /* If object underneath is a SPACE */
                if (*((*(current_level + col)) + (row + 1)) == SPACE) {
                    if (current_level[col][row + 2] != SPACE) {
                        sound(WHEEP);
                    }
                    *((*(current_level + col)) + row) = SPACE;
                    *((*(current_level + col)) + (row + 1)) = SNAKE;
                    *((*(gravity + col)) + row) = STATIONARY;
                    *((*(gravity + col)) + (row + 1)) = FALLING;
                }

                /* If object underneath is beastie */
                if (*((*(current_level + col)) + (row + 1)) == BEAST) {
                    *((*(current_level + col)) + row) = SPACE;
                    *((*(current_level + col)) + (row + 1)) = SNAKE;
                    sound(SPLAT);
                    score += 50;
                    upto500 += 50;
                }

                /* If object underneath is mannie */
                if (*((*(current_level + col)) + (row + 1)) == PRONGS) {
                    if (*((*(gravity + col)) + row) == FALLING) {
                        DIE = 1;
                        sound(EXPLODE);
                        *((*(current_level + col)) + row) = SPACE;
                    }
                }
            }




                /* if a diamond.....*/
            else if (FALLING_DIAMONDS == ON) {

                if (*((*(current_level + col)) + row) == DIAMOND) {


                    if (*((*(current_level + col)) + (row + 1)) == DIAMOND ||
                        *((*(current_level + col)) + (row + 1)) == ROCK) {


                        if (*((*(current_level + (col + 1))) + row) == SPACE &&
                            *((*(current_level + (col + 1))) + (row + 1)) == SPACE) {
                            *((*(current_level + col)) + row) = SPACE;
                            *((*(current_level + (col + 1))) + row) = DIAMOND;
                            *((*(gravity + col)) + row) = STATIONARY;
                            gravity[col + 1][row] = FALLING;
                        } else if (*((*(current_level + (col - 1))) + row) == SPACE &&
                                   *((*(current_level + (col - 1))) + (row + 1)) == SPACE) {
                            *((*(current_level + col)) + row) = SPACE;
                            *((*(current_level + (col - 1))) + row) = DIAMOND;
                            *((*(gravity + col)) + row) = STATIONARY;
                            gravity[col - 1][row] = FALLING;
                            sound(WHEEP);
                        }
                    }


                    if (*((*(current_level + col)) + (row + 1)) == SPACE) {
                        if (current_level[col][row + 2] != SPACE) {
                            sound(WHEEP);
                        }
                        *((*(current_level + col)) + row) = SPACE;
                        *((*(current_level + col)) + (row + 1)) = DIAMOND;
                        *((*(gravity + col)) + row) = STATIONARY;
                        *((*(gravity + col)) + (row + 1)) = FALLING;
                    }


                    if (*((*(current_level + col)) + (row + 1)) == PRONGS) {
                        if (*((*(gravity + col)) + row) == FALLING) {
                            DIE = 1;
                            *((*(current_level + col)) + row) = SPACE;
                        }
                    }
                }
            } else *((*(current_level + col)) + row) = *((*(current_level + col)) + row);
        }
    }
}


/*
 * Passed position in array, and draws screen with this position at
 * centre. Screen is 6*6. The bitmaps are not compiled, as the prog runs way
 * too fast as it is!
 */
void draw_screen(int x, int y) {

    int col, row, xpos, ypos;
    xpos = 5;
    ypos = 5;

    for (col = x - 8; col < x + 8; col++) {
        for (row = y - 7; row < y + 7; row++) {
            if (col > -1 && row > -1 && col < 64 && row < 48) {
                if (*((*(current_level + col)) + row) == PRONGS) {
                    gl_putbox(xpos, ypos, 32, 32, *(graphics + PRONGS + manimframe));
                } else {
                    gl_putbox(xpos, ypos, 32, 32, graphics[(*((*(current_level + col)) + row)) + animframe]);
                }
            } else { gl_putbox(xpos, ypos, 32, 32, *(graphics + OUTSIDE)); }
            ypos += 32;
        }
        xpos += 32;
        ypos = 5;
    }
}

void display_map(void) {
    int col, row;
    for (col = 0; col < 64; col++) {
        for (row = 0; row < 48; row++) {
            gl_putbox((col * 10), (row * 10), 10, 10, map_graphics[*((*(current_level + col)) + row)]);
        }
    }
}


/*
 *
 * Draw status
 *
 */
void draw_status(void) {

    int i, j, col, diamonds_printed;
    col = 0;
    diamonds_printed = 0;
    for (i = 0; i < LIVES; i++) {
        gl_putboxmask(534 + col, 32, 32, 32, graphics[PRONGSDOWN]);
        col += 32;
    }
    /* Draw smiley's */
    gl_putbox(534, 370, 32, 32, graphics[SMILEY + score_block[0]]);
    gl_putbox(598, 370, 32, 32, graphics[SMILEY + score_block[1]]);
    gl_putbox(534, 434, 32, 32, graphics[SMILEY + score_block[2]]);
    gl_putbox(598, 434, 32, 32, graphics[SMILEY + score_block[3]]);
    if (HASMAP == YES) {
        gl_putboxmask(566, 80, 32, 32, graphics[MAP + animframe]);
    }
    if (diamondcount > 200) {
        for (i = 0; i < 200; i += 10) {
            for (j = 0; j < 100; j += 10) {
                gl_putboxmask(530 + j, 150 + i, 10, 10, map_graphics[DIAMOND]);
            }
        }
    }
    if (diamondcount < 201) {
        for (i = -10; i < 200; i += 10) {
            for (j = -10; j < 100; j += 10) {
                if (diamonds_printed < diamondcount) {
                    gl_putboxmask(530 + j, 150 + i, 10, 10, map_graphics[DIAMOND]);
                    diamonds_printed++;
                } else {
                    gl_putboxmask(530 + j, 150 + i, 10, 10, map_graphics[SPACE]);
                }
            }
        }
    }
}


int main_edit_loop(void) {
    FILE *infile;
    int i, j, xp, yp, row, col, xpos, ypos, lev, im, cur_level;
    GraphicsContext realscreen;
    GraphicsContext virtualscreen;
    int current_block;
    cur_level = 0;
    xp = yp = 0;
    xpos = ypos = 0;
    keyboard_init();
    mouse_reset();
    gl_setcontextvga(10);
    gl_getcontext(&realscreen);
    gl_setcontextvgavirtual(10);
    gl_getcontext(&virtualscreen);
    current_block = 0;

    while (1) {
        uint32_t frameStart = SDL_GetTicks();
        keyboard_update();

        if (keyboard_keypressed(SCANCODE_Q)) {
            yp--;
            if (yp < 0) {
                yp = 0;
            }
        }
        if (keyboard_keypressed(SCANCODE_A)) {
            yp++;
            if (yp > 47) {
                yp = 47;
            }
        }
        if (keyboard_keypressed(SCANCODE_O)) {
            xp--;
            if (xp < 0) {
                xp = 0;
            }
        }
        if (keyboard_keypressed(SCANCODE_P)) {
            xp++;
            if (xp > 63) {
                xp = 63;
            }
        }
        if (keyboard_keypressed(SCANCODE_CURSORBLOCKUP)) {
            current_block += 4;
            if (current_block > 255) {
                current_block = 0;
            }
        }
        if (keyboard_keypressed(SCANCODE_CURSORBLOCKDOWN)) {
            current_block -= 4;
            if (current_block < 0) {
                current_block = 0;
            }
        }

        if (keyboard_keypressed(SCANCODE_CURSORUP)) {
            cur_level++;
            if (cur_level > 19) {
                cur_level = 19;
            }
        }
        if (keyboard_keypressed(SCANCODE_CURSORDOWN)) {
            cur_level--;
            if (cur_level < 0) {
                cur_level = 0;
            }
        }

        if (keyboard_keypressed(SCANCODE_SPACE)) {
            levels[cur_level][xp][yp] = current_block;
        }
        if (keyboard_keypressed(SCANCODE_F1)) {
            for (i = 0; i < 64; i++) {
                for (j = 0; j < 48; j++) {
                    levels[cur_level][i][j] = SPACE;
                }
            }
        }


        if (keyboard_keypressed(SCANCODE_ESCAPE)) {
            infile = fopen("levelshi", "wb");
            for (lev = 0; lev < 20; lev++) {
                for (xp = 0; xp < 64; xp++) {
                    for (yp = 0; yp < 48; yp++) {
                        if (levels[lev][xp][yp] == 64) {
                            levels[lev][xp][yp] = 4;
                        }
                        fwrite(&(levels[lev][xp][yp]), 1, 1, infile);
                    }
                }
            }
            fclose(infile);
            keyboard_close();
            return EXIT_SUCCESS;
        }

        for (col = 0; col < 64; col++) {
            for (row = 0; row < 48; row++) {
                gl_putbox(col * 10, row * 10, 10, 10, map_graphics[levels[cur_level][col][row]]);
            }
        }
        gl_putboxmask(xp * 10, yp * 10, 10, 10, map_graphics[current_block]);
        gl_copyscreen();

        uint32_t frameTime = SDL_GetTicks() - frameStart;

        // This keeps us from displaying more frames than intended FPS
        if (frameDelayEditor > frameTime) {
            SDL_Delay(frameDelayEditor - frameTime);
        }
    }
    keyboard_close();
    return EXIT_SUCCESS;
}


/*
 *
 *
 *               ***************************************
 *                 MAIN LOOP
 *               ***************************************
 *
 *
 *
 */
int main_loop(void) {

    /*
    *
    *   ******************************************
    *    PRE LOOP STUFF
    *   ******************************************
    *
    *
    */

    int i, smiley_posn;
    start_t coords;
    GraphicsContext realscreen, virtualscreen;
    PRONGS = PRONGSDOWN;
    copy_level(current);
    reset_gravity();
    coords = find_start(current);
    diamondcount = coords.no_of_diamonds + coords.no_of_morphers;
    prongsx = coords.x;
    prongsy = coords.y;
    LIVES = 3;
    keyboard_init();
    for (i = 0; i < 4; i++) {
        score_block[i] = 0;
    }
    upto500 = 0;
    smiley_posn = 0;
    CHANGE = OFF;
    HASMAP = NO;
    gl_setcontextvga(10);
    gl_getcontext(&realscreen);
    gl_setcontextvgavirtual(10);
    gl_getcontext(&virtualscreen);

    gl_clearscreen(4);
    draw_status();
    draw_screen(prongsx, prongsy);
    gl_copyscreen();
    sound(END);

    /*
    *
    *     ******************************************
    *            LOOP
    *     ******************************************
    *
    *
    */

    while (1) {
        uint32_t frameStart = SDL_GetTicks();
        keyboard_update();
        update_level();
        /*
       *
       *        *************************************
       *           LEFT KEY PRESSED
       *        *************************************
       *
       */

        if (keyboard_keypressed(SCANCODE_O) ||
            keyboard_keypressed(SCANCODE_CURSORBLOCKLEFT)) {
            PRONGS = PRONGSLEFT;
            manimframe++;
            if (manimframe > 3) {
                manimframe = 0;
            }
            if (*((*(current_level + (prongsx - 1))) + prongsy) == BRICK) {
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == VIRUS) {
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == CUBE) {
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == MORPHBLOCK) {
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == MAP) {
                sound(MAPSOUND);
                HASMAP = YES;
                *((*(current_level + (prongsx - 1))) + prongsy) = SPACE;
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == ROCK) {
                if (*((*(current_level + (prongsx - 2))) + prongsy) == SPACE) {
                    *((*(current_level + (prongsx - 2))) + prongsy) = ROCK;
                    *((*(current_level + (prongsx - 1))) + prongsy) = SPACE;
                    *((*(current_level + prongsx)) + prongsy) = SPACE;
                    prongsx--;
                    sound(GRIND);
                    if (prongsx < 0) {
                        prongsx = 0;
                    }
                }
                if (*((*(current_level + (prongsx - 2))) + prongsy) == BEAST) {
                    *((*(current_level + (prongsx - 2))) + prongsy) = ROCK;
                    *((*(current_level + (prongsx - 1))) + prongsy) = SPACE;
                    *((*(current_level + prongsx)) + prongsy) = SPACE;
                    prongsx--;
                    if (prongsx < 0) {
                        prongsx = 0;
                    }
                    sound(SPLAT);
                    score += 50;
                    upto500 += 50;
                }
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == EGG) {
                if (*((*(current_level + (prongsx - 2))) + prongsy) == SPACE) {
                    *((*(current_level + (prongsx - 2))) + prongsy) = EGG;
                    *((*(current_level + (prongsx - 1))) + prongsy) = SPACE;
                    *((*(current_level + prongsx)) + prongsy) = SPACE;
                    prongsx--;
                    sound(GRIND);
                    if (prongsx < 0) {
                        prongsx = 0;
                    }
                }
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == BOMB) {
                if (*((*(current_level + (prongsx - 2))) + prongsy) == SPACE) {
                    *((*(current_level + (prongsx - 2))) + prongsy) = BOMB;
                    *((*(current_level + (prongsx - 1))) + prongsy) = SPACE;
                    *((*(current_level + prongsx)) + prongsy) = SPACE;
                    prongsx--;
                    sound(GRIND);
                    if (prongsx < 0) {
                        prongsx = 0;
                    }
                }
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == CHANGER) {
                *((*(current_level + (prongsx - 1))) + prongsy) = SPACE;
                sound(CLICK);
                CHANGE = ON;
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == SAND) {
                score++;
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                sound(SLURP);
                prongsx--;
                if (prongsx < 0) {
                    prongsx = 0;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == SNAKE) {
                score += 50;
                upto500 += 50;
                sound(WHINE);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsx--;
                if (prongsx < 0) {
                    prongsx = 0;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else if (*((*(current_level + (prongsx - 1))) + prongsy) == DIAMOND) {
                score += 10;
                upto500 += 10;
                diamondcount--;
                sound(WHEEP);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsx--;
                if (prongsx < 0) {
                    prongsx = 0;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else {
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsx--;
                if (prongsx < 0) {
                    prongsx = 0;
                }
            }
            *((*(current_level + prongsx)) + prongsy) = PRONGS;
        }

        /*
       *
       *
       *
       *              ********************************
       *                RIGHT KEY PRESSED
       *              ********************************
       *
       *
       *
       */
        if (keyboard_keypressed(SCANCODE_P) ||
            keyboard_keypressed(SCANCODE_CURSORBLOCKRIGHT)) {
            PRONGS = PRONGSRIGHT;
            manimframe++;
            if (manimframe > 3) {
                manimframe = 0;
            }
            if (current_level[prongsx + 1][prongsy] == BRICK) {
            } else if (current_level[prongsx + 1][prongsy] == VIRUS) {
            } else if (current_level[prongsx + 1][prongsy] == MORPHBLOCK) {
            } else if (current_level[prongsx + 1][prongsy] == CUBE) {
            } else if (current_level[prongsx + 1][prongsy] == BEAST) {
                sound(ROAR);
                DIE = 1;
            } else if (current_level[prongsx + 1][prongsy] == MAP) {
                sound(MAPSOUND);
                HASMAP = YES;
                current_level[prongsx + 1][prongsy] = SPACE;
            } else if (current_level[prongsx + 1][prongsy] == ROCK) {
                if (current_level[prongsx + 2][prongsy] == SPACE) {
                    current_level[prongsx + 2][prongsy] = ROCK;
                    current_level[prongsx + 1][prongsy] = SPACE;
                    *((*(current_level + prongsx)) + prongsy) = SPACE;
                    sound(GRIND);
                    prongsx++;
                    if (prongsx > 64) {
                        prongsx = 64;
                    }
                }
                if (current_level[prongsx + 2][prongsy] == BEAST) {
                    current_level[prongsx + 2][prongsy] = ROCK;
                    current_level[prongsx + 1][prongsy] = SPACE;
                    *((*(current_level + prongsx)) + prongsy) = SPACE;
                    sound(SPLAT);
                    score += 50;
                    upto500 += 50;
                    prongsx++;
                    if (prongsx > 64) {
                        prongsx = 64;
                    }
                }
            } else if (current_level[prongsx + 1][prongsy] == CHANGER) {
                current_level[prongsx + 1][prongsy] = SPACE;
                sound(CLICK);
                CHANGE = ON;
            } else if (current_level[prongsx + 1][prongsy] == EGG) {
                if (current_level[prongsx + 2][prongsy] == SPACE) {
                    current_level[prongsx + 2][prongsy] = EGG;
                    current_level[prongsx + 1][prongsy] = SPACE;
                    *((*(current_level + prongsx)) + prongsy) = SPACE;
                    prongsx++;
                    sound(GRIND);
                    if (prongsx > 64) {
                        prongsx = 64;
                    }
                }
            } else if (current_level[prongsx + 1][prongsy] == BOMB) {
                if (current_level[prongsx + 2][prongsy] == SPACE) {
                    current_level[prongsx + 2][prongsy] = BOMB;
                    current_level[prongsx + 1][prongsy] = SPACE;
                    *((*(current_level + prongsx)) + prongsy) = SPACE;
                    prongsx++;
                    sound(GRIND);
                    if (prongsx > 64) {
                        prongsx = 64;
                    }
                }
            } else if (current_level[prongsx + 1][prongsy] == SAND) {
                score++;
                sound(SLURP);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsx++;
                if (prongsx > 64) {
                    prongsx = 64;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else if (current_level[prongsx + 1][prongsy] == SNAKE) {
                score += 50;
                upto500 += 50;
                sound(WHINE);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsx++;
                if (prongsx > 64) {
                    prongsx = 64;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else if (current_level[prongsx + 1][prongsy] == DIAMOND) {
                score += 10;
                upto500 += 10;
                diamondcount--;
                sound(WHEEP);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsx++;
                if (prongsx > 64) {
                    prongsx = 64;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else {
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsx++;
                if (prongsx > 64) {
                    prongsx = 64;
                }
            }
            *((*(current_level + prongsx)) + prongsy) = PRONGS;
        }

        /*
       *
       *
       *
       *           *****************************
       *                UP KEY PRESSED
       *           *****************************
       *
       *
       *
       */
        if (keyboard_keypressed(SCANCODE_Q) ||
            keyboard_keypressed(SCANCODE_CURSORBLOCKUP)) {
            PRONGS = PRONGSUP;
            manimframe++;
            if (manimframe > 3) {
                manimframe = 0;
            }
            if (current_level[prongsx][prongsy - 1] == BRICK ||
                current_level[prongsx][prongsy - 1] == ROCK) {
            } else if (current_level[prongsx][prongsy - 1] == VIRUS) {
            } else if (current_level[prongsx][prongsy - 1] == EGG) {
            } else if (current_level[prongsx][prongsy - 1] == BOMB) {
            } else if (current_level[prongsx][prongsy - 1] == CUBE) {
            } else if (current_level[prongsx][prongsy - 1] == MORPHBLOCK) {
            } else if (current_level[prongsx][prongsy - 1] == BEAST) {
                sound(ROAR);
                DIE = 1;
            } else if (current_level[prongsx][prongsy - 1] == MAP) {
                sound(MAPSOUND);
                HASMAP = YES;
                current_level[prongsx][prongsy - 1] = SPACE;
            } else if (current_level[prongsx][prongsy - 1] == CHANGER) {
                current_level[prongsx][prongsy - 1] = SPACE;
                sound(CLICK);
                CHANGE = ON;
            } else if (current_level[prongsx][prongsy - 1] == SAND) {
                score++;
                sound(SLURP);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsy--;
                if (prongsy < 0) {
                    prongsy = 0;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else if (current_level[prongsx][prongsy - 1] == SNAKE) {
                score += 50;
                upto500 += 50;
                sound(WHINE);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsy--;
                if (prongsx < 0) {
                    prongsx = 0;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else if (current_level[prongsx][prongsy - 1] == DIAMOND) {
                score += 10;
                upto500 += 10;
                diamondcount--;
                sound(WHEEP);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsy--;
                if (prongsy < 0) {
                    prongsy = 0;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else {
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsy--;
                if (prongsy < 0) {
                    prongsy = 0;
                }
            }
            *((*(current_level + prongsx)) + prongsy) = PRONGS;
        }


        /*
       *
       *
       *               ***************************
       *                 DOWN KEY PRESSED
       *               ***************************
       *
       *
       *
       */
        if (keyboard_keypressed(SCANCODE_A) ||
            keyboard_keypressed(SCANCODE_CURSORBLOCKDOWN)) {
            PRONGS = PRONGSDOWN;
            manimframe++;
            if (manimframe > 3) {
                manimframe = 0;
            }
            if (current_level[prongsx][prongsy + 1] == BRICK ||
                current_level[prongsx][prongsy + 1] == ROCK) {
            } else if (current_level[prongsx][prongsy + 1] == VIRUS) {
            } else if (current_level[prongsx][prongsy + 1] == MORPHBLOCK) {
            } else if (current_level[prongsx][prongsy + 1] == CUBE) {
            } else if (current_level[prongsx][prongsy + 1] == EGG) {
            } else if (current_level[prongsx][prongsy + 1] == BOMB) {
            } else if (current_level[prongsx][prongsy + 1] == BEAST) {
                sound(ROAR);
                DIE = 1;
            } else if (current_level[prongsx][prongsy + 1] == MAP) {
                sound(MAPSOUND);
                HASMAP = YES;
                current_level[prongsx][prongsy + 1] = SPACE;
            } else if (current_level[prongsx][prongsy + 1] == CHANGER) {
                current_level[prongsx][prongsy + 1] = SPACE;
                sound(CLICK);
                CHANGE = ON;
            } else if (current_level[prongsx][prongsy + 1] == SAND) {
                score++;
                sound(SLURP);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsy++;
                if (prongsy > 47) {
                    prongsy = 47;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else if (current_level[prongsx][prongsy + 1] == SNAKE) {
                score += 50;
                upto500 += 50;
                sound(WHINE);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsy++;
                if (prongsx > 47) {
                    prongsx = 47;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else if (current_level[prongsx][prongsy + 1] == DIAMOND) {
                score += 10;
                upto500 += 10;
                diamondcount--;
                sound(WHEEP);
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsy++;
                if (prongsy > 47) {
                    prongsy = 47;
                }
                gravity[prongsx][prongsy - 1] = STATIONARY;
            } else {   /* Rubout prongs */
                *((*(current_level + prongsx)) + prongsy) = SPACE;
                prongsy++;
                if (prongsy > 47) {
                    prongsy = 47;
                }
            }
            *((*(current_level + prongsx)) + prongsy) = PRONGS;
        }

        /*
       *
       *
       *         ****************************
       *           MAP BUTTON
       *         ****************************
       *
       *
       */
        if (keyboard_keypressed(SCANCODE_SPACE) && HASMAP == YES) {

            sound(MAPSOUND);
            display_map();
            gl_copyscreen();
            while (keyboard_keypressed(SCANCODE_SPACE)) {
                keyboard_update();
            }
            gl_clearscreen(4);
            gl_copyscreen();
        }
        /* Restart button */
        if (keyboard_keypressed(SCANCODE_X)) {
            DIE = 1;
            copy_level(current);
            reset_gravity();
            CHANGE = OFF;
            HASMAP = NO;
            diamondcount = coords.no_of_diamonds + coords.no_of_morphers;
        }
        /* Suicide button */
        if (keyboard_keypressed(SCANCODE_ESCAPE)) {
            keyboard_clearstate();
            DIE = 1;
        }



        /*
       *
       *
       *
       *    ***********************************
       *     POST LOOP STUFF
       *    ***********************************
       *
       *
       *
       */


        draw_status();
        draw_screen(prongsx, prongsy);
        gl_copyscreen();

        /* The 'what to do if you die' routine */
        if (DIE == 1) {
            DIE = 0;

            mouse_reset();
            sound(SCREAM);
            *((*(current_level + prongsx)) + prongsy) = SPACE;
            /* Screen pulse thingy */
            gl_setcontext(&realscreen);
            for (i = 17; i < 30; i++) {
                gl_clearscreen(i);
            }
            for (i = 30; i > 16; i--) {
                gl_clearscreen(i);
            }
            gl_setcontext(&virtualscreen);
            LIVES--;
            if (LIVES == 0) {
                if (SOUND == ON) {
                    sound(TOC);
                    sleep(5);
                }
                keyboard_close();
                return EXIT_FAILURE;
            }
            prongsx = coords.x;
            prongsy = coords.y;
            current_level[prongsx][prongsy] = PRONGS;
            gl_clearscreen(4);
            gl_copyscreen();
        }


        /* The 'what you do if you win' routine */
        if (diamondcount == 0) {
            gl_clearscreen(0);
            for (i = 0; i < 17; i++) {
                sound(i);
            }
            sleep(3);
            keyboard_close();
            return EXIT_SUCCESS;
        }

        /* The 'update the smiley's bit! */
        if (upto500 > 500) {
            score_block[smiley_posn]++;
            if (score_block[smiley_posn] == 15) {
                smiley_posn++;
                if (smiley_posn == 4) {
                    smiley_posn = 0;
                }
            }
            upto500 = 0;
        }

        /* The 'chunter along as usual' routine */
        animframe++;
        if (animframe > 3) {
            animframe = 0;
        }
        Monstermoved = NO;
        Cubemoved = NO;
        Virusmoved = NO;
        uint32_t frameTime = SDL_GetTicks() - frameStart;

        // This keeps us from displaying more frames than intended FPS
        if (frameDelayGame > frameTime) {
            SDL_Delay(frameDelayGame - frameTime);
        }
    }
    gl_freecontext(&realscreen);
    gl_freecontext(&virtualscreen);
}

/*
 *
 *
 *
 *           ************************************************
 *            PLAY THE GAME LOOP
 *           ************************************************
 *
 *
 *
 */
int play_the_game(void) {
    int ALIVE = YES;

    while (ALIVE == YES) {
        if (main_loop() == EXIT_FAILURE) {
            gl_clearscreen(0);
            ALIVE = NO;
        } else {
            current++;
            if (current > NO_OF_LEVELS) {
                ALIVE = NO;
            }
        }
    }
    return EXIT_SUCCESS;
}

/* Title Page bit */
int title_page(void) {
    GraphicsContext realscreen, static_picture;
    int mt, mX, mY, PLAY, row, col;
    int beastx, beasty, i, j, x, y;
    beastx = 100;
    beasty = 100;
    gl_setcontextvga(10);
    gl_clearscreen(0);
    gl_getcontext(&realscreen);
    gl_setcontextvgavirtual(10);
    gl_clearscreen(0);
    /* Draw Static bits and store in static_picture */

    /* Tile background */
    for (col = 0; col < 640; col += 32) {
        for (row = 0; row < 480; row += 32) {
            gl_putbox(col, row, 32, 32, graphics[OUTSIDE]);
        }
    }
    /* Print 'LIZARDS' */
    gl_putboxmask(224, 32, 32, 32, f_graphics[f_L]);
    gl_putboxmask(256, 32, 32, 32, f_graphics[f_I]);
    gl_putboxmask(288, 32, 32, 32, f_graphics[f_Z]);
    gl_putboxmask(320, 32, 32, 32, f_graphics[f_A]);
    gl_putboxmask(352, 32, 32, 32, f_graphics[f_R]);
    gl_putboxmask(384, 32, 32, 32, f_graphics[f_D]);
    gl_putboxmask(416, 32, 32, 32, f_graphics[f_S]);

    /* Print controls */
    gl_putboxmask(320, 96, 32, 32, f_graphics[f_Q]);
    gl_putboxmask(320, 128, 32, 32, f_graphics[f_UP]);
    gl_putboxmask(256, 160, 32, 32, f_graphics[f_O]);
    gl_putboxmask(288, 160, 32, 32, f_graphics[f_LEFT]);
    gl_putboxmask(352, 160, 32, 32, f_graphics[f_RIGHT]);
    gl_putboxmask(384, 160, 32, 32, f_graphics[f_P]);
    gl_putboxmask(320, 192, 32, 32, f_graphics[f_DOWN]);
    gl_putboxmask(320, 224, 32, 32, f_graphics[f_A]);

    /* Level Incrementor and decrementor */
    gl_putboxmask(272, 400, 32, 32, f_graphics[f_PLUS]);
    gl_putboxmask(368, 400, 32, 32, f_graphics[f_MINUS]);
    gl_putboxmask(320, 400, 32, 32, f_graphics[f_LEVEL]);
    /* Suicide & reset keys */
    gl_putboxmask(192, 96, 32, 32, f_graphics[f_X]);
    gl_putboxmask(192, 128, 32, 32, f_graphics[f_RESET]);
    gl_putboxmask(448, 96, 32, 32, f_graphics[f_ESC]);
    gl_putboxmask(448, 128, 32, 32, f_graphics[f_ABORT]);

    /* Quitbutton */
    gl_putboxmask(32, 400, 32, 32, f_graphics[f_QUIT]);
    /* Play button */
    gl_putboxmask(576, 400, 32, 32, f_graphics[f_PLAY]);
    /* Level Editor button */
    gl_putboxmask(320, 336, 32, 32, f_graphics[f_EDIT_ICON]);
    /* Writ by bit */
    gl_putboxmask(304, 288, 32, 32, f_graphics[f_WRIT1]);
    gl_putboxmask(336, 288, 32, 32, f_graphics[f_WRIT2]);
    gl_getcontext(&static_picture);
    /* Loop around until 'PLAY' selected */
    gl_setcontextvgavirtual(10);
    mt = vga_getmousetype();
    printf("Initialising mouse......");

    if (mouse_init("/dev/mouse", mt, 150) == -1) {
        printf("Can't open mouse.....is gpm running ? \n");
        exit(1);
    }
    mouse_setdefaulteventhandler();
    mouse_setposition(0, 0);
    mouse_setxrange(0, 600);
    mouse_setyrange(0, 440);
    mouse_setscale(1);
    mX = mY = 0;
    printf("done\n");
    PLAY = YES;

    /* Do cool screen drawing thingy */
    if (SOUND == ON) {
        sound(END);
    }
    for (i = 0; i < 80; i++) {
        gl_copyboxfromcontext(&static_picture, 0, 0, (i << 3), 480, 640 - (i << 3), 0);
        gl_copyscreen();
    }

    while (PLAY == YES) {
        uint32_t frameStart = SDL_GetTicks();
        gl_copyboxfromcontext(&static_picture, 0, 0, 639, 479, 0, 0);
        mX = mouse_getx();
        mY = mouse_gety();

        /* This bit moves the beastie around */
        animframe++;
        if (animframe > 3) {
            animframe = 0;
        }
        if (beastx > mX) {
            beastx -= 8;
        }
        if (beastx < mX) {
            beastx += 8;
        }
        if (beasty > mY) {
            beasty -= 8;
        }
        if (beasty < mY) {
            beasty += 8;
        }
        if (beastx > 608) {
            beastx = 608;
        }
        if (beastx < 32) {
            beastx = 32;
        }
        if (beasty > 448) {
            beasty = 448;
        }
        if (beasty < 32) {
            beasty = 32;
        }

        /* And hence the beastie is printED */

        if (mouse_getbutton() == MOUSE_LEFTBUTTON) {
            if (mX > 32 && mX < 64 && mY > 400 && mY < 432) {
                /* Quit button */
                if (SOUND == ON) {
                    sound(FLUSH);
                }
                for (i = 0; i < 60; i++) {
                    gl_clearscreen(0);
                    gl_copyboxfromcontext(&static_picture, 0, 0, 639, 479 - (i << 3), 0, (i << 3));
                    gl_copyscreen();
                }
                gl_clearscreen(0);
                gl_copyscreen();
                PLAY = NO;
            }
            if (mX > 272 && mX < 304 && mY > 400 && mY < 432) {
                /* Level increment */
                current++;
                if (current > 19) {
                    current = 19;
                }
            }
            if (mX > 368 && mX < 400 && mY > 400 && mY < 432) {
                /* Level decrement */
                current--;
                if (current < 0) {
                    current = 0;
                }
            }
            if (mX > 576 && mX < 608 && mY > 400 && mY < 432) {
                /* play button */
                play_the_game();
            }

            if (mX > 240 && mX < 300 && mY > 270 & mY < 334) {
                if (SOUND == ON) {
                    Snd_restore();
                    SOUND = OFF;
                } else {
                    SOUND = ON;
                    Snd_init(MAXSAMP, samps, SAMPRATE, NO_OF_CHANNELS, "/dev/dsp");
                    sound(CLICK);
                }
            }
            if (mX > 370 && mX < 430 && mY > 270 & mY < 334) {
                if (FALLING_DIAMONDS == ON) {
                    FALLING_DIAMONDS = OFF;
                } else {
                    FALLING_DIAMONDS = ON;
                }
            }
            if (mX > 320 && mX < 356 && mY > 336 && mY < 368) {
                main_edit_loop();
            }
        }


        if (PLAY == YES) {
            if (SOUND == ON) {
                gl_putboxmask(256, 288, 32, 32, f_graphics[f_SOUND]);
            } else {
                gl_putboxmask(256, 288, 32, 32, f_graphics[f_NOSOUND]);
            }
            if (FALLING_DIAMONDS == ON) {
                gl_putboxmask(384, 288, 32, 32, f_graphics[f_FALLING]);
            } else {
                gl_putboxmask(384, 288, 32, 32, f_graphics[f_NOTFALLING]);
            }
            /* Draw Level indicators */
            for (i = 0; i < current; i++) {
                gl_line(304 + (3 * i), 400, 304 + (3 * (i + 2)), 416, i + 32);
                gl_line(304 + (3 * (i + 2)), 416, 304 + (3 * i), 432, i + 32);
            }
            gl_putboxmask(beastx, beasty, 32, 32, graphics[BEAST + animframe]);
            gl_putboxmask(mX, mY, 32, 32, graphics[SNAKE]);
            gl_copyscreen();
            mouse_update();
        }
        uint32_t frameTime = SDL_GetTicks() - frameStart;

        // This keeps us from displaying more frames than 60/Second
        if (frameDelayTitle > frameTime) {
            SDL_Delay(frameDelayTitle - frameTime);
        }
    }
    mouse_close();
    gl_freecontext(&realscreen);
    gl_freecontext(&static_picture);
    return EXIT_SUCCESS;
}


int main(void) {
    printf("Starting initialization...\n");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    printf("SDL initialized successfully.\n");

    load_graphics();
    load_textdata();
    load_map_graphics();
    load_levels();
    initialise();

    printf("Entering title page...\n");
    title_page();

    printf("Cleaning up...\n");
    if (SOUND == ON) {
        Snd_restore();
    }
    cleanup(); // Make sure this function properly cleans up SDL resources

    printf("Exiting...\n");
    return 0;
}
