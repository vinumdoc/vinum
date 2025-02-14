fs = import('fs')

tests = [
  'simple_test.c',
  'library_test.c'
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

  test(
    basename,
    test_exe,
    env : test_env,
    protocol : 'tap',
    verbose : true,
    depends: vinumc,
  )
endforeach
