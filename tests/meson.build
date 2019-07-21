tests = [
    'signal',
]

wayland_tests = [
    'wl/abi',
]

test_deps = [ waysig_dep, catch2_dep ]

if get_option('test_wayland')
    wayland_dep = dependency('wayland-server')
    tests += wayland_tests
    test_deps += wayland_dep
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
        dependencies: test_deps,
    )
    test(t, test_exe)
endforeach