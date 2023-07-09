/*
 * game_tetris.c
 *
 *  Created on: 28 May 2023
 *      Author: brandon
 */

#include "active_objects.h"

#include "timer.h"
#include "buttons.h"
#include "analog_stick.h"

#include "display.h"

#include "raster.h"
#include "sprite.h"
#include "pos.h"

// Sprites for Tetris pieces (rotate clockwise)
const sprite_t tetris_s_1_1= {
            .width = 2,
            .height = 3,
            .pixels={
                    0b10,
                    0b10,
                    0b11,
            }
        };

const sprite_t tetris_s_1_2={
            .width = 3,
            .height = 2,
            .pixels={
                    0b111,
                    0b100,
            }
        };
const sprite_t tetris_s_1_3={
            .width = 2,
            .height = 3,
            .pixels={
                    0b11,
                    0b01,
                    0b01,
            }
        };
const sprite_t tetris_s_1_4={
            .width = 3,
            .height = 2,
            .pixels={
                    0b001,
                    0b111,
            }
        };

        /* Left L */
const sprite_t tetris_s_2_1= {
            .width = 2,
            .height = 3,
            .pixels={
                    0b01,
                    0b01,
                    0b11,
            }
        };
const sprite_t tetris_s_2_2={
            .width = 3,
            .height = 2,
            .pixels={
                    0b100,
                    0b111,
            }
        };
const sprite_t tetris_s_2_3={
            .width = 2,
            .height = 3,
            .pixels={
                    0b11,
                    0b10,
                    0b10,
            }
        };
const sprite_t tetris_s_2_4={
            .width = 3,
            .height = 2,
            .pixels={
                    0b111,
                    0b001,
            }
        };
    /* square */
const sprite_t tetris_s_3_1={
            .width = 2,
            .height = 2,
            .pixels={
                    0b11,
                    0b11,
            },
        };
    /* straight */
const sprite_t tetris_s_4_1={
            .width = 1,
            .height = 4,
            .pixels={
                    0b1,
                    0b1,
                    0b1,
                    0b1,
            }
        };
const sprite_t tetris_s_4_2={
            .width = 4,
            .height = 1,
            .pixels={
                    0b1111,
            }
        };
    /* Tee */
const sprite_t tetris_s_5_1={
            .width = 3,
            .height = 2,
            .pixels={
                    0b111,
                    0b010,
            }
        };
const sprite_t tetris_s_5_2={
            .width = 2,
            .height = 3,
            .pixels={
                    0b01,
                    0b11,
                    0b01,
            }
        };
const sprite_t tetris_s_5_3={
            .width = 3,
            .height = 2,
            .pixels={
                    0b010,
                    0b111,
            }
        };
const sprite_t tetris_s_5_4={
            .width = 2,
            .height = 3,
            .pixels={
                    0b10,
                    0b11,
                    0b10,
            }
        };
    /* Zig */
const sprite_t tetris_s_6_1={
            .width = 3,
            .height = 2,
            .pixels={
                    0b110,
                    0b011,
            }
        };
const sprite_t tetris_s_6_2={
            .width = 2,
            .height = 3,
            .pixels={
                    0b01,
                    0b11,
                    0b10,
            }
        };
    /* Zag */
const sprite_t tetris_s_7_1={
            .width = 3,
            .height = 2,
            .pixels={
                    0b011,
                    0b110,
            }
        };
const sprite_t tetris_s_7_2={
            .width = 2,
            .height = 3,
            .pixels={
                    0b10,
                    0b11,
                    0b01,
            }
        };

sprite_t const *const tetris_sprites[7][4]={
        {&tetris_s_1_1,&tetris_s_1_2,&tetris_s_1_3,&tetris_s_1_4},
        {&tetris_s_2_1,&tetris_s_2_2,&tetris_s_2_3,&tetris_s_2_4},
        {&tetris_s_3_1,&tetris_s_3_1,&tetris_s_3_1,&tetris_s_3_1},
        {&tetris_s_4_1,&tetris_s_4_2,&tetris_s_4_1,&tetris_s_4_2},
        {&tetris_s_5_1,&tetris_s_5_2,&tetris_s_5_3,&tetris_s_5_4},
        {&tetris_s_6_1,&tetris_s_6_2,&tetris_s_6_1,&tetris_s_6_2},
        {&tetris_s_7_1,&tetris_s_7_2,&tetris_s_7_1,&tetris_s_7_2},
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
    raster_t image_fallen_blocks;
    pos_t piece_pos;
    sprite_t *cur_sprite;
    int  sprite_idx;
    int  sprite_rotation;
    int  animation;
    int  deleted_rows[4];
}tetris_ao_t;

tetris_ao_t tetris_ao;

