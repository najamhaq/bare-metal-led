// #include "queue.h"
//
//
// class queue
// {
//
//     void mq_init(msg_queue_t *q, msg_t *storage, size_t capacity) {
//         q->buf = storage;
//         q->cap = capacity;
//         q->head = 0;
//         q->tail = 0;
//         q->len  = 0;
//     }
//
//     bool mq_is_empty(const msg_queue_t *q) { return q->len == 0; }
//     bool mq_is_full (const msg_queue_t *q) { return q->len == q->cap; }
//     size_t mq_size(const msg_queue_t *q) { return q->len; }
//
//     bool mq_push_isr(msg_queue_t *q, const msg_t *m) {
//         if (q->cap == 0) return false;
//
//         mq_enter_critical();
//         if (mq_is_full(q)) {
//             mq_exit_critical();
//             return false;
//         }
//
//         q->buf[q->head] = *m;                 // copy message
//         q->head = (q->head + 1) % q->cap;
//         q->len++;
//
//         mq_exit_critical();
//         return true;
//     }
//
//     bool mq_pop(msg_queue_t *q, msg_t *out) {
//         if (q->cap == 0) return false;
//
//         mq_enter_critical();
//         if (mq_is_empty(q)) {
//             mq_exit_critical();
//             return false;
//         }
//
//         if (out) *out = q->buf[q->tail];
//         q->tail = (q->tail + 1) % q->cap;
//         q->len--;
//
//         mq_exit_critical();
//         return true;
//     }
//
// };
