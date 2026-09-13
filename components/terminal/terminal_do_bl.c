#include "terminal_do_bl.h"
#include "terminal.h"

#include <string.h>

static void do_bl_cmd_usage(void) {
  terminal_printf("Usage: bl <command>\n");
  terminal_printf("Available commands:\n");
  terminal_printf("  info - Display black list information\n");
  terminal_printf("  add <device_id> - Add device to black list\n");
  terminal_printf("  remove <device_id> - Remove device from black list\n");
  terminal_printf("  clear - Clear all devices from black list\n");
}

static int do_bl_cmd(int argc, char **argv) {
  if (argc < 2) {
    do_bl_cmd_usage();
  } else {
    if (strcmp(argv[1], "info") == 0 && argc == 2) {
      // 在这里实现显示黑名单信息的逻辑
      terminal_printf("Black List Information:\n");
      terminal_printf("Device ID: 123456\n");
      terminal_printf("Device ID: 789012\n");
    } else if (strcmp(argv[1], "add") == 0 && argc == 3) {
      // 在这里实现添加设备到黑名单的逻辑
      terminal_printf("Device %s added to black list.\n", argv[2]);
    } else if (strcmp(argv[1], "remove") == 0 && argc == 3) {
      // 在这里实现从黑名单中移除设备的逻辑
      terminal_printf("Device %s removed from black list.\n", argv[2]);
    } else if (strcmp(argv[1], "clear") == 0 && argc == 2) {
      // 在这里实现清空黑名单的逻辑
      terminal_printf("All devices cleared from black list.\n");
    } else {
      terminal_printf("Unknown command or incorrect number of arguments.\n");
      do_bl_cmd_usage();
      return 1;
    }
  }
  return 0;
}

const esp_console_cmd_t terminal_cmd_bl = {.command = "bl",
                                           .help =
                                               "Manage black list information",
                                           .hint = NULL,
                                           .func = &do_bl_cmd,
                                           .argtable = NULL};