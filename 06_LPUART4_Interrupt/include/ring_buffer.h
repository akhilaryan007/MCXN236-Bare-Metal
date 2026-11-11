#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 8U

typedef struct
{
    uint8_t buffer[BUFFER_SIZE];

    uint32_t head;
    uint32_t tail;
    uint32_t count;

    uint32_t buffer_overflow;

} Ringbuffer;

void RING_BUFFER_INIT(Ringbuffer *rb);

bool RING_BUFFER_HEAD(Ringbuffer *rb, uint8_t data);

bool RING_BUFFER_TAIL(Ringbuffer *rb, uint8_t *data);

#endif