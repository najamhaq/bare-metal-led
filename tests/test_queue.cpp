//
// Created by najamhaq on 2026-01-16.
//
#include "message.h"
#include "queue.h"
#include "unity.h"

void setUp()
{
}

void tearDown()
{
}

void test_queue_empty()
{
  Message queueStorage[32];
  Queue queue(queueStorage, 32);

  TEST_ASSERT_TRUE(queue.isEmpty());
}

void test_queue_not_empty()
{
  Message queueStorage[32];
  Queue queue(queueStorage, 32);

  Message message;
  queue.push(message);

  TEST_ASSERT_FALSE(queue.isEmpty());
}


void test_queue_push_message()
{
  Message queueStorage[32];
  Queue queue(queueStorage, 32);

  Message message;
  queue.push(message);

  TEST_ASSERT_FALSE(queue.isEmpty());
}


void test_queue_pop_message_fails_on_empty()
{
  Message queueStorage[32];
  Queue queue(queueStorage, 32);

  Message message;

  TEST_ASSERT_FALSE(queue.pop(message));
}

void test_queue_pop_message_fails_on_reading_beyond()
{
  Message queueStorage[32];
  Queue queue(queueStorage, 32);

  Message message;
  queue.push(message);

  queue.pop(message); // first pop should succeed

  TEST_ASSERT_FALSE(queue.pop(message));
}


void test_queue_push_message_fails_on_capacity()
{
  Message queueStorage[3];
  Queue queue(queueStorage, sizeof(queueStorage) / sizeof(queueStorage[0]) );

  Message message;
  queue.push(message);
  queue.push(message);
  queue.push(message);

  TEST_ASSERT_FALSE(queue.push(message));
}

void test_queue_push_message_rollover_capacity()
{
  Message queueStorage[2];
  Queue queue(queueStorage, sizeof(queueStorage) / sizeof(queueStorage[0]) );

  Message message;
  queue.push(message);
  queue.push(message);  // queue full
  queue.pop(message);  // queue now has space at the beginning

  TEST_ASSERT_TRUE(queue.push(message));
}


void test_queue_push_message_rollover_capacity_full()
{
  Message queueStorage[3];
  Queue queue(queueStorage, sizeof(queueStorage) / sizeof(queueStorage[0]) );

  Message message;
  queue.push(message);
  queue.push(message);
  queue.push(message);  // queue full
  queue.pop(message);  // queue now has space at the beginning
  queue.push(message);  // fill it again in the beginning
  TEST_ASSERT_FALSE(queue.push(message));
}



void test_queue_pop_message_rollover_capacity()
{
  Message queueStorage[2];
  Queue queue(queueStorage, sizeof(queueStorage) / sizeof(queueStorage[0]) );

  Message message;
  queue.push(message);
  queue.push(message);  // queue full
  queue.pop(message);  // queue now has space at the beginning

  queue.push(message);  // queue full
  queue.pop(message);  // queue now has space at the beginning
  TEST_ASSERT_TRUE(queue.pop(message));
}

int main()
{
  UNITY_BEGIN();
  RUN_TEST(test_queue_empty);
  RUN_TEST(test_queue_not_empty);
  RUN_TEST(test_queue_pop_message_fails_on_empty);
  RUN_TEST(test_queue_pop_message_fails_on_reading_beyond);
  RUN_TEST(test_queue_push_message_fails_on_capacity);
  RUN_TEST(test_queue_push_message_rollover_capacity);
  RUN_TEST(test_queue_push_message_rollover_capacity_full);
  RUN_TEST(test_queue_pop_message_rollover_capacity);
  return UNITY_END();
}
