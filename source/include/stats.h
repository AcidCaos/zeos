/*
 * sched.h - functions for process statistics
 */


#ifndef STATS_H
#define STATS_H

//#include <sched.h>

/* Structure used by 'get_stats' function */
struct stats {
  unsigned long user_ticks;       // Total ticks executing user code
  unsigned long system_ticks;     // Total ticks executing system code
  unsigned long blocked_ticks;    // Total ticks in the blocked state
  unsigned long ready_ticks;      // Total ticks in the ready state
  unsigned long elapsed_total_ticks; // Ticks from power-on to the beggining of the current state
  unsigned long total_trans;      // Number of times the process has got the CPU: READY->RUN transitions 
  unsigned long remaining_ticks;  // Remaining ticks to end the quantum 
};


void init_stats (struct stats* s);

void chstat_sys_to_user ();
void chstat_user_to_sys ();

void chstat_sys_to_ready ();
void chstat_ready_to_sys ();


#endif /* !STATS_H */
