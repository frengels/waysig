set(TESTS
    signal
    inplace_slot
    container)

set(WAYLAND_TESTS
    wl_abi)

find_package(Catch2 REQUIRED)

add_library(catch2 STATIC catch2.cpp)
target_link_libraries(catch2 PUBLIC Catch2::Catch2)

function(make_test target)
    string(CONCAT target_src ${target} ".cpp")
    add_executable(${target} ${target_src})
    target_link_libraries(${target} PRIVATE ${CMAKE_PROJECT_NAME} catch2)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

foreach(t ${TESTS})
    make_test(${t})
endforeach()

if(WAYSIG_BUILD_TESTS_WAYLAND)
    find_package(ECM REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH "${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH}")

    find_package(Wayland REQUIRED)

    foreach(t ${WAYLAND_TESTS})
        make_test(${t})
        target_link_libraries(${t} PRIVATE Wayland::Server)
    endforeach()
endif()