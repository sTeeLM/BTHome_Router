idf_component_register(SRCS "terminal.c" "terminal_do_net.c" "terminal_do_bl.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "logger"
                    REQUIRES "console"
                    REQUIRES "driver"
                    REQUIRES "delay"
                    REQUIRES "esp_driver_uart")
