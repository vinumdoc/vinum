fs = import('fs')

tests = [
  'cocktail_test.c',
  'help_flag_test.c',
  'library_test.c',
  'metaprog_test.c',
  'output_flag_test.c',
  'simple_test.c',
  'text_handling_test.c',
  'overview_test.c',
  'functions_test.c',
  'expansion_test.c',
  'shots_test.c',
  'compounds_test.c',
  'scope_test.c',
  'namespace_test.c',
  'document_test.c',
  'modules_test.c',
  'blanks_test.c',
  'escapes_test.c',
  'filters_test.c',
  'automatics_test.c',
  'deferral_test.c',
  'control_structures_test.c',
  'preprocessor_test.c',
]

vunit_dep = dependency('vunit', fallback : ['vunit', 'vunit_dep'])

testlib_srcs = [
  'testlib.c',
]

testlib = shared_library(
  'testlib',
  testlib_srcs,
  include_directories : [
    local_inc,
    generated_inc,
  ],
  dependencies : libvinumc_dep_priv,
  install : true,
)

foreach t : tests
  basename = fs.stem(t)

  test_exe = executable(
    basename,
    [t, 'test_helpers.c'],
    include_directories : [
      local_inc,
      generated_inc,
    ],
    dependencies : [
      vunit_dep,
      libvinumc_dep_priv,
    ],
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
    depends: [
      vinumc,
      testlib,
    ],
    kwargs: test_kwargs,
  )
endforeach
