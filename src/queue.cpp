//
// Created by mnajm on 2026-01-16.
//

#include <stddef.h>
#include "message.h"
#include "queue.h"

Queue::Queue()  : m_buf(nullptr), m_capacity(0), m_read(0), m_write(0), m_message_count(0)
{
  return ;
}

Queue:: Queue(Message* buffer, size_t capacity)  : m_buf(buffer), m_capacity(capacity), m_read(0), m_write(0), m_message_count(0)
{
  return ;
}

void Queue::init(Message* buffer, size_t capacity)
{
  m_buf = buffer ;
  m_capacity= capacity ;
  m_read = 0 ;
  m_write =0 ;
  m_message_count =0 ;
  return ;
}

bool Queue::isEmpty() const
{
    return m_message_count == 0;
}

bool Queue::isFull() const
{
    return m_message_count == m_capacity;
}


bool Queue::push(const Message& msg)
{
  if (isFull())  {
    return false;
  }
   // This needs to account for concurrency with main loop popping messages , or elsewhere
  if (m_write >= m_capacity) {
    // rollover to the beginning
    if (m_read ==  0) {
      // no space
      return false;
    }
    m_write = 0;
  }

  m_buf[m_write] = msg;
  m_message_count++;
  m_write++;
  return true;
}

// Consumer side (main loop)
bool Queue::pop(Message& out)
{
  if (m_message_count == 0)
  {
    return false;
  }
  // This needs to account for concurrency with ISR pushing messages

  if (m_message_count > 0 && m_read >= m_capacity) {
    // rollover to the beginning
    m_read = 0;
  }
  out = m_buf[m_read];
  m_message_count--;
  m_read++;
  return true;
}



