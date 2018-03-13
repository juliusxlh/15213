/* 
 * Code for basic C skills diagnostic.
 * Developed for courses 15-213/18-213/15-513 by R. E. Bryant, 2017
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdlib.h>
#include <stdio.h>

#include "harness.h"
#include "queue.h"

/*
  Create empty queue.
  Return NULL if could not allocate space.
*/
queue_t *q_new()
{
    queue_t *qn= (queue_t*) malloc(sizeof(queue_t));
    qn->size = (int *) malloc(sizeof(int));
    if (!qn) return NULL;
    qn->head = NULL;
    qn->tail = NULL;
    *(qn->size) = 0;
    /* Remember to handle the case if malloc returned NULL */
    return qn;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* Remember to free the queue structue and list elements */
    list_ele_t *nextp = q->head;
    list_ele_t *nowp = q->head;
    while (!nextp){
        nextp=nextp->next;
        free(nowp);
        nowp=nextp;
    }
    free(q->size);
    q->size = NULL;
    free(q);
    q = NULL;
}

/*
  Attempt to insert element at head of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
 */
bool q_insert_head(queue_t *q, int v)
{
	
    /* What should you do if the q is NULL? */
    if (!q) return false;
    list_ele_t *insp = (list_ele_t*)malloc(sizeof(list_ele_t));
    /* What if malloc returned NULL? */
    if (!insp) return false;
    insp->value = v;
    insp->next = q->head;
    q->head = insp;
    ++*(q->size);
    if (*(q->size) == 1) q->tail = insp;
    return true;
}


/*
  Attempt to insert element at tail of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
 */
bool q_insert_tail(queue_t *q, int v)
{
    if (!q) return false;
    list_ele_t *insp = (list_ele_t*)malloc(sizeof(list_ele_t));
    /* Remember: It should operate in O(1) time */
    if (!insp) return false;
    insp->value = v;
    insp->next = NULL;
    if (q->size){
        q->tail->next = insp;
        q->tail = insp;
    } else q->tail = insp;
    ++*(q->size);
    if (*(q->size) == 1) q->head = insp;
    return true;
}
/*
  Attempt to remove element from head of queue.
  Return true if successful.
  Return false if queue is NULL or empty.
  If vp non-NULL and element removed, store removed value at *vp.
  Any unused storage should be freed
*/
bool q_remove_head(queue_t *q, int *vp)
{
    if (!q||!(q->head)) return false;
    if (vp) *vp = q->head->value;
    list_ele_t *delp = q->head;
    q->head = q->head->next;
    --*(q->size);
    free(delp);
    delp = NULL;
    return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* Remember: It should operate in O(1) time */
    if (!q) return 0;else
    return *(q->size);
}

/*
  Reverse elements in queue.

  Your implementation must not allocate or free any elements (e.g., by
  calling q_insert_head or q_remove_head).  Instead, it should modify
  the pointers in the existing data structure.
 */
void q_reverse(queue_t *q)
{
    if (q) {
        if (q->head){
            q->tail = q->head;
            list_ele_t *p1 = q->head;
            list_ele_t *p2 = p1->next;
            while (p2){
                list_ele_t *p3 = p2->next;
                p2->next = p1;
                p1 = p2;
                p2 = p3;
            }
            q->head = p1;
            q->tail->next = NULL;
            }
    }
}


