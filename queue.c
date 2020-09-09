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
    if (!q)
        return;

    /* Free the string inside the elements and the elements */
    list_ele_t *curr = q->head;
    while (curr) {
        list_ele_t *next = curr->next;
        free(curr->value);
        free(curr);
        curr = next;
    }

    /* Free queue structure */
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

    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;
    newh->next = NULL;

    /* Don't forget to allocate space for the string and copy it */
    /* What if either call to malloc returns NULL? */
    size_t len = strlen(s) + 1;
    newh->value = malloc(len);
    if (!newh->value) {
        free(newh);
        return false;
    }

    /* Copy the string */
    strncpy(newh->value, s, len);

    /* Concatenate the new element */
    if (!q->tail)
        q->tail = newh;
    newh->next = q->head;
    q->head = newh;

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
    /* Remember: It should operate in O(1) time */
    if (!q)
        return false;

    list_ele_t *newt = malloc(sizeof(list_ele_t));
    if (!newt)
        return false;
    newt->next = NULL;

    size_t len = strlen(s) + 1;
    newt->value = malloc(len);
    if (!newt->value) {
        free(newt);
        return false;
    }

    /* Copy the string */
    strncpy(newt->value, s, len);

    /* Concatenate */
    if (!q->head)
        q->head = newt;
    else
        q->tail->next = newt;
    q->tail = newt;

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

    list_ele_t *rm = q->head;
    q->head = q->head->next;

    /* Copy the string when sp exists */
    if (sp) {
        strncpy(sp, rm->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    free(rm->value);
    free(rm);

    /* When the elements in the list had all been removed */
    if (!q->head)
        q->tail = NULL;

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
    if (!q)
        return 0;
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
    if (!q || !q->head)
        return;

    /* Store original head and tail */
    list_ele_t *orig_head = q->head;
    list_ele_t *orig_tail = q->tail;

    list_ele_t *curr = q->head;

    while (curr != orig_tail) {
        list_ele_t *next = curr->next;
        curr->next = q->tail->next;
        q->tail->next = curr;
        curr = next;
    }

    q->head = orig_tail;

    /* Cut off the tail */
    q->tail = orig_head;
    q->tail->next = NULL;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */

static list_ele_t *merge_list(list_ele_t *l1, list_ele_t *l2)
{
    if (!l2)
        return l1;
    if (!l1)
        return l2;

    list_ele_t *curr, *head;

    if (strcmp(l1->value, l2->value) < 0) {
        head = l1;
        l1 = l1->next;
    } else {
        head = l2;
        l2 = l2->next;
    }

    curr = head;

    while (l1 && l2) {
        if (strcmp(l1->value, l2->value) < 0) {
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

static list_ele_t *sort_list(list_ele_t *head)
{
    if (!head || !head->next)
        return head;

    /* Split the list into 2 parts */
    list_ele_t *fast = head->next;
    list_ele_t *slow = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    /* split each list */
    list_ele_t *l1 = sort_list(head);
    list_ele_t *l2 = sort_list(fast);

    /* merge and sort l1 and l2 */
    return merge_list(l1, l2);
}

void q_sort(queue_t *q)
{
    if (!q || q->size <= 1)
        return;

    /* Sort the list */
    q->head = sort_list(q->head);

    /* Update the new tail */
    list_ele_t *newt = q->head;
    while (newt->next)
        newt = newt->next;
    q->tail = newt;
}
