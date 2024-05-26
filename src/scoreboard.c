/*
 * scoreboard.c
 *
 *  Created on: 4 May 2024
 *      Author: brandon
 */


#include <stdio.h>
#include "active_objects.h"
#include "epaper.h"
#include "gfx.h"
#include "timer.h"

typedef struct scoreboard_ao_s{
    ao_t super;
    uint16_t score;
}scoreboard_ao_t;


scoreboard_ao_t scoreboard_ao;

fsm_status_t scoreboard_initial(scoreboard_ao_t *me, os_event_t *event);
fsm_status_t scoreboard_waiting(scoreboard_ao_t *me, os_event_t *event);
fsm_status_t scoreboard_update(scoreboard_ao_t *me, os_event_t *event);



void scoreboard_init(void)
{
    ao_cons((ao_t*)&scoreboard_ao, scoreboard_initial);
}

void sb_disp_score(uint8_t *raster, int canvas_width, int canvas_height, int y_offset)
{
    char score[10];
    snprintf(score, 10, "% 3d", scoreboard_ao.score);
    gfx_print(raster, canvas_width, canvas_height, 10, 2-(y_offset), "Score", 6);
    gfx_print(raster, canvas_width, canvas_height, 0, 55-(y_offset), score, 4);
#if 0
    gfx_print(raster, canvas_width, canvas_height, 0, 0-(y_offset), "hello world", 1);
    gfx_print(raster, canvas_width, canvas_height, 0, 10-(y_offset), "hello world", 2);
    gfx_print(raster, canvas_width, canvas_height, 0, 26-(y_offset), "hello world", 3);
    gfx_print(raster, canvas_width, canvas_height, 0, 50-(y_offset), "hello world", 4);
    gfx_print(raster, canvas_width, canvas_height, 0, 90-(y_offset), "hello world", 5);
    gfx_print(raster, canvas_width, canvas_height, 0, 135-(y_offset), "hello world", 6);
#endif
}

void sb_disp_start(uint8_t *raster, int canvas_width, int canvas_height, int y_offset)
{
    gfx_print(raster, canvas_width, canvas_height, 10, 10-(y_offset), "Tetris", 4);
}


fsm_status_t scoreboard_initial(scoreboard_ao_t *me, os_event_t *event)
{
    fsm_status_t status=EVENT_IGNORED;
    switch(event->event)
    {
    case E_STATE_ENTER:
        ep_update_display(&sb_disp_start,0);
        me->score = 0;
        status = FSM_TRANS(scoreboard_waiting);
        break;
    default:
        status = EVENT_IGNORED;
        break;
    }
    return status;

}
fsm_status_t scoreboard_waiting(scoreboard_ao_t *me, os_event_t *event)
{
    fsm_status_t status=EVENT_IGNORED;
    switch(event->event)
    {
    case E_STATE_ENTER:
        status = EVENT_HANDLED;
        break;
    case E_SCORE_1UP:
        me->score ++;
        status = FSM_TRANS(scoreboard_update);
        break;
    default:
        status = EVENT_IGNORED;
        break;
    }
    return status;
}

fsm_status_t scoreboard_update(scoreboard_ao_t *me, os_event_t *event)
{
    fsm_status_t status=EVENT_IGNORED;
    switch(event->event)
    {
    case E_STATE_ENTER:
        timer_set(TIMER_s(5));
        ep_update_display(&sb_disp_score,1);
        status = EVENT_HANDLED;
        break;
    case E_TIMER:
        status = FSM_TRANS(scoreboard_waiting);
        status = EVENT_HANDLED;
        break;
    case E_SCORE_1UP:
        me->score ++;
        status = EVENT_HANDLED;
        break;
    default:
        status = EVENT_IGNORED;
        break;
    }
    return status;
}

void sb_adjust_score(int scoreDiff)
{
    static os_msg_t message = {.event=E_SCORE_1UP};
    msg_send(ao_getTask(&scoreboard_ao), &message);
}
