idf_component_register(SRCS "terminal.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "logger"
                    REQUIRES "console"
                    REQUIRES "driver"
                    REQUIRES "delay"
                    REQUIRES "esp_driver_uart")
