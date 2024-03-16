#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "shuffle.h"

bool q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return false;

    int len = q_size(head);

    for (struct list_head *tail = head->prev; tail != head;
         tail = tail->prev, len--) {
        struct list_head *target = head->next;
        int j = rand() % len;
        while (j--)
            target = target->next;
        if (tail == target)
            continue;
        struct list_head *temp = target->prev;
        list_move(target, tail);
        list_move(tail, temp);
        tail = target;
    }
    return true;
}
