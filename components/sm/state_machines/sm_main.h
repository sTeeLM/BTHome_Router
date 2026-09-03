#ifndef SOL_SM_MAIN_H
#define SOL_SM_MAIN_H

#include "task.h"
#include "sm.h"

extern const char * sm_states_names_main[];
extern const sm_trans_t * sm_trans_main[];

enum sm_states_main
{
  SM_MAIN_INIT        // 关机
};

#endif  // SOL_SM_MAIN_H