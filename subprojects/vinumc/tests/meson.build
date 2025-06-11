fs = import('fs')

tests = [
  'help_flag_test.c',
  'library_test.c',
  'output_flag_test.c',
  'simple_test.c',
  'text_as_program_child_test.c',
]

vunit_dep = dependency('vunit', fallback : ['vunit', 'vunit_dep'])

foreach t : tests
  basename = fs.stem(t)

  test_exe = executable(
    basename,
    t,
    dependencies : vunit_dep,
  )

  test_env = environment()
  test_env.prepend('PATH', fs.parent(vinumc.full_path()))

  test_kwargs = {}
  if meson.version().version_compare('>=0.62.0')
    test_kwargs += {'verbose': true}
  endif

  test(
    basename,
    test_exe,
    env : test_env,
    protocol : 'tap',
    depends: vinumc,
    kwargs: test_kwargs,
  )
endforeach

testlib_srcs = [
  'testlib.c',
]

testlib = shared_library(
  'testlib',
  testlib_srcs,
  dependencies : vinumc_dep,
  install : true,
)
