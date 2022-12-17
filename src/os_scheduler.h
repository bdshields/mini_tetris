#ifndef __SCHED_H
#define __SCHED_H

#include <stddef.h>
#include <stdint.h>
#include <msp430.h>

#define SCHED_THREADS_MAX   3

#define OS_FLAG_WAIT_MSG    0x0100


typedef void* thread_t;
typedef void (*thread_func)(void *);

void scheduler_init(void);
thread_t sched_thread_start(thread_func function, void *context, uint16_t *stack, uint16_t stack_size);

#define os_yield()  do{IFG1 |= WDTIFG;}while(0)

void * os_get_active_msg_queue(void);
void * os_get_msg_queue(thread_t thread);
void os_set_active_msg_queue(void* queue);

void * os_get_active_thread(void);

void os_thread_set_flags(thread_t thread, uint16_t flags);
void os_thread_clr_flags(thread_t thread, uint16_t flags);

#define OS_ENTER_CRITICAL() \
{ __istate_t interrupt_state = _get_interrupt_state();  _disable_interrupts();

#define OS_EXIT_CRITICAL()  \
_set_interrupt_state(interrupt_state);  }

#endif
