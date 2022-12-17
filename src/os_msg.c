/*
 * message.c
 *
 *  Created on: 5 Dec. 2022
 *      Author: brandon
 */


#include "os_scheduler.h"
#include "os_msg.h"
#include "os_event.h"

#define CIRC_NAME msg_queue
#define CIRC_TYPE os_msg_p

#include "circbuff_template.h"

typedef msg_queue_circbuff_t  msg_queue_t;

msg_queue_t msg_queue_list[MSG_QUEUE_MAX]={0};

int msg_register(os_msg_p *buffer, uint16_t length)
{
    int counter;
    if(os_get_active_msg_queue() != NULL)
    {
        // one queue per thread
        return 0;
    }
    for(counter=0; counter<MSG_QUEUE_MAX; counter++)
    {
        if(msg_queue_list[counter].buffer == NULL)
        {
            goto init_queue;
        }
    }
    return 0;
init_queue:
    msg_queue_init( &msg_queue_list[counter], buffer, length);
    os_set_active_msg_queue(&msg_queue_list[counter]);
    return 1;
}
/**
 * Blocking function to get a message
 */
os_msg_p msg_get(void)
{
    os_msg_p msg;
    msg_queue_t *queue = os_get_active_msg_queue();
    OS_ENTER_CRITICAL();
    if(msg_queue_pending(queue) == 0)
    {
        os_thread_set_flags(os_get_active_thread(), OS_FLAG_WAIT_MSG);
        os_yield(); // Will switch context when EXIT CRITICAL section
    }
    OS_EXIT_CRITICAL();
    /*  Context should switch here */
    OS_ENTER_CRITICAL();
    msg = msg_queue_read(queue);
    OS_EXIT_CRITICAL();

    return msg;
}

/**
 * Send message to recipient thread
 */
__attribute__((critical))
void msg_send(thread_t thread, os_msg_p msg)
{
    msg_queue_write(os_get_msg_queue(thread), msg);
    os_thread_clr_flags(thread, OS_FLAG_WAIT_MSG);
}
