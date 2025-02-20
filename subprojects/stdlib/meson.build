project('stdlib', 'c')

extern_library_helper_dep = subproject('vinumc').get_variable('extern_library_helper_dep')

stdlib_srcs = [
  'stdlib.c',
]

stdlib = shared_library(
  'stdlib',
  stdlib_srcs,
  dependencies : extern_library_helper_dep,
  install : true,
)
