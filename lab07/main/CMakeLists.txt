idf_component_register(SRCS main.c gameControl.c
                       INCLUDE_DIRS .
                       PRIV_REQUIRES config esp_timer lcd pin)
# PRIV_REQUIRES ...
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
