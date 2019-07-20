project(
    'waysig',
    'cpp',
    version: '0.0.1',
    license: 'MIT',
    meson_version: '>=0.48.0',
    default_options: [
        'cpp_std=c++17',
        'warning_level=3',
    ],
)

waysig_inc = include_directories('include')

if get_option('tests')
    catch2_dep = dependency('catch2')
    subdir('tests')
endif