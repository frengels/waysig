tests = []

wayland_tests = [
    'wl/abi',
]

if get_option('test_wayland')
    wayland_dep = dependency('wayland-server')
    tests += wayland_tests
endif

catch_lib = static_library(
    'catch2',
    'catch2.cpp',
    dependencies: catch2_dep,
)

foreach t : tests
    test_exe = executable(
        t.underscorify(),
        '@0@.cpp'.format(t),
        link_with: catch_lib,
        dependencies: [ waysig_dep, catch2_dep ],
    )
    test(t, test_exe)
endforeach