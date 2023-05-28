/*
 * game_tetris.c
 *
 *  Created on: 28 May 2023
 *      Author: brandon
 */

#include "active_objects.h"

#include "timer.h"
#include "buttons.h"

#include "display.h"

#include "raster.h"
#include "sprite.h"

const sprite_t star={
        .width = 3,
        .height = 3,
        .pixels={
                0x02,
                0x07,
                0x02
        }
};

const sprite_t pause={
        .width = 5,
        .height = 6,
        .pixels={
                0x1E,
                0x11,
                0x11,
                0x1E,
                0x10,
                0x10
        }
};


typedef struct tetris_ao_s{
    ao_t super;
    raster_t raster;
    int pos_y;
}tetris_ao_t;

tetris_ao_t tetris_ao;

fsm_status_t tetris_state_active(tetris_ao_t *me, os_event_t *event);
fsm_status_t tetris_state_pause(tetris_ao_t *me, os_event_t *event);



void tetris_init(void)
{
    ao_cons((ao_t*)&tetris_ao, tetris_state_active);
}


fsm_status_t tetris_state_active(tetris_ao_t *me, os_event_t *event)
{
    fsm_status_t status=EVENT_IGNORED;
    switch(event->event)
    {
    case E_STATE_ENTER:
        raster_clear(&me->raster);
        raster_paste_sprite(&me->raster, &star, 2, me->pos_y);
        disp_copy_image(me->raster);
        buttons_request();
        timer_set(TIMER_ms(500));
        status = EVENT_HANDLED;
        break;
    case E_STATE_EXIT:
        buttons_cancel();
        status = EVENT_HANDLED;
        break;
    case E_TIMER:
        me->pos_y ++;
        if(me->pos_y > 16)
        {
            me->pos_y = -3;
        }
        raster_clear(&me->raster);
        raster_paste_sprite(&me->raster, &star, 2, me->pos_y);
        disp_copy_image(me->raster);
        timer_set(TIMER_ms(500));
        status = EVENT_HANDLED;
        break;
    case E_BUTTON:
        status = FSM_TRANS(tetris_state_pause);
        break;
    default:
        status = EVENT_IGNORED;
        break;
    }
    return status;
}

fsm_status_t tetris_state_pause(tetris_ao_t *me, os_event_t *event)
{
    fsm_status_t status=EVENT_IGNORED;
    switch(event->event)
    {
    case E_STATE_ENTER:
        buttons_request();
        raster_clear(&me->raster);
        raster_paste_sprite(&me->raster, &pause, 1, 2);
        disp_copy_image(me->raster);
        status = EVENT_HANDLED;
        break;
    case E_STATE_EXIT:
        buttons_cancel();
        status = EVENT_HANDLED;
        break;
    case E_BUTTON:
        status = FSM_TRANS(tetris_state_active);
        break;
    default:
        status = EVENT_IGNORED;
        break;
    }
    return status;
}
