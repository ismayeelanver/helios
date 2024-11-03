project('Helios', 'cpp', version: '0.1.0')


src = ['main.cpp', 'helios.cpp', 'config.cpp']

dependencies = [dependency('xcb'), dependency('tomlplusplus'), dependency('fmt'), dependency('xcb-cursor'), dependency('xcb-ewmh')]

executable('bin/helios', src, dependencies: dependencies)
