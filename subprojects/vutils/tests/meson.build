fs = import('fs')

tests = [
  'vec_macros.c',
]

vunit_dep = dependency('vunit', fallback : ['vunit', 'vunit_dep'])

foreach t : tests
  basename = fs.stem(t)

  test_exe = executable(
    basename,
    t,
    dependencies : [
      vunit_dep,
      vutils_dep,
    ],
  )

  test_kwargs = {}
  if meson.version().version_compare('>=0.62.0')
    test_kwargs += {'verbose': true}
  endif

  test(
    basename,
    test_exe,
    protocol : 'tap',
    kwargs: test_kwargs,
  )
endforeach
