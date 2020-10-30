cmake_minimum_required(VERSION 3.8)

project(waysig VERSION "0.3.1" LANGUAGES CXX)

option(WAYSIG_BUILD_TESTS "Build waysig's unit tests" ON)
option(WAYSIG_BUILD_TESTS_WAYLAND "Build waysig's test against the wayland abi" ON)

add_library(waysig INTERFACE)
add_library(waysig::waysig ALIAS waysig)
target_include_directories(
  waysig
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

if (WAYSIG_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  waysig-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

set(WAYSIG_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/waysig)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/waysig-config.cmake.in waysig-config.cmake @ONLY)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/waysig-config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/waysig-config.cmake"
  INSTALL_DESTINATION ${WAYSIG_CMAKE_DIR}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

install(
  TARGETS waysig
  EXPORT waysig-targets)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/waysig-config-version.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/waysig-config.cmake"
  DESTINATION ${WAYSIG_CMAKE_DIR})

install(
  EXPORT waysig-targets
  FILE waysig-targets.cmake
  NAMESPACE waysig::
  DESTINATION ${WAYSIG_CMAKE_DIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
  DESTINATION .)
