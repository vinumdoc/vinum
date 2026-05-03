project('vinum', 'c', meson_version: '>=0.61.2',
  default_options : [
    'c_std=c17',
    'werror=true',
    'warning_level=2',
    'c_args=-Wformat-security',
  ]
)

subproject('vutils')
subproject('vunit')
subproject('vinumc')
subproject('vutils_tests')
