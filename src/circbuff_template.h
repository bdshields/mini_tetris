/*
 * circbuff_template.h
 *
 *  Created on: 10 Dec. 2022
 *      Author: brandon
 */

/**
 * CIRC_NAME - Name of circular buffer implementation
 * CIRC_TYPE - Type of each element
 *
 * --- OPTIONAL ---
 * CIRC_SIZE_TYPE     - Type for tracking size of buffer
 * CIRC_FAIL_OBJECT   - Instance of CIRC_TYPE that represents a failure
 */

#ifndef CIRC_NAME || CIRC_TYPE
    #error CIRCBUFF Template not defined correctly
    #define CIRC_NAME   template
    #define CIRC_TYPE   int
#endif


#ifndef CIRC_SIZE_TYPE
    #define CIRC_SIZE_TYPE unsigned int
#endif

#ifndef CIRC_FAIL_OBJECT
    #define CIRC_FAIL_OBJECT (-1)
#endif

#define CB_CONCAT_(_a, _b) _a ## _b
#define CB_CONCAT(_a, _b) CB_CONCAT_(_a, _b)

#define CB_EMIT(_thing) CB_CONCAT(CIRC_NAME, _thing)


#define CB_OBJECT CB_EMIT(_circbuff_t)
typedef struct CB_EMIT(_circbuff_s){
    CIRC_TYPE   *buffer;
    CIRC_TYPE   *read;
    CIRC_TYPE   *write;
    CIRC_SIZE_TYPE length;
}CB_OBJECT;

static void CB_EMIT(_init)(  CB_OBJECT* circ, CIRC_TYPE *buffer, CIRC_SIZE_TYPE length)
{
    circ->buffer = buffer;
    circ->read = buffer;
    circ->write = buffer;
    circ->length = length;
}

/**
 * Reads one element from the buffer
 * Will return CIRC_FAIL_OBJECT if empty
 */
static CIRC_TYPE CB_EMIT(_read)(CB_OBJECT* circ)
{
    if(circ->read == circ->write)
    {
        // empty
        return (CIRC_TYPE)CIRC_FAIL_OBJECT;
    }
    else
    {
        CIRC_TYPE value;
        value = *circ->read;
        circ->read++;
        if(circ->read >= (circ->buffer + circ->length))
        {
            circ->read = circ->buffer;
        }
        return value;
    }
}

static int CB_EMIT(_write)(CB_OBJECT* circ, CIRC_TYPE item)
{
    CIRC_TYPE *new_write;
    new_write = circ->write + 1;

    if(new_write >= (circ->buffer + circ->length))
    {
        new_write = circ->buffer;
    }
    if(new_write != circ->read)
    {
        *circ->write = item;
        circ->write = new_write;
        return 1;
    }
    else
    {
        // Buffer full
        return 0;
    }
}

/**
 * Returns how many elements are in the buffer
 */
static CIRC_SIZE_TYPE CB_EMIT(_pending)(CB_OBJECT* circ)
{
    if(circ->read <= circ->write)
    {
        return circ->write - circ->read;
    }
    else
    {
        return circ->length - (circ->read - circ->write);
    }
}

#undef CB_OBJECT
#undef CIRC_NAME
#undef CIRC_TYPE
#undef CIRC_FAIL_OBJECT
#undef CIRC_SIZE_TYPE

