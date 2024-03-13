#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new) {
        return false;
    }
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new) {
        return false;
    }
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *entry = list_first_entry(head, element_t, list);
    if (sp && entry) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&entry->list);
    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *entry = list_last_entry(head, element_t, list);
    if (sp && entry) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&entry->list);
    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    int count = 0;
    struct list_head *li;
    list_for_each (li, head) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    struct list_head *first = head->next;
    struct list_head *end = head->prev;
    if (list_empty(head))
        return false;
    while (first != end && first->next != end) {
        first = first->next;
        end = end->prev;
    }
    element_t *entry = list_entry(first, element_t, list);
    list_del(first);
    q_release_element(entry);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head *cur, *next;
    bool dup = false;
    list_for_each_safe (cur, next, head) {
        element_t *entry = list_entry(cur, element_t, list);
        element_t *safe = list_entry(next, element_t, list);

        if (cur->next != head && strcmp(entry->value, safe->value) == 0) {
            // printf("entry dup: %s\n", entry->value);
            // printf("safe dup: %s\n", safe->value);
            list_del(cur);
            q_release_element(entry);
            dup = true;
        } else if (dup) {
            dup = false;
            list_del(cur);
            q_release_element(entry);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *cur, *next = NULL;

    list_for_each_safe (cur, next, head)
        list_move(cur, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    int count = 0;
    struct list_head *cur, *next = NULL;
    list_for_each_safe (cur, next, head) {
        count++;
        if (count == k) {
            struct list_head *tmp = cur->prev;
            struct list_head *tmp_prev;
            while (count > 1) {
                tmp_prev = tmp->prev;
                list_del(tmp);
                list_add(tmp, cur);
                cur = cur->next;
                tmp = tmp_prev;
                count--;
            }
            count = 0;
        }
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *first = head->next;
    struct list_head *end = head->prev;

    while (first != end && first->next != end) {
        first = first->next;
        end = end->prev;
    }

    struct list_head left_list, right_list;
    struct list_head *left_head = &left_list;
    struct list_head *right_head = &right_list;

    INIT_LIST_HEAD(left_head);
    INIT_LIST_HEAD(right_head);

    list_cut_position(left_head, head, first);
    list_splice_init(head, right_head);

    q_sort(left_head, descend);
    q_sort(right_head, descend);

    while (!list_empty(left_head) && !list_empty(right_head)) {
        element_t *left = list_entry(left_list.next, element_t, list);
        element_t *right = list_entry(right_list.next, element_t, list);
        if (descend) {
            if (strcmp(left->value, right->value) > 0) {
                list_move_tail(&left->list, head);
            } else {
                list_move_tail(&right->list, head);
            }
        } else {
            if (strcmp(left->value, right->value) < 0) {
                list_move_tail(&left->list, head);
            } else {
                list_move_tail(&right->list, head);
            }
        }
    }

    if (!list_empty(left_head))
        list_splice_tail(left_head, head);
    else
        list_splice_tail(right_head, head);
}


/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *tmp = head->next;
    char *curMin = list_entry(tmp, element_t, list)->value;
    tmp = tmp->next;
    while (tmp != head) {
        element_t *entry = list_entry(tmp, element_t, list);
        if (strcmp(curMin, entry->value) > 0) {
            struct list_head *del = tmp;
            tmp = tmp->next;
            list_del(del);
            q_release_element(entry);
        } else {
            curMin = entry->value;
            tmp = tmp->next;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *tmp = head->next;
    char *curMax = list_entry(tmp, element_t, list)->value;
    tmp = tmp->next;
    while (tmp != head) {
        element_t *entry = list_entry(tmp, element_t, list);
        if (strcmp(curMax, entry->value) < 0) {
            struct list_head *del = tmp;
            tmp = tmp->next;
            list_del(del);
            q_release_element(entry);
        } else {
            curMax = entry->value;
            tmp = tmp->next;
        }
    }
    return q_size(head);
}


/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    int len = 0;
    queue_contex_t *first_q = list_entry(head->next, queue_contex_t, chain);
    struct list_head *tmp;

    list_for_each (tmp, head) {
        if (tmp == head->next) {
            continue;
        }

        queue_contex_t *another_q = list_entry(tmp, queue_contex_t, chain);
        if (list_empty(another_q->q))
            continue;
        len += another_q->size;
        list_splice_init(another_q->q, first_q->q);
    }
    q_sort(first_q->q, descend);
    return len;
}
