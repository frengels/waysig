set(TESTS
    signal)

set(WAYLAND_TESTS
    wl_abi)

find_package(Catch2 REQUIRED)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH}")

find_package(Wayland REQUIRED Server)

add_library(catch2 STATIC catch2.cpp)
target_link_libraries(catch2 PUBLIC Catch2::Catch2)

function(make_test target)
    string(CONCAT target_src ${target} ".cpp")
    string(CONCAT target_wl ${target} "_wl")
    add_executable(${target} ${target_src})
    target_link_libraries(${target} PRIVATE ${CMAKE_PROJECT_NAME} catch2)
    add_test(NAME ${target} COMMAND ${target})
    add_executable(${target_wl} ${target_src})
    target_link_libraries(${target_wl} PRIVATE ${CMAKE_PROJECT_NAME} catch2 Wayland::Server)
    target_compile_definitions(${target_wl} PUBLIC WAYSIG_ENABLE_WL)
    add_test(NAME ${target_wl} COMMAND ${target_wl})
endfunction()

foreach(t ${TESTS})
    make_test(${t})
endforeach()

if(WAYSIG_BUILD_TESTS_WAYLAND)
    foreach(t ${WAYLAND_TESTS})
        make_test(${t})
        target_link_libraries(${t} PRIVATE Wayland::Server)
    endforeach()
endif()
