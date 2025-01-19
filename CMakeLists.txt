cmake_minimum_required(VERSION 3.1...3.18)

project(DigitalStateVariableFilterPlugin VERSION 1.0)

option(FASTMATH "Enable fast math"    ON)
option(LOGGING  "Enable runtime logs" OFF)
option(CHRONO   "Enable timing logs"  OFF)
option(WARNINGS "Enable all warnings" ON)

include("${CMAKE_CURRENT_LIST_DIR}/lib/CPM.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/JUCE.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/Pluginval.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/src/DigitalStateVariableFilterPlugin/Plugin.cmake")
