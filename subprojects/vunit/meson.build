project('vunit', 'c')

local_inc = include_directories('include/vunit')
external_inc = include_directories('include')

srcs = [
  'vunit.c',
]

install_subdir(
  'include',
  install_dir: 'include',
)

libvunit = library(
  'vunit',
  srcs,
  include_directories : local_inc,
  install : true,
)

vunit_dep = declare_dependency(
  include_directories : external_inc,
  link_with : libvunit,
)
