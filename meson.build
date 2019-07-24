project(
    'waysig',
    'cpp',
    version: '0.1.0',
    license: 'MIT',
    meson_version: '>=0.48.0',
    default_options: [
        'cpp_std=c++17',
        'warning_level=3',
    ],
)

waysig_inc = include_directories('include')

waysig_dep = declare_dependency(
    include_directories: [ waysig_inc ],
)

install_subdir('include/ws', install_dir: 'include')

pkgconfig = import('pkgconfig')
pkgconfig.generate(
    name: meson.project_name(),
    filebase: meson.project_name(),
    version: meson.project_version(),
    description: 'C++ signal/slot implementation based on wl_signal/listener',
)

if get_option('tests')
    catch2_dep = dependency('catch2')
    subdir('tests')
endif