#include "terminal.h"
#include "driver/uart.h"
#include "esp_console.h"
#include "esp_system.h"
#include "linenoise/linenoise.h"
#include "logger.h"
#include "delay.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "termios.h"

#include <fcntl.h>
#include <unistd.h>


static const char *TAG = "TERMINAL";

static bool terminal_running; // 用于控制 shell 循环的标志位

static esp_console_repl_t *terminal_repl = NULL;

static int do_exit_cmd(int argc, char **argv) 
{
  BTR_LOGI(TAG, "Exiting shell and returning to main loop...");
  ESP_ERROR_CHECK(terminal_repl->del(terminal_repl));
  terminal_repl = NULL;
  terminal_running = false; // 修改标志位，退出 shell 循环
  return 0;
}

static int do_hello_cmd(int argc, char **argv) 
{
  BTR_LOGI(TAG, "Hello from ESP32-C3 Shell!");
  return 0;
}

static void terminal_register_commands(void)
{
  esp_console_cmd_t exit_cmd = {.command = "exit",
                                .help = "Exit the interactive shell",
                                .hint = NULL,
                                .func = &do_exit_cmd,
                                .argtable = NULL};
  ESP_ERROR_CHECK(esp_console_cmd_register(&exit_cmd));

  esp_console_cmd_t hello_cmd = {.command = "hello",
                                 .help = "Print hello message",
                                 .hint = NULL,
                                 .func = &do_hello_cmd,
                                 .argtable = NULL};
  ESP_ERROR_CHECK(esp_console_cmd_register(&hello_cmd));

  // 也可以注册系统自带的 help 命令
  esp_console_register_help_command();
}

void terminal_init(void) 
{
  BTR_LOGI(TAG, "init");
  terminal_running = false; // 初始化标志位为 false
  terminal_repl = NULL; // 初始化 repl 指针为 NULL
}

void terminal_run_interactive_shell(void) 
{
  BTR_LOGI(TAG, "terminal_run_interactive_shell");
  esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
  esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();

  repl_config.prompt = "BTHOME_ROUTER>";
  repl_config.max_cmdline_length = 256;
  repl_config.max_cmdline_args = 8;

  ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &terminal_repl));
  terminal_register_commands();
  esp_console_register_help_command();
  ESP_ERROR_CHECK(esp_console_start_repl(terminal_repl));
}


void terminal_run(void) 
{
  int c;
  if(terminal_running) {
    delay_ms(1000); // 避免 CPU 占用过高
    return;
  } else {
    c = fgetc(stdin);
    // 如果读取到了非 EOF 字符（这里检测到回车 '\n' 或 '\r' 时触发进入 Shell）
    if (c != EOF && (c == '\r' || c == '\n')) {
      BTR_LOGI(TAG, "[Trigger Detected] Entering Shell...");

      terminal_running = true; // 设置标志位，表示正在运行 shell

      // 进入 Shell 交互，阻塞直到用户输入 exit
      terminal_run_interactive_shell();

      BTR_LOGI(TAG, "Back in main loop. Press [Enter] to re-enter shell.");
    } else {
      // 如果不是回车，继续等待
      BTR_LOGD(TAG, "Waiting for [Enter] to enter shell...");
      delay_ms(1000); // 避免 CPU 占用过高
    }
  }
}
