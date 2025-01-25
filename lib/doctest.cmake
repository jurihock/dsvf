# https://github.com/doctest/doctest

CPMAddPackage(
  NAME doctest
  VERSION 2.4.11
  GIT_REPOSITORY https://github.com/doctest/doctest
  DOWNLOAD_ONLY YES)

if(doctest_ADDED)

  add_library(doctest INTERFACE)

  target_include_directories(doctest
    INTERFACE "${doctest_SOURCE_DIR}/doctest")

endif()
