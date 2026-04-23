project('vunit', 'c', version: '0.0.1')

local_inc = include_directories('include/vunit')
external_inc = include_directories('include')

srcs = [
  'vunit.c',
]

install_subdir(
  'include',
  install_dir: 'include',
)

vutils_dep = dependency('vutils', fallback : ['vutils', 'vutils_dep'])

libvunit = library(
  'vunit',
  srcs,
  include_directories : local_inc,
  install : true,
  dependencies: [
    vutils_dep,
  ],
)

vunit_dep = declare_dependency(
  include_directories : external_inc,
  link_with : libvunit,
  dependencies: [
    vutils_dep,
  ],
)
