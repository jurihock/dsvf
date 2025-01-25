project(DigitalStateVariableFilter)

add_library(${PROJECT_NAME}
  INTERFACE)

file(
  GLOB_RECURSE
    HDR "${CMAKE_CURRENT_LIST_DIR}/*.h"
    CPP "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

target_sources(${PROJECT_NAME}
  PRIVATE
    "${HDR}"
    "${CPP}")

target_include_directories(${PROJECT_NAME}
  INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/..")

target_compile_features(${PROJECT_NAME}
  INTERFACE
    cxx_std_20)

add_library(dsvf
  ALIAS ${PROJECT_NAME})
