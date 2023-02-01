#pragma once

#define _LIST_DECLARE(list, type, prev, next) \
    struct list { \
        int len; \
        type* first; \
        type* last; \
    }
#define LIST_DECLARE(...) _LIST_DECLARE(__VA_ARGS__)

#define _LIST_DEFINE(list, type, prev, next) struct list list
#define LIST_DEFINE(...) _LIST_DEFINE(__VA_ARGS__)

#define _LIST_SPOT(list, type, prev, next) \
    struct { \
        type* prev; \
        type* next; \
    }
#define LIST_SPOT(...) _LIST_SPOT(__VA_ARGS__)

#define _LIST_APPEND(list, type, prev, next, item) \
    do { \
        list.len++; \
        if (!list.last) \
            list.first = (item); \
        else \
            list.last->next = (item); \
        list.last = item; \
        (item)->next = 0; \
    } while (0)
#define LIST_APPEND(...) _LIST_APPEND(__VA_ARGS__)

#define _LIST_REMOVE(list, type, prev, next, item) \
    do { \
        list.len--; \
        if ((item) == list.first) \
            list.first = (item)->next; \
        if ((item) == list.last) \
            list.last = (item)->prev; \
        if ((item)->prev) \
            (item)->prev->next = (item)->next; \
        if ((item)->next) \
            (item)->next->prev = (item)->prev; \
        (item)->prev = 0; \
        (item)->next = 0; \
    } while (0)
#define LIST_REMOVE(...) _LIST_REMOVE(__VA_ARGS__)

#define _LIST_FOREACH(list, type, prev, next) \
    for (type* item = list.first; item != 0; item = item->next)
#define LIST_FOREACH(...) _LIST_FOREACH(__VA_ARGS__)
