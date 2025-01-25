add_executable(test)

file(
  GLOB_RECURSE
    HDR "${CMAKE_CURRENT_LIST_DIR}/*.h"
    CPP "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

target_sources(test
  PRIVATE
    "${HDR}"
    "${CPP}")

target_include_directories(test
  PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/..")

target_link_libraries(test
  PRIVATE
    doctest
    dsvf
    numcpp)
