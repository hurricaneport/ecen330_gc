idf_component_register(SRCS "bird.c" "main.c" "gameControl.c" "pipe.c"
                       INCLUDE_DIRS .
                       PRIV_REQUIRES config esp_timer lcd pin sound c24k_8b)
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
