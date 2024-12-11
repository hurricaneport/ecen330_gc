idf_component_register(
SRCS
  sfx_hit.c sfx_point.c sfx_wing.c
INCLUDE_DIRS
  .
)
# PRIV_REQUIRES driver
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
