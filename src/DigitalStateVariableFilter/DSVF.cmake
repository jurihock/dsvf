add_library(dsvf
  INTERFACE)

file(
  GLOB_RECURSE
    HDR "${CMAKE_CURRENT_LIST_DIR}/*.h"
    CPP "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

target_sources(dsvf
  PRIVATE
    "${HDR}"
    "${CPP}")

target_include_directories(dsvf
  INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/..")

target_compile_features(dsvf
  INTERFACE
    cxx_std_20)
