#ifndef BTHOME_ROUTER_TASK_H
#define BTHOME_ROUTER_TASK_H

#include <stdbool.h>

// max 64
typedef enum _task_event_t
{           
  EV_INIT                = 0,
  EV_TIMEO,
  EV_1S,                 // 大约每1s转一下
  EV_CNT  
} task_event_t;

extern const char * task_names[];

typedef void (*TASK_PROC)(task_event_t);

void task_init (void);

void task_set(task_event_t ev);
bool task_test_clr(task_event_t ev);
void task_run(void);


#endif  // NEW_IV_CLOCK_TASK_H
