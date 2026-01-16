//
// Created by mnajm on 2026-01-16.
//

#ifndef MICROBIT_BAREMETAL_MESSAGE_H
#define MICROBIT_BAREMETAL_MESSAGE_H

#include <stdint.h>
typedef struct {
    uint16_t type;
    uint16_t param;
    uint32_t data;
} Message ;


typedef struct {
    uint16_t type;
    uint16_t param;
    uint32_t data;
} msg_t;

typedef struct {
    msg_t  *buf;
    size_t  cap;   // number of messages
    size_t  head;  // write
    size_t  tail;  // read
    size_t  len;   // count
} msg_queue_t;


#endif //MICROBIT_BAREMETAL_MESSAGE_H