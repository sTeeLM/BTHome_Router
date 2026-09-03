#include <stdio.h>

#include "logger.h"
#include "config.h"
#include "nvs_wrapper.h"
#include "nm.h"
#include "clock.h"
#include "task.h"
#include "sm.h"
#include "terminal.h"
#include "cjson_wrapper.h"

void app_main(void)
{
  logger_init();
  config_init();
  nvs_wrapper_init();
  nm_init();
  clock_init();
  task_init();
  sm_init();
  terminal_init();
  cjson_wrapper_init();

  while(1) {
    task_run();
    terminal_run();
  }
}
