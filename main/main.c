#include <stdio.h>

#include "logger.h"
#include "config.h"
#include "nvs_wrapper.h"
#include "nm.h"
#include "ws2812b.h"
#include "delay.h"
#include "terminal.h"
#include "cjson_wrapper.h"
#include "bthome_parser.h"

void app_main(void)
{
  logger_init();
  delay_init();
  cjson_wrapper_init(); 
  bthome_parser_init(); 
  ws2812b_init();
  config_init();
  nvs_wrapper_init();
  nm_init();
  terminal_init();

  ws2812b_set_background(0, 0, 255);
  ws2812b_refresh();

  

  while(1) {
    terminal_run();
  }
}
