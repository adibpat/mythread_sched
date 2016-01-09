
/*
 * myqueue.c -- implementation for queue ops
 */
#define FALSE 0
#define TRUE 1
#include "mythread.h"
#include <malloc.h>
#include <stdio.h>
int mythread_inq(mythread_queue_t *headp, void *item)
{
  mythread_queue_t old, q, head;

  head = *headp;

#ifndef TRASH
  if (head == NULL)
    return FALSE;
#endif

  q = head;
  do {
    if (q->item == item)
      return TRUE; /* found */
    q = q->next;
  } while (q != head);

  return FALSE; /* not found */
}

int mythread_lenq(mythread_queue_t *headp)
{
  mythread_queue_t old, q, head;
  int ret = 0;

  head = *headp;

#ifndef TRASH
  if (head == NULL)
    return ret;
#endif

  q = head;
  do {
    q = q->next;
    ret++;
  } while (q != head);

  return ret;
}

void mythread_q_init(mythread_queue_t *headp, void *item)
{
  mythread_queue_t new, head;
  new = (mythread_queue_t) malloc(sizeof(struct mythread_queue));
  if (new == NULL) {
    printf("malloc error\n");
    exit(1);
  }
  new->item = item;

  new->prev = new;
  new->next = new;

  *headp = new;
  return;
}

void mythread_enq_tail(mythread_queue_t *headp, void *item)
{
  mythread_queue_t new, head, temp;

  if (*headp == NULL) {
    mythread_q_init(headp, item);
    return;
  }

  head = *headp;
  new = (mythread_queue_t) malloc(sizeof(struct mythread_queue));
  if (new == NULL) {
    printf("malloc error\n");
    exit(1);
  }

  new->item = item;
  temp = head->prev;
  
  //Traverse the queue until the priority of the current thread matches with some thread's priority in the queue
  while(((mythread_t)(temp->item))->preemptions != ((mythread_t)(new->item))->preemptions && 
  	((mythread_t)(temp->item))->preemptions > ((mythread_t)(new->item))->preemptions && temp!=head)
  {
  	temp = temp->prev;
  }
  //Insert the thread
  head = temp->next;

  new->prev = head->prev;
  new->next = head;
  head->prev->next = new;
  head->prev = new;

  head = *headp;
  //Only one element in the queue and it's priority is lesser then current thread's move head pointer of the queue.
  if(temp==head && ((mythread_t)(temp->item))->preemptions > ((mythread_t)(new->item))->preemptions)
  {
  	head = head->prev;
  }
  *headp = head;
  return;
  
}

void mythread_enq_head(mythread_queue_t *headp, void *item)
{
  mythread_queue_t new, head;

  if (*headp == NULL) {
    mythread_q_init(headp, item);
    return;
  }

  head = *headp;

  mythread_enq_tail(&head->next, item);

  *headp = head->prev;
}

void mythread_deq(mythread_queue_t *headp, void *item)
{
  mythread_queue_t old, q, head;

  head = *headp;

  q = head;
  do {
    if (q->item == item) {
      old = q;
      old->next->prev = q->prev;
      old->prev->next = q->next;
      if (old == head) {
	if (old->next == old)
	  *headp = (mythread_queue_t) NULL;
	else
	  *headp = old->next;
      }
      free(old);
      return; /* found */
    }
    q = q->next;
  } while (q != head);

  /* not found */
}


