vsl_srcs = [
  'vsl.c',
]

vsl = shared_library(
  'vsl',
  vsl_srcs,
  include_directories : [
    external_inc
  ],
  install : true,
)

vsl_dep = declare_dependency(
  link_with : vsl,
)
