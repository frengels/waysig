set(TESTS
    signal)

set(WAYLAND_TESTS
    wl_abi)

find_package(Catch2 REQUIRED)
find_package(PkgConfig REQUIRED)


pkg_check_modules(WaylandServer REQUIRED IMPORTED_TARGET wayland-server)

add_library(catch2 STATIC catch2.cpp)
target_link_libraries(catch2 PUBLIC Catch2::Catch2)

function(make_test target)
    string(CONCAT target_src ${target} ".cpp")
    string(CONCAT target_wl ${target} "_wl")
    add_executable(${target} ${target_src})
    target_link_libraries(${target} PRIVATE waysig::waysig catch2)
    add_test(NAME ${target} COMMAND ${target})
    add_executable(${target_wl} ${target_src})
    target_link_libraries(${target_wl} PRIVATE waysig::waysig PkgConfig::WaylandServer catch2)
    target_compile_definitions(${target_wl} PRIVATE WAYSIG_ENABLE_WL)
    add_test(NAME ${target_wl} COMMAND ${target_wl})
endfunction()

foreach(t ${TESTS})
    make_test(${t})
endforeach()

if(WAYSIG_BUILD_TESTS_WAYLAND)
    foreach(t ${WAYLAND_TESTS})
        make_test(${t})
	target_link_libraries(${t} PRIVATE PkgConfig::WaylandServer)
    endforeach()
endif()
