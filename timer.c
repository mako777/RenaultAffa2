/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):                  by Poul-Henning Kamp
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Simple timer queue management
 *
 * $Id: timer.c,v 1.4 2004/09/29 20:41:55 j Exp $
 */

//#include <inttypes.h>
//#include <stdlib.h>

#ifdef TMR_INTPROTECT
#include <avr/interrupt.h>
#endif /* TMR_INTPROTECT */

//#include <avr/io.h>

#include "timer.h"
#include <stdlib.h>
//#include <inttypes.h>

struct timerq
{
  struct timerq *next;
  time_t when;
  timeout_t action;
  union timeoutarg arg;
};

static struct timerq *tqhead;
static time_t now;

void
timertick(void)
{
  struct timerq *p, *q;
#ifdef TMR_INTPROTECT
  uint8_t sreg;
#endif /* TMR_INTPROTECT */

  now++;

  q = p = 0;

  /*
   * First, find the cutoff entry.  Everything from current tqhead up
   * to that entry needs to be triggered afterwards.  Adjust tqhead
   * accordingly, so it points onto the first entry that needs to be
   * triggered in future.
   */
#ifdef TMR_INTPROTECT
  sreg = SREG;
  cli();
#endif /* TMR_INTPROTECT */
  while (tqhead)
    {
      if (now < tqhead->when)
	/* no more actions by now */
	break;

      if (!q)
	/* first event to trigger now, remember it */
	q = tqhead;

      p = tqhead;
      tqhead = tqhead->next;
    }
#ifdef TMR_INTPROTECT
  SREG = sreg;
#endif /* TMR_INTPROTECT */

  if (!q)
    /* nothing to be done */
    return;

  /*
   * Now, tqhead points to the new queue (that is to be considered
   * next time).  The previous head of the queue, up to the cutoff
   * entry, contains timers that are expired now, and their callback
   * functions needs to be run.  It is important that the queue
   * starting at tqhead is stable again, since the callback functions
   * are expected to possibly insert new timers as well.
   *
   * Mark the tail of this subchain, then walk through it, call each
   * callback function in succession, and free up that entry.
   */
  p->next = 0;
  for (p = q; p;)
    {
      p->action(p->arg);
      q = p;
      p = p->next;
      free(q);
    }
}

void *
timeout(time_t tm, timeout_t fun, union timeoutarg arg)
{
  struct timerq *p, *q, *newent;
#ifdef TMR_INTPROTECT
  uint8_t sreg;
#endif /* TMR_INTPROTECT */

  if ((newent = malloc(sizeof(struct timerq))) == 0)
    return 0;

  newent->action = fun;
  newent->arg = arg;
  tm += now;
  newent->when = tm;

  /*
   * If queue is currently empty, just put this element onto top of
   * queue, and we are done.
   */
#ifdef TMR_INTPROTECT
  sreg = SREG;
  cli();
#endif /* TMR_INTPROTECT */
  if (tqhead == 0)
    {
      tqhead = newent;
      newent->next = 0;
#ifdef TMR_INTPROTECT
      SREG = sreg;
      goto out;
#else /* TMR_INTPROTECT */
      return newent;
#endif /* TMR_INTPROTECT */
    }

  /*
   * If there are existing entries, find the position with the first
   * element that is to be triggered after us.  Always remember our
   * predecessor, so we can adjust their `next' field after queue
   * insertion.  If there was no prececessor, we've got a new tqhead
   * instead.
   */
  for (p = tqhead, q = 0; p; q = p, p = p->next)
    {
      if (tm < p->when)
	{
	  /*
	   * Insert before current entry, update previous in chain (or
	   * head of queue).
	   */
	  newent->next = p;
	  if (q)
	    q->next = newent;
	  else
	    tqhead = newent;

#ifdef TMR_INTPROTECT
	  SREG = sreg;
	  goto out;
#else /* TMR_INTPROTECT */
	  return newent;
#endif /* TMR_INTPROTECT */
	}
    }
  /*
   * Fell off the end of the queue: must append new entry to tail,
   * and update previous tail.
   */
  q->next = newent;
  newent->next = 0;

#ifdef TMR_INTPROTECT
  SREG = sreg;
 out:
#endif /* TMR_INTPROTECT */
  return newent;
}

/*
 * Just try looking up `handle' in the queue, and remove it.  Again,
 * fix up the `next' field of our predecessor, or missing one, fix up
 * tqhead instead.
 */
int
untimeout(void *handle)
{
  struct timerq *ev = handle, *p, *q;
#ifdef TMR_INTPROTECT
  uint8_t sreg;
#endif /* TMR_INTPROTECT */

#ifdef TMR_INTPROTECT
  sreg = SREG;
  cli();
#endif /* TMR_INTPROTECT */
  for (p = tqhead, q = 0; p; q = p, p = p->next)
    {
      if (p == ev)
	{
	  if (q)
	    q->next = p->next;
	  else
	    tqhead = p->next;
	  free(p);
#ifdef TMR_INTPROTECT
	  SREG = sreg;
#endif /* TMR_INTPROTECT */
	  return 0;
	}
    }
#ifdef TMR_INTPROTECT
  SREG = sreg;
#endif /* TMR_INTPROTECT */

  return -1;
}

