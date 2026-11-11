#include"ring_buffer.h"
#include<stdio.h>

void RING_BUFFER_INIT(Ringbuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->buffer_overflow =0;
}

bool RING_BUFFER_HEAD(Ringbuffer *rb, uint8_t data)

{
    if(rb->count >= BUFFER_SIZE)
    {
        rb->buffer_overflow++;
        return false;
    }


        rb->buffer[rb->head]=data;
        rb->head = rb->head+1;

    /*wrap around 0 to 7*/
    if (rb->head >= BUFFER_SIZE)
    {
        rb->head = 0;
    }

    /* Update number of stored bytes */
    rb->count++; 
    
    return true;

}

bool RING_BUFFER_TAIL(Ringbuffer *rb, uint8_t *data)
{
    if ((rb == NULL) || (data == NULL ))
    {
        /*buffer is empty*/
        return false;
    }

    if (rb->count == 0U)
    {
        return false;
    }

    *data = rb->buffer[rb->tail];   // give the retrieved byte to data
    rb->tail = rb->tail+1;

    /*wrap around 0 to 7*/
    if (rb->tail >= BUFFER_SIZE)
    {
        rb->tail = 0;
    }

    rb->count--;   // decrement count if consumer reads one byte the number of unread bytes must decrease by one

    return true;
}