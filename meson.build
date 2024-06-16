project('3drender', ['c', 'cpp'],
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++14'])

eigen_inc = include_directories('extlib/eigen-3.4.0')
eigen_dep = declare_dependency(include_directories : eigen_inc)

flex = find_program('flex', version : '>=2.6')
bison = find_program('bison', version : '>=3.5')

flex_target_for_scanner = custom_target('flex generator',
    command: [flex, '-v', '-o', '@OUTPUT@', '@INPUT@'],
    input: 'scanner.l',
    output: 'scanner.yy.cpp'
)

bison_target_for_parser = custom_target('bison generator',
    command: [bison, '-o', '@OUTPUT0@', '@INPUT@', '-d'],
    input: 'parser.y',
    output: ['parser.tab.cpp', 'parser.tab.hpp']
)

executable('3drender',
  sources: [
    'main.cpp',
    'ast.cpp',
    'extlib/libattopng/libattopng.c',
    flex_target_for_scanner,
    bison_target_for_parser
  ],
  dependencies: [eigen_dep],
  install : true
)
