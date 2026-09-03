idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES config logger nvs_wrapper nm clock task sm cjson_wrapper terminal)
