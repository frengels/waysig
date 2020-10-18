cmake_minimum_required(VERSION 3.8)

project(waysig VERSION "0.2.0" LANGUAGES CXX)

option(WAYSIG_BUILD_TESTS "Build waysig's unit tests" ON)
option(WAYSIG_BUILD_TESTS_WAYLAND "Build waysig's test against the wayland abi" ON)


include(CMakePackageConfigHelpers)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)
target_include_directories(
  ${PROJECT_NAME}
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

find_package(PkgConfig)
pkg_check_modules(WaylandServer IMPORTED_TARGET wayland-server)

add_library(waysig_wl INTERFACE)
add_library(waysig::waysig_wl ALIAS waysig_wl)
target_compile_features(waysig_wl INTERFACE cxx_std_11)
target_compile_definitions(waysig_wl INTERFACE WAYSIG_ENABLE_WL)
target_link_libraries(waysig_wl INTERFACE PkgConfig::WaylandServer)
target_include_directories(
  waysig_wl
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

if (WAYSIG_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

write_basic_package_version_file(
  ${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(
  TARGETS waysig waysig_wl
  EXPORT ${PROJECT_NAME}Config)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION lib/cmake/${PROJECT_NAME})

install(
  EXPORT ${PROJECT_NAME}Config
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
  DESTINATION .)