fsm_status_t tetris_state_initial(tetris_ao_t *me, os_event_t *event);
fsm_status_t tetris_state_active(tetris_ao_t *me, os_event_t *event);
fsm_status_t tetris_state_colapsing(tetris_ao_t *me, os_event_t *event);
fsm_status_t tetris_state_lose(tetris_ao_t *me, os_event_t *event);
fsm_status_t tetris_state_pause(tetris_ao_t *me, os_event_t *event);



int tetris_colapsable(raster_t *fallen_blocks);


void tetris_init(void)
{
    ao_cons((ao_t*)&tetris_ao, tetris_state_initial);
}

fsm_status_t tetris_state_initial(tetris_ao_t *me, os_event_t *event)
{
    fsm_status_t status=EVENT_IGNORED;
    switch(event->event)
    {
    case E_STATE_ENTER:
        me->piece_pos.x = 4;
        me->piece_pos.y = -2;
        me->cur_sprite = tetris_sprites[0][0];
        me->sprite_idx=0;
        me->sprite_rotation = 0;
        raster_clear(&me->image_fallen_blocks);
        buttons_request();
        stick_request();
        status = FSM_TRANS(tetris_state_active);
        break;
    default:
        status = EVENT_IGNORED;
        break;
    }
    return status;
}

fsm_status_t tetris_state_active(tetris_ao_t *me, os_event_t *event)
{
    static sprite_t paint_stat = {
            .width = 8,
            .height = 1,
            .pixels = {0},
    };
    raster_t temp_raster;
    pos_t    temp_pos;
    int      temp_rotation;
    sprite_t *temp_cur_sprite;

    fsm_status_t status=EVENT_IGNORED;
    switch(event->event)
    {
    case E_STATE_ENTER:
        paint_stat.pixels[0] = raster_check_outside(me->cur_sprite, me->piece_pos);
        raster_clear(&temp_raster);
        raster_or_sprite(&temp_raster, &paint_stat, _POS(0,0));
        raster_or_sprite(&temp_raster, me->cur_sprite, me->piece_pos);
        raster_or_raster(&temp_raster, &me->image_fallen_blocks);
        disp_copy_image(temp_raster);
        timer_set(TIMER_ms(100));
        status = EVENT_HANDLED;
        break;
    case E_STATE_EXIT:
        status = EVENT_HANDLED;
        break;
    case E_TIMER:
        temp_pos = me->piece_pos;
        temp_pos.y ++;
        if(raster_check_overlap(&me->image_fallen_blocks, me->cur_sprite, temp_pos)
                || (raster_check_outside(me->cur_sprite, temp_pos) & 0x02))
        {
            // pieces are touching or outside

            // attach piece to the fallen blocks
            raster_or_sprite(&me->image_fallen_blocks, me->cur_sprite, me->piece_pos);

            // Is it End of Game
            if(me->piece_pos.y < 0)
            {
                status = FSM_TRANS(tetris_state_lose);
                goto exit;
            }

            // reset the game state
            me->piece_pos.y = -2;
            me->piece_pos.x = 4;
            me->sprite_rotation = 0;
            if(++me->sprite_idx >= 7)
            {
                me->sprite_idx=0;
            }
            me->cur_sprite = tetris_sprites[me->sprite_idx][me->sprite_rotation];

            if(tetris_colapsable(&me->image_fallen_blocks))
            {
                status = FSM_TRANS(tetris_state_colapsing);
                goto exit;
            }

        }
        else
        {
            me->piece_pos.y ++;
        }

        paint_stat.pixels[0] = raster_check_outside(me->cur_sprite, me->piece_pos);
        raster_clear(&temp_raster);
        raster_or_sprite(&temp_raster, &paint_stat, _POS(0,0));
        raster_or_sprite(&temp_raster, me->cur_sprite, me->piece_pos);
        raster_or_raster(&temp_raster, &me->image_fallen_blocks);
        disp_copy_image(temp_raster);
        timer_set(TIMER_ms(500));
        status = EVENT_HANDLED;
        break;
    case E_STICK_UPDATE:
        status = EVENT_HANDLED;
#if 1
        pos_t    pos = ((stick_event_t*)event)->pos;
        temp_pos = me->piece_pos;
        temp_pos.x += pos.x > 0 ? 1 : pos.x < 0 ? -1 : 0;
        temp_pos.y += pos.y > 0 ? 1 : 0;
        if(raster_check_overlap(&me->image_fallen_blocks, me->cur_sprite, temp_pos)
                || (raster_check_outside(me->cur_sprite, temp_pos) != 0))
        {
            goto exit;
        }
        me->piece_pos = temp_pos;
#else
        pos_t    pos = ((stick_event_t*)event)->pos;
        temp_pos = me->piece_pos;
        temp_pos.x = (pos.x / (STICK_MAX_X / 4)) + 2;
        temp_pos.y = (pos.y / (STICK_MAX_Y / 8)) + 6;
        paint_stat.pixels[0] = raster_check_outside(&star, temp_pos);

        me->piece_pos = temp_pos;
#endif
        raster_clear(&temp_raster);
        raster_or_sprite(&temp_raster, &paint_stat, _POS(0,0));
        raster_or_sprite(&temp_raster, me->cur_sprite, me->piece_pos);
        raster_or_raster(&temp_raster, &me->image_fallen_blocks);
        disp_copy_image(temp_raster);
        break;
    case E_BUTTON:
        temp_rotation = me->sprite_rotation;
        if(++temp_rotation >= 4)
        {
            temp_rotation = 0;
#if 0
            if(++me->sprite_idx >= 7)
            {
                me->sprite_idx=0;
            }
#endif
        }
        temp_cur_sprite = tetris_sprites[me->sprite_idx][temp_rotation];
        if(raster_check_overlap(&me->image_fallen_blocks, temp_cur_sprite, me->piece_pos)
                || (raster_check_outside(temp_cur_sprite, me->piece_pos) != 0))
        {
            // pieces are touching or outside
            break;
        }
        else
        {
            me->sprite_rotation = temp_rotation;
            me->cur_sprite = temp_cur_sprite;
        }

//        status = FSM_TRANS(tetris_state_pause);
        break;
    default:
        status = EVENT_IGNORED;
        break;
    }
exit:
    return status;
}

