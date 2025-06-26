project('vinumc', 'c', version: '0.1.0')

flex = find_program('flex')
bison = find_program('bison')

lex = generator(flex,
    output : [ '@BASENAME@.c', '@BASENAME@.h' ],
    arguments : [
        '--outfile=@OUTPUT0@',
        '--header-file=@OUTPUT1@',
        '@INPUT@'
    ]
)

parse = generator(bison,
    output : [ '@BASENAME@.c', '@BASENAME@.h' ],
    arguments : [
        '-d', '@INPUT@', '-v',
        '--output=@OUTPUT0@',
        '--defines=@OUTPUT1@'
    ]
)

common_srcs = [
  'ast.c',
  parse.process('dry_bison.y'),
  lex.process('dry_flex.l'),
  'eval.c',
  'str.c',
  'library_loader.c',
]

vinumc_srcs = common_srcs + [
  'vinumc.c',
]

inc = include_directories('include')

vinumc_dep = declare_dependency(
  include_directories : inc,
)

if meson.version().version_compare('>=0.62.0')
  dl_dep = dependency('dl')
else
  dl_dep = meson.get_compiler('c').find_library('dl', required: true)
endif

headers = [
  'include/extern_library.h',
]

install_headers(headers, subdir: 'vinumc')

vinumc = executable(
  'vinumc',
  vinumc_srcs,
  include_directories : inc,
  install : true,
  dependencies: dl_dep
)

vin2dot_srcs = common_srcs + [
  'vin2dot.c',
]

if get_option('build_vin2dot')
  executable(
    'vin2dot',
    vin2dot_srcs,
    include_directories : inc,
    install : true,
    dependencies: dl_dep
  )
endif

subdir('tests')
