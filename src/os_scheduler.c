#include <msp430.h>
#include <stdint.h>

#include "os_scheduler.h"


typedef struct sched_thread_info_s{
    uint16_t  *sp;
    uint16_t   flags;
    void      *msg_queue;

    // Not absolutely required, but good for stack overflow checks
    uint16_t  *stack;
    uint16_t  stack_size;
    
}sched_thread_info_t;

#define SCHED_FLAGS_USED         0x0001        // Slot used
#define SCHED_FLAGS_FINISHED     0x0002        // thread has existed

#define SCHED_BLOCK_MASK         0xFF00

sched_thread_info_t *sched_cur_thread;
sched_thread_info_t *sched_next_thread;
sched_thread_info_t sched_threads[SCHED_THREADS_MAX] = {};

__attribute__((interrupt(COMPARATORA_VECTOR)))
void isr_switchContext(void)
{
    /**
     * Could merge the context switch into the systick handler,
     * but keeping it separate means the assembly won't change
     * as the systick handler evolves
     */


    // Save register context
    asm("push R15 \n\t"
        "push R14 \n\t"
        "push R13 \n\t"
        "push R12 \n\t"
        "push R11 \n\t"
        "push R10 \n\t"
        "push R9 \n\t"
        "push R8 \n\t"
        "push R7 \n\t"
        "push R6 \n\t"
        "push R5 \n\t"
        "push R4 \n\t");

    // R1 is the Stack Pointer register
    // current thread no longer active, copy SP to context
    asm("mov R1, %[old_stack] \n\t"
        : [old_stack] "=m" (sched_cur_thread->sp));


    asm("mov %[new_stack], R1 \n\t"
        :
        : [new_stack] "m" (sched_next_thread->sp));

    sched_cur_thread = sched_next_thread;

    // restore register context
    asm("pop R4\n\t"
        "pop R5\n\t"
        "pop R6\n\t"
        "pop R7\n\t"
        "pop R8\n\t"
        "pop R9\n\t"
        "pop R10\n\t"
        "pop R11\n\t"
        "pop R12\n\t"
        "pop R13\n\t"
        "pop R14\n\t"
        "pop R15\n\t");

}

__attribute__((interrupt(WDT_VECTOR)))
void isr_systick(void)
{
    //P6OUT ^= 0x01;
    if(sched_cur_thread != NULL && (sched_cur_thread->flags & SCHED_FLAGS_FINISHED))
    {
        // Thread exited
        sched_cur_thread->flags = 0;
    }
#if 0
    if(sched_cur_thread->sp < sched_cur_thread->stack)
    {
        // stack crash
        while(1);
    }
#endif
    sched_next_thread = sched_cur_thread;
    while(++sched_next_thread < (sched_threads + SCHED_THREADS_MAX))
    {
        if( (sched_next_thread->flags & (SCHED_FLAGS_USED | SCHED_FLAGS_FINISHED | SCHED_BLOCK_MASK)) == SCHED_FLAGS_USED)
        {
            goto switch_tasks;
        }
    }
    sched_next_thread = sched_threads;
switch_tasks:
    if(sched_next_thread != sched_cur_thread)
    {
        // trigger interrupt to switch context
        CACTL1 |= CAIFG;
    }
    return;
}

#define WDT_CONFIG  WDT_ADLY_1_9
void scheduler_init(void)
{
    WDTCTL = WDT_CONFIG;
    IE1 |= WDTIE;
    CACTL1 |= CAIE;  // Using unused comparator as software interrupt

    sched_threads[0].flags = SCHED_FLAGS_USED;
    sched_threads[0].stack = (uint16_t*)0x4000;
    sched_threads[0].stack_size = 1024;

    sched_cur_thread = sched_threads;

    __bis_SR_register(GIE);

}

void sched_thread_exit(void)
{
    // This is the termination point for a thread
    sched_cur_thread->flags |= SCHED_FLAGS_FINISHED;
    IFG1 |= WDTIFG;
}


int main (void);
__attribute__((critical))
thread_t sched_thread_start(thread_func function, void *context, uint16_t *stack, uint16_t stack_size)
{
    int counter;
    uint16_t *sp;
    for(counter=0; counter < SCHED_THREADS_MAX; counter++)
    {
        if( ! (sched_threads[counter].flags & SCHED_FLAGS_USED) )
        {
            sched_threads[counter].flags = SCHED_FLAGS_USED;
            sched_threads[counter].stack = stack;
            sched_threads[counter].stack_size = stack_size;
            
            sp = stack + stack_size;
            sp--;
            // build up the context

            *sp-- = 0x3FFF;
            *sp-- = ((uint16_t)main) + 2;
            *sp-- = ((uint16_t)sched_thread_exit);
            *sp-- = ((uint16_t)function);
            *sp-- = GIE;   // SR
            *sp-- = (uint16_t)context;   // R12

            // Fake register context save
            *sp-- = 0; // R4
            *sp-- = 0; // R5
            *sp-- = 0; // R6
            *sp-- = 0; // R7
            *sp-- = 0; // R8
            *sp-- = 0; // R9
            *sp-- = 0; // R10
            *sp-- = 0; // R11
            *sp-- = 0; // R12
            *sp-- = 0; // R13
            *sp-- = 0; // R14
            *sp   = 0; // R15
            
            sched_threads[counter].sp = sp;
            
            
            return (thread_t)(sched_threads + counter);
        }
    }
    while(1)
    {
        // ran out of threads
        // block until fixed
    }
    return NULL;
}

__attribute__((critical))
void os_thread_set_flags(thread_t thread, uint16_t flags)
{
    sched_thread_info_t *t;
    t = (sched_thread_info_t *)thread;
    t->flags |= flags;
}

__attribute__((critical))
void os_thread_clr_flags(thread_t thread, uint16_t flags)
{
    sched_thread_info_t *t;
    t = (sched_thread_info_t *)thread;
    t->flags &= ~flags;
}

void * os_get_active_msg_queue(void)
{
    return sched_cur_thread->msg_queue;
}
void os_set_active_msg_queue(void* queue)
{
    sched_cur_thread->msg_queue = queue;
}


void * os_get_msg_queue(thread_t thread)
{
    return ((sched_thread_info_t*)thread)->msg_queue;
}


void * os_get_active_thread(void)
{
    return sched_cur_thread;
}

