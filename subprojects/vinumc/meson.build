project('vinumc', 'c')

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
]

vinumc_srcs = common_srcs + [
  'vinumc.c',
]

vinumc = executable(
  'vinumc',
  vinumc_srcs,
  install : true,
)

vin2dot_srcs = common_srcs + [
  'vin2dot.c',
]

if get_option('build_vin2dot')
  executable(
    'vin2dot',
    vin2dot_srcs,
    install : true,
  )
endif

subdir('tests')
