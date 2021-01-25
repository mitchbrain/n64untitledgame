#ifndef __PLAYER_H
#define __PLAYER_H

#include "system.h"
#include "graphics.h"

/* Player definitions */

/* States */
typedef enum player_state
{
    PLAYER_STATE_READY,
    PLAYER_STATE_PLAY,
    PLAYER_STATE_DYING,
    PLAYER_STATE_DEAD,
    PLAYER_STATE_TITLE
} player_state_t;

/* Colors */
typedef enum player_color
{
    PLAYER_COLOR_YELLOW,
    PLAYER_COLOR_RED,
    PLAYER_COLOR_BLUE,
    PLAYER_NUM_COLORS
} player_color_t;

typedef struct
{
    sprite_t *sprite;
    player_state_t state;
    /* Center point */
    float x;
    float y;
} player_t;


void tick(player_t *player, const gamepad_state_t *gamepad);


#endif