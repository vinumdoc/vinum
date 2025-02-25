project('stdlib', 'c')

extern_library_dep = subproject('vinumc').get_variable('extern_library_dep')

stdlib_srcs = [
  'stdlib.c',
]

stdlib = shared_library(
  'stdlib',
  stdlib_srcs,
  dependencies : extern_library_dep,
  install : true,
)
