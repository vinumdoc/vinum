project('vunit', 'c')

inc = include_directories('include')

srcs = [
  'vunit.c',
]

libvunit = static_library(
  'vunit',
  srcs,
  include_directories : inc,
  install : true,
)

vunit_dep = declare_dependency(
  include_directories : inc,
  link_with : libvunit,
)
