project('vutils', 'c', version: '0.1.0')

local_inc = include_directories('include/vutils')
external_inc = include_directories('include')

install_subdir(
  'include',
  install_dir: 'include',
)

srcs = [
  'allocator.c',
  'str.c',
]

libvutils = library(
  'vutils',
  srcs,
  include_directories : local_inc,
  install : true,
)

vutils_dep = declare_dependency(
  include_directories : external_inc,
  link_with : libvutils,
)
