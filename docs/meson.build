py = import('python').find_installation(modules: [
  'hawkmoth'
])

sphinx_build = find_program('sphinx-build')

docs_src = meson.current_source_dir() / 'source/'
docs_out = meson.current_build_dir() / 'html/'

conf_data = configuration_data()
conf_data.set('BUILD_ROOT', meson.project_build_root())
conf_data.set('SOURCE_ROOT', meson.project_source_root())

configured_conf = configure_file(
    input: 'source/conf.py.in',
    output: 'conf.py',
    configuration: conf_data
)

vinumc_sp = subproject('vinumc')
bison_target = vinumc_sp.get_variable('bison_gen')
libvinumc_target = vinumc_sp.get_variable('libvinumc')

docs = files(
  './source/build.rst',
  './source/contributing.rst',
  './source/index.rst',
)

subdir('source/subprojects/vinumc')
subdir('source/subprojects/vutils')
subdir('source/subprojects/vunit')

custom_target(
    'docs',
    output: 'html',
    input: [
      docs,
      configured_conf,
    ],
    depends: [
      bison_target,
      libvinumc_target,
    ],
    command: [
        sphinx_build,
        '-v',
        '-b', 'html',
        '-a',
        # Add flags to show more warnings
        '-n', '-W', '--keep-going',
        '-c', meson.current_build_dir(),
        docs_src,
        docs_out
    ],
    build_by_default: false,
    console: true,
)

sphinx_autobuild = find_program('sphinx-autobuild', required: false)

if sphinx_autobuild.found()
  custom_target(
      'watch-docs',
      output: 'dhtml',
      input: [
        docs,
        configured_conf,
      ],
      depends: [
        bison_target,
        libvinumc_target,
      ],
      command: [
          sphinx_autobuild,
          '-v',
          '-b', 'html',
          '-a',
          # Add flags to show more warnings
          '-n', '-W', '--keep-going',
          '-c', meson.current_build_dir(),
          '--watch', meson.project_source_root() + '/subprojects',
          docs_src,
          docs_out
      ],
      build_by_default: false,
      console: true,
  )
endif
