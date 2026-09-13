#include "terminal.h"
#include "terminal_do_net.h"

#include <string.h>

static void do_net_cmd_usage(void) 
{
  terminal_printf("Usage: net <command>\n");
  terminal_printf("Available commands:\n");
  terminal_printf("  info - Display network information\n");
  terminal_printf("  reconnect - Reconnect to the network\n");
  terminal_printf("  list_wifi - Display available WiFi networks\n");
  terminal_printf("  ssid <ssid> - Set the SSID of the wifi to connect\n");
  terminal_printf("  pass <pass> - Set the password of the wifi to connect\n");
  terminal_printf("  ntp  <ntp_server> - Set the NTP server\n");
  terminal_printf("  report_url  <url> - Set the report URL\n");
  terminal_printf("  report_user  <user> - Set the report username\n");
  terminal_printf("  report_pass  <pass> - Set the report password\n");
}

static int do_net_cmd(int argc, char **argv) 
{
  if(argc < 2) {
    do_net_cmd_usage();
  } else {
    if(strcmp(argv[1], "info") == 0 && argc == 2) {
      // 在这里实现显示网络信息的逻辑
      terminal_printf("Network Information:\n");
      terminal_printf("IP Address: 192.168.1.100\n");
      terminal_printf("Subnet Mask: 255.255.255.0\n");
      terminal_printf("Default Gateway: 192.168.1.1\n");
    } else if(strcmp(argv[1], "reconnect") == 0 && argc == 2) {
      // 在这里实现重新连接网络的逻辑
      terminal_printf("Reconnecting to the network...\n");
    } else if(strcmp(argv[1], "ssid") == 0 && argc == 3) {
      // 在这里实现设置 SSID 的逻辑
      terminal_printf("SSID set to: %s\n", argv[2]);
    } else if(strcmp(argv[1], "pass") == 0 && argc == 3) {
      // 在这里实现设置密码的逻辑
      terminal_printf("Password set to: %s\n", argv[2]);
    } else if(strcmp(argv[1], "ntp") == 0 && argc == 3) {
      terminal_printf("NTP server set to: %s\n", argv[2]);
    } else if(strcmp(argv[1], "report_url") == 0 && argc == 3) {
      // 在这里实现设置报告 URL 的逻辑
      terminal_printf("Report URL set to: %s\n", argv[2]);
    } else if(strcmp(argv[1], "report_user") == 0 && argc == 3) {
      // 在这里实现设置报告用户名的逻辑
      terminal_printf("Report username set to: %s\n", argv[2]);
    } else if(strcmp(argv[1], "report_pass") == 0 && argc == 3) {
      // 在这里实现设置报告密码的逻辑
        terminal_printf("Report password set to: %s\n", argv[2]);
    } else {
      terminal_printf("Unknown command or incorrect number of arguments.\n");
      do_net_cmd_usage();
      return 1;
    }
  }
  return 0;
}

const esp_console_cmd_t terminal_cmd_net = {
    .command = "net",
    .help = "Manage network information",
    .hint = NULL,
    .func = &do_net_cmd,
    .argtable = NULL
};