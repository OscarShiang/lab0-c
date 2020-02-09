#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

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
    if (q == NULL)
        return;

    if (q->head != NULL) {
        list_ele_t *pos = q->head;
        list_ele_t *next;
        while (pos != NULL) {
            if (pos->value != NULL) {
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
    newh = malloc(sizeof(list_ele_t));
    if (newh == NULL)
        return false;
    /* Don't forget to allocate space for the string and copy it */
    /* What if either call to malloc returns NULL? */

    // connect the link
    newh->next = q->head;
    q->head = newh;
    if (q->tail == NULL)
        q->tail = newh;

    // copy the string
    int str_len = strlen(s);
    char *str = malloc(str_len + 1);
    if (str == NULL)
        return false;

    strncpy(str, s, str_len);
    str[str_len] = '\0';
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
    /* TODO: You need to write the complete code for this function */
    if (!q)
        return false;

    list_ele_t *newt = malloc(sizeof(list_ele_t));
    if (newt == NULL)
        return false;
    newt->next = NULL;

    // initialize the queue if q is NULL
    if (q->head == NULL) {
        q->head = newt;
    } else {
        list_ele_t *tail = q->tail;
        tail->next = newt;
    }
    q->tail = newt;

    // cpoy the string
    int str_len = strlen(s);
    char *str = malloc(str_len + 1);
    if (str == NULL)
        return false;

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
    /* TODO: You need to fix up this code. */
    if (!q || !q->head)
        return false;

    // break the connection of the head element
    list_ele_t *rm = q->head;
    q->head = q->head->next;

    // copy string into sp if sp is non-NULL
    if (sp) {
        strncpy(sp, rm->value, bufsize);
        sp[bufsize] = '\0';
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
    /* TODO: You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
    if (q == NULL || q->head == NULL)
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
    /* TODO: You need to write the code for this function */
    if (!q || !q->head)
        return;

    list_ele_t *o_head = q->head;
    list_ele_t *o_tail = q->tail;

    list_ele_t *prev = q->head;
    list_ele_t *next, *pos = q->head->next;
    q->head->next = NULL;
    while (pos != NULL) {
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
void q_sort(queue_t *q)
{
    /* TODO: You need to write the code for this function */
    /* TODO: Remove the above comment when you are about to implement. */
}
