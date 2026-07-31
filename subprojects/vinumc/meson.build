project('vinumc', 'c', version: '0.2.0')

flex = find_program('flex')
bison = find_program('bison')

# Bison and flex run as custom_targets so their outputs land at a single
# build-dir path the parser lib and the rest of the subproject can include.
bison_gen = custom_target('dry_bison',
    input : 'dry_bison.y',
    output : [ 'dry_bison.c', 'dry_bison.h' ],
    command : [
        bison, '-d', '@INPUT@', '-v',
        '--output=@OUTPUT0@',
        '--defines=@OUTPUT1@',
    ],
)

lex_gen = custom_target('dry_flex',
    input : 'dry_flex.l',
    output : [ 'dry_flex.c', 'dry_flex.h' ],
    command : [
        flex,
        '--outfile=@OUTPUT0@',
        '--header-file=@OUTPUT1@',
        '@INPUT@',
    ],
    depends : bison_gen,
)

cc = meson.get_compiler('c')

local_inc = include_directories('include/vinumc')
external_inc = include_directories('include')
generated_inc = include_directories('.')

vutils_dep = dependency('vutils', fallback : ['vutils', 'vutils_dep'])

# Flex/Bison skeletons can trigger warnings we don't control. Add -Wno-error only for the
# the generated sources.
parser_lib = static_library('vinumc_parser',
  bison_gen,
  lex_gen,
  include_directories : [local_inc, generated_inc],
  dependencies : vutils_dep,
  c_args : cc.get_supported_arguments(['-Wno-error']),
)

parser_dep = declare_dependency(
  link_with : parser_lib,
  sources : [bison_gen[1], lex_gen[1]],
  include_directories : generated_inc,
  dependencies : vutils_dep,
)

if meson.version().version_compare('>=0.62.0')
  dl_dep = dependency('dl')
else
  dl_dep = cc.find_library('dl', required: true)
endif

install_subdir(
  'include',
  install_dir: 'include',
)

subdir('vsl')

libvinumc = library(
  'vinumc',
  sources: [
      'ast.c',
      'eval.c',
      'library_loader.c',
      'libvinumc.c',
      'v_lib.c',
  ],
  include_directories: [
    local_inc,
    generated_inc,
  ],
  dependencies: [
     dl_dep,
     vutils_dep,
     parser_dep,
     vsl_dep,
   ],
  install: true
)

libvinumc_dep = declare_dependency(
  link_with: libvinumc,
  include_directories: external_inc,
)

libvinumc_dep_priv = declare_dependency(
  link_with: libvinumc,
  sources : [bison_gen[1], lex_gen[1]],
  include_directories: [external_inc, libvinumc.private_dir_include()],
)

vinumc = executable(
  'vinumc',
  sources: [
    'vinumc.c',
  ],
  include_directories : local_inc,
  install : true,
  dependencies: [
    vutils_dep,
    libvinumc_dep_priv,
  ],
)

vinumc_dep = declare_dependency(
  include_directories : external_inc,
)

if get_option('build_vin2dot')
  executable(
    'vin2dot',
    sources: [
      'vin2dot.c',
    ],
    include_directories : local_inc,
    install : true,
    dependencies: [
      vutils_dep,
      libvinumc_dep_priv,
    ],
  )
endif

conf_data = configuration_data()
conf_data.set_quoted('vinumc_version', meson.project_version())

configure_file(input : 'config.h.in',
  output : 'config.h',
  configuration : conf_data)

subdir('tests')
