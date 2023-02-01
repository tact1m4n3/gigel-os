#pragma once

#define _QUEUE_DECLARE(queue, type, next) \
    struct queue { \
        type* first; \
        type* last; \
    }
#define QUEUE_DECLARE(...) _QUEUE_DECLARE(__VA_ARGS__)

#define _QUEUE_DEFINE(queue, type, next) struct queue queue
#define QUEUE_DEFINE(...) _QUEUE_DEFINE(__VA_ARGS__)

#define _QUEUE_SPOT(queue, type, next) type* next
#define QUEUE_SPOT(...) _QUEUE_SPOT(__VA_ARGS__)

#define _QUEUE_EMPTY(queue, type, next) (!(queue.last))
#define QUEUE_EMPTY(...) _QUEUE_EMPTY(__VA_ARGS__)

#define _QUEUE_NEXT(queue, type, next) (queue.first)
#define QUEUE_NEXT(...) _QUEUE_NEXT(__VA_ARGS__)

#define _QUEUE_PUSH(queue, type, next, item) \
    do { \
        if (!queue.last) \
            queue.first = (item); \
        else \
            queue.last->next = (item); \
        queue.last = item; \
        (item)->next = 0; \
    } while (0)
#define QUEUE_PUSH(...) _QUEUE_PUSH(__VA_ARGS__)

#define _QUEUE_POP(queue, type, next) if(queue.first && !(queue.first = queue.first->next)) queue.last = 0
#define QUEUE_POP(...) _QUEUE_POP(__VA_ARGS__)
