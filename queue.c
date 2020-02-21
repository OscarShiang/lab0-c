#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#include "natsort/strnatcmp.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* Free queue structure */
    if (!q)
        return;

    if (q->head) {
        list_ele_t *pos = q->head;
        list_ele_t *next;
        while (pos) {
            if (pos->value) {
                free(pos->value);
            }
            next = pos->next;
            free(pos);
            pos = next;
        }
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newh;
    char *str;

    int len = strlen(s);

    // allocate the memory
    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;
    str = malloc(len + 1);
    if (!str) {
        free(newh);
        return false;
    }

    // connect the link
    newh->next = q->head;
    q->head = newh;
    if (!q->tail)
        q->tail = newh;

    // copy the string
    strncpy(str, s, len);
    str[len] = '\0';
    newh->value = str;

    q->size++;

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newt;
    char *str;
    int str_len = strlen(s);

    // alocate memory
    newt = malloc(sizeof(list_ele_t));
    if (!newt)
        return false;
    str = malloc(str_len + 1);

    if (!str) {
        free(newt);
        return false;
    }

    // connect the list
    newt->next = NULL;

    if (!q->head)
        q->head = newt;
    else {
        list_ele_t *tail = q->tail;
        tail->next = newt;
    }
    q->tail = newt;

    // cpoy the string
    strncpy(str, s, str_len);
    str[str_len] = '\0';

    newt->value = str;
    q->size++;

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head)
        return false;

    // break the connection of the head element
    list_ele_t *rm = q->head;
    q->head = q->head->next;

    // copy string into sp if sp is non-NULL
    if (sp && rm->value) {
        strncpy(sp, rm->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    // free the element
    free(rm->value);
    free(rm);
    q->size--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* Remember: It should operate in O(1) time */
    if (!q || !q->head)
        return 0;
    else
        return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q)
        return;

    if (!q->head)
        return;

    list_ele_t *o_head = q->head;
    list_ele_t *o_tail = q->tail;

    list_ele_t *prev = q->head;
    list_ele_t *next, *pos = q->head->next;
    q->head->next = NULL;
    while (pos) {
        next = pos->next;
        pos->next = prev;
        prev = pos;
        pos = next;
    }

    q->head = o_tail;
    q->tail = o_head;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    if (!l2)
        return l1;
    if (!l1)
        return l2;

    list_ele_t *curr, *head;

    if (strnatcmp(l1->value, l2->value) < 0) {
        head = l1;
        l1 = l1->next;
    } else {
        head = l2;
        l2 = l2->next;
    }

    curr = head;

    while (l1 && l2) {
        if (strnatcmp(l1->value, l2->value) < 0) {
            curr->next = l1;
            l1 = l1->next;
        } else {
            curr->next = l2;
            l2 = l2->next;
        }
        curr = curr->next;
    }

    if (l1)
        curr->next = l1;
    if (l2)
        curr->next = l2;

    return head;
}

list_ele_t *mergeSort(list_ele_t *head)
{
    if (!head || !head->next)
        return head;

    list_ele_t *fast = head->next;
    list_ele_t *slow = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    // sort each list
    list_ele_t *l1 = mergeSort(head);
    list_ele_t *l2 = mergeSort(fast);

    // merge sorted l1 and l2
    return merge(l1, l2);
}

void q_sort(queue_t *q)
{
    if (!q)
        return;
    else if (q->head == NULL)
        return;
    else if (q->size <= 1)
        return;

    q->head = mergeSort(q->head);

    list_ele_t *tail = q->head;
    while (tail->next)
        tail = tail->next;
    q->tail = tail;
}
