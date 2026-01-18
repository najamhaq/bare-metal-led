// queue.h
#pragma once
#include <stddef.h>
#include "message.h"

class Queue {
public:
    Queue(); // default construtor
    // Constructor: storage provided by caller (no heap)
    Queue(Message* buffer, size_t capacity);
    // we have to uise init as there is no heap allocation in baremetal
    void init(Message* buffer, size_t capacity);

    // Producer side (ISR-safe)
    bool push(const Message& msg);

    // Consumer side (main loop)
    bool pop(Message& out);

    // State / diagnostics
    bool isEmpty() const;
    bool isFull() const;
    size_t size() const;
    size_t capacity() const;

private:
    Message* m_buf;
    size_t   m_capacity;
    size_t   m_read;  // read index into the m_buf . We move index forward AFTER the read
    size_t   m_write; // write index into the m_buf . We move index forward AFTER the write
    size_t   m_message_count;  // current number of messages in the queue


    // no copying
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
};
