cmake_minimum_required(VERSION 3.1...3.18)

project(DigitalStateVariableFilter)

option(FASTMATH "Enable fast math"    OFF)
option(LOGGING  "Enable runtime logs" OFF)
option(CHRONO   "Enable timing logs"  OFF)
option(WARNINGS "Enable all warnings" ON)

include("${CMAKE_CURRENT_LIST_DIR}/lib/cpm.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/juce.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/numcpp.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/doctest.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/pluginval.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/src/DigitalStateVariableFilter/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/src/DigitalStateVariableFilterPlugin/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/src/DigitalStateVariableFilterTest/CMakeLists.txt")
