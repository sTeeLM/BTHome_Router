#include <stdio.h>

#include "logger.h"
#include "config.h"
#include "nvs_wrapper.h"
#include "btm.h"

void app_main(void)
{
  logger_init();
  config_init();
  nvs_wrapper_init();
  btm_init();
}
