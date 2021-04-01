/*
 * sched.c - functions for process statistics
 */

#include <stats.h>
#include <utils.h> // for get_ticks()
#include <sched.h>



void init_stats (struct stats* s) {
  s->user_ticks = 0;
  s->system_ticks = 0;
  s->blocked_ticks = 0;
  s->ready_ticks = 0;
  s->elapsed_total_ticks = get_ticks();
  s->total_trans = 0;
  s->remaining_ticks = 0;
}

void chstat_sys_to_user () { // system run - to user run
  struct stats* s = &current()->stats;
  s->system_ticks += get_ticks() - s->elapsed_total_ticks;
  s->elapsed_total_ticks = get_ticks();
}

void chstat_user_to_sys () { // user run - to system run
  struct stats* s = &current()->stats;
  s->user_ticks += get_ticks() - s->elapsed_total_ticks;
  s->elapsed_total_ticks = get_ticks();
}

void chstat_sys_to_ready () {
  struct stats* s = &current()->stats;
  s->system_ticks += get_ticks() - s->elapsed_total_ticks;
  s->elapsed_total_ticks = get_ticks();
}

void chstat_ready_to_sys () {
  struct stats* s = &current()->stats;
  s->ready_ticks += get_ticks() - s->elapsed_total_ticks;
  s->total_trans ++;
  s->elapsed_total_ticks = get_ticks();
}

