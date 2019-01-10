#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
  extern "C" {
#endif

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
 * $Id: timer.h,v 1.3 2004/09/29 20:41:55 j Exp $
 */

#include <avr/io.h>

/* argument to pass to an action function */
union timeoutarg
{
  uint16_t i;
  uint8_t v;
  char c;
  void *p;
};

/* the action function itself */
typedef void (*timeout_t)(union timeoutarg);
/* we maintain a 32-bit clock */
typedef uint32_t time_t;

/*
 * Insert one-shot timer `tm' ticks after now.  Call `fun'
 * with `arg' when timer expires.
 *
 * Returns handle to timer queue entry that can be saved for
 * later deletion of the event.  Returns 0 if fails (no more
 * memory for allocating a timer).
 */
void *timeout(time_t tm, timeout_t fun, union timeoutarg arg);

/*
 * Delete the timer handle from the queue.  Returns 0 for
 * success, -1 if event was not found in queue.
 */
int untimeout(void *handle);

/*
 * Must be called per hardware timer tick from main().  Should
 * not be called directly from within an interrupt service, since
 * the callback functions for expired timers will be called within
 * the context of timertick(), and since the queue handling has no
 * precautions against being interrupted.  So normally, the
 * interrupt service will just mark a flag, and upon returning to
 * the main loop, this flag will be used to run timertick().
 *
 * If the entire timer library is compiled with the TMR_INTPROTECT
 * defined, everything will be protected so it is safe to be called
 * from within an interrupt context.  In that case, the interrupt
 * handler should initially perform all the time-critical
 * housekeeping, then re-enable interrupts, and finally call
 * timertick().
 */
void timertick(void);

#ifdef __cplusplus
  }
#endif

#endif /* TIMER_H */
