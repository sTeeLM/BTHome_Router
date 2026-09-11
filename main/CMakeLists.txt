idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES config logger nvs_wrapper nm delay ws2812b cjson_wrapper terminal bthome_parser)
