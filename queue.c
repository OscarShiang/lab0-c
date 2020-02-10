#include <ctype.h>
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
    if (!q) {
        return;
    }

    if (q->head == NULL) {
        return;
    }

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
int compare_right(char const *a, char const *b)
{
    int bias = 0;

    for (;; a++, b++) {
        if (!isdigit(*a) && !isdigit(*b))
            return bias;
        if (!isdigit(*a))
            return -1;
        if (!isdigit(*b))
            return 1;

        if (*a < *b) {
            if (!bias)
                bias = -1;
        } else if (*a > *b) {
            if (!bias)
                bias = 1;
        } else if (!*a && !*b)
            return bias;
    }
    return 0;
}

int compare_left(char const *a, char const *b)
{
    for (;; a++, b++) {
        if (!isdigit(*a) && !isdigit(*b))
            return 0;
        if (!isdigit(*a))
            return -1;
        if (!isdigit(*b))
            return 1;
        if (*a < *b)
            return -1;
        if (*a > *b)
            return 1;
    }
    return 0;
}

int strnatcmp(char const *a, char const *b)
{
    int ai, bi;
    int fractional, result;

    ai = bi = 0;
    while (1) {
        while (isspace(a[ai]))
            ++ai;

        while (isspace(b[bi]))
            ++bi;

        if (isdigit(a[ai]) && isdigit(b[bi])) {
            fractional = (a[ai] == '0' || b[bi] == '0');

            if (fractional) {
                if ((result = compare_left(a + ai, b + bi)) != 0)
                    return result;
            } else {
                if ((result = compare_right(a + ai, b + bi)) != 0)
                    return result;
            }
        }

        if (!a[ai] && !b[bi])
            return 0;

        if (a[ai] < b[bi])
            return -1;
        if (a[ai] > b[bi])
            return 1;

        ++ai;
        ++bi;
    }
}

list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    if (!l2)
        return l1;
    if (!l1)
        return l2;

    if (strnatcmp(l1->value, l2->value) < 0) {
        l1->next = merge(l1->next, l2);
        return l1;
    } else {
        l2->next = merge(l1, l2->next);
        return l2;
    }
}

list_ele_t *mergeSortList(list_ele_t *head)
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
    list_ele_t *l1 = mergeSortList(head);
    list_ele_t *l2 = mergeSortList(fast);

    // merge sorted l1 and l2
    return merge(l1, l2);
}

void q_sort(queue_t *q)
{
    /* TODO: You need to write the code for this function */
    if (!q)
        return;
    else if (q->head == NULL)
        return;
    else if (q->size <= 1)
        return;

    q->head = mergeSortList(q->head);
}