fsm_status_t tetris_state_colapsing(tetris_ao_t *me, os_event_t *event)
{
    raster_t temp_raster;
    fsm_status_t status=EVENT_IGNORED;

    int src_cntr;
    int dest_cntr;
    int index;

    switch(event->event)
    {
    case E_STATE_ENTER:
        me->animation=0;
        me->deleted_rows[0]=-1;
        me->deleted_rows[1]=-1;
        me->deleted_rows[2]=-1;
        me->deleted_rows[3]=-1;
        dest_cntr=0;
        for(src_cntr=0; src_cntr< DISP_MAX_Y; src_cntr++)
        {
            if(me->image_fallen_blocks[src_cntr] == 0xFF)
            {
                me->deleted_rows[dest_cntr++]=src_cntr;
            }
        }
        timer_set(TIMER_ms(50));
        status = EVENT_HANDLED;
        break;
    case E_STATE_EXIT:
        status = EVENT_HANDLED;
        break;
    case E_TIMER:
        if(me->animation++ < 8)
        {
            for(src_cntr=0; src_cntr< 4; src_cntr++)
            {
                if(me->deleted_rows[src_cntr] > -1)
                {
                    me->image_fallen_blocks[me->deleted_rows[src_cntr]] >>= 1;
                }
            }
            timer_set(TIMER_ms(100));
        }
        else
        {
            src_cntr=DISP_MAX_Y-1;
            dest_cntr = DISP_MAX_Y-1;
            while(src_cntr > -1)
            {
                me->image_fallen_blocks[dest_cntr] = me->image_fallen_blocks[src_cntr];
                for(index=0; index< 4; index++)
                {
                    if(me->deleted_rows[index] == src_cntr)
                    {
                        // delete row src_cntr
                        goto skip_row;
                    }
                }
keep_row:
                dest_cntr --;
skip_row:
                src_cntr --;
            }
            status = FSM_TRANS(tetris_state_active);
        }
        raster_clear(&temp_raster);
        raster_or_raster(&temp_raster, &me->image_fallen_blocks);
        disp_copy_image(temp_raster);
        break;
    default:
        status = EVENT_IGNORED;
        break;
    }
    return status;
}

fsm_status_t tetris_state_lose(tetris_ao_t *me, os_event_t *event)
{
    fsm_status_t status=EVENT_IGNORED;
    raster_t temp_raster;

    switch(event->event)
    {
    case E_STATE_ENTER:
        raster_clear(&temp_raster);
        raster_or_sprite(&temp_raster, &pause,  (pos_t){.x = 1, .y = 2});
        disp_copy_image(temp_raster);
        timer_set(TIMER_ms(1000));
        status = EVENT_HANDLED;
        break;
    case E_STATE_EXIT:
        // prep new game
        me->piece_pos.x = 3;
        me->piece_pos.y = -2;
        raster_clear(&me->image_fallen_blocks);
        status = EVENT_HANDLED;
        break;
    case E_TIMER:
        status = FSM_TRANS(tetris_state_active);
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
    raster_t temp_raster;

    switch(event->event)
    {
    case E_STATE_ENTER:
        raster_clear(&temp_raster);
        raster_or_sprite(&temp_raster, &pause,  (pos_t){.x = 1, .y = 2});
        disp_copy_image(temp_raster);
        status = EVENT_HANDLED;
        break;
    case E_STATE_EXIT:
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

int tetris_colapsable(raster_t *fallen_blocks)
{
    int result = 0;
    int counter;
    for(counter=0; counter< DISP_MAX_Y; counter++)
    {
        if((*fallen_blocks)[counter] == 0xFF)
        {
            result++;
        }
    }
    return result;
}

