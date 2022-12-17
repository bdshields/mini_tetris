/*
 * o_allocator.h
 *
 *  Created on: 10 Dec. 2022
 *      Author: brandon
 */

#ifndef SRC_O_ALLOCATOR_H_
#define SRC_O_ALLOCATOR_H_

#include <stdint.h>

typedef struct o_base_obj_s{
    uint16_t    flags;
}o_base_obj_t;

typedef void* o_allocator;


o_allocator o_initialise(void *buffer, uint16_t element_count, uint16_t element_size);
void *o_allocate(o_allocator pool);
void o_free(void *object);


#endif /* SRC_O_ALLOCATOR_H_ */
