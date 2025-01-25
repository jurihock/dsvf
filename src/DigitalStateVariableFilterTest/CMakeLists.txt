project(DigitalStateVariableFilterTest)

add_executable(${PROJECT_NAME})

file(
  GLOB_RECURSE
    HDR "${CMAKE_CURRENT_LIST_DIR}/*.h"
    CPP "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

target_sources(${PROJECT_NAME}
  PRIVATE
    "${HDR}"
    "${CPP}")

target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/..")

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    doctest
    dsvf
    numcpp)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    OUTPUT_NAME "test")
