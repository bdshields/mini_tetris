/*
 * o_allocator.c
 *
 *  Created on: 10 Dec. 2022
 *      Author: brandon
 */

#include <stddef.h>
#include "o_allocator.h"

#define OBJECT_TYPE_MAX     10

#define O_FLAG_USED 0x0001

typedef struct o_type_s{
    void       *list;
    uint16_t    obj_size;
    uint16_t    obj_count;
}o_allocator_t;

o_allocator_t o_list[OBJECT_TYPE_MAX]={0};


o_allocator o_initialise(void *buffer, uint16_t element_count, uint16_t element_size)
{
    int counter;
    for(counter=0; counter<OBJECT_TYPE_MAX; counter++)
    {
        if(o_list[counter].list == NULL)
        {
            o_list[counter].list = buffer;
            o_list[counter].obj_size = element_size;
            o_list[counter].obj_count = element_count;
            return (o_allocator) o_list + counter;
        }
    }
    return NULL;
}

void *o_allocate(o_allocator pool)
{
    o_allocator_t   *allocator = (o_allocator_t*)pool;
    uint8_t         *object;
    uint8_t         *end;

    object = allocator->list;
    end = ((uint8_t *)allocator->list) + (allocator->obj_count * allocator->obj_size);

    while(object < end)
    {
        if((((o_base_obj_t*)object)->flags & O_FLAG_USED) == 0)
        {
            // found a free slot
            ((o_base_obj_t*)object)->flags |= O_FLAG_USED;
            return object;
        }
        else
        {
            object += allocator->obj_size;
        }
    }
    return NULL;
}

void o_free(void *object)
{
    ((o_base_obj_t*)object)->flags &= ~O_FLAG_USED;
}
