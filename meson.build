project('simplex-noise', 'c',
  version: '2.0.0',
  license: 'MIT',
  meson_version: '>= 0.50.0',
  default_options: [
    'c_std=c99',
    'warning_level=3',
    'werror=true',
  ]
)

# Dependencies
m_dep = meson.get_compiler('c').find_library('m', required: false)

# Build options
simplex_build_shared = get_option('default_library') != 'static'
simplex_build_static = get_option('default_library') != 'shared'
simplex_build_tests = get_option('tests')
simplex_build_examples = get_option('examples')
simplex_enable_simd = get_option('simd')
simplex_enable_profiling = get_option('profiling')

# Compiler flags
c_args = []
if get_option('warning_level').to_int() >= 3
  c_args += ['-Wall', '-Wextra', '-Wpedantic']
endif

# SIMD support
if simplex_enable_simd
  c_args += ['-DSIMPLEX_ENABLE_SIMD']
  
  # Check for SIMD instruction sets
  if meson.get_compiler('c').has_argument('-mavx2')
    c_args += ['-mavx2', '-DSIMPLEX_HAVE_AVX2']
  elif meson.get_compiler('c').has_argument('-msse4.1')
    c_args += ['-msse4.1', '-DSIMPLEX_HAVE_SSE41']
  elif meson.get_compiler('c').has_argument('-mneon')
    c_args += ['-mneon', '-DSIMPLEX_HAVE_NEON']
  endif
endif

# Profiling support
if simplex_enable_profiling
  c_args += ['-DSIMPLEX_ENABLE_PROFILING']
endif

# Include directories
inc_dirs = include_directories('include')

# Source files
simplex_sources = files('src/simplex_noise.c')
simplex_headers = files('include/simplex_noise.h')

# Build library
if simplex_build_shared
  libsimplex_shared = shared_library('simplex_noise',
    sources: simplex_sources,
    include_directories: inc_dirs,
    c_args: c_args,
    dependencies: m_dep,
    version: meson.project_version(),
    soversion: '2',
    install: true
  )
endif

if simplex_build_static
  libsimplex_static = static_library('simplex_noise',
    sources: simplex_sources,
    include_directories: inc_dirs,
    c_args: c_args,
    dependencies: m_dep,
    install: true
  )
endif

# Create alias for the primary library
if simplex_build_shared
  libsimplex = libsimplex_shared
else
  libsimplex = libsimplex_static
endif

# Install headers
install_headers(simplex_headers, subdir: 'simplex_noise')

# pkg-config file
pkg = import('pkgconfig')
pkg.generate(
  name: 'Simplex Noise',
  description: 'Advanced Pure C implementation of Simplex Noise algorithm',
  version: meson.project_version(),
  libraries: libsimplex,
  subdirs: 'simplex_noise'
)

# Build tests
if simplex_build_tests
  subdir('tests')
endif

# Build examples
if simplex_build_examples
  subdir('examples')
endif

# Documentation
if get_option('docs')
  doxygen = find_program('doxygen', required: false)
  if doxygen.found()
    configure_file(
      input: 'Doxyfile.in',
      output: 'Doxyfile',
      configuration: configuration_data()
    )
    
    custom_target('docs',
      input: 'Doxyfile',
      output: 'html',
      command: [doxygen, '@INPUT@'],
      install: true,
      install_dir: 'share/doc/simplex-noise'
    )
  endif
endif

# Summary
message('')
message('Simplex Noise Configuration Summary:')
message('  Version: @0@'.format(meson.project_version()))
message('  Build type: @0@'.format(get_option('buildtype')))
message('  C standard: @0@'.format(get_option('c_std')))
message('  Build shared library: @0@'.format(simplex_build_shared))
message('  Build static library: @0@'.format(simplex_build_static))
message('  Build tests: @0@'.format(simplex_build_tests))
message('  Build examples: @0@'.format(simplex_build_examples))
message('  Enable SIMD: @0@'.format(simplex_enable_simd))
message('  Enable profiling: @0@'.format(simplex_enable_profiling))
message('  Build documentation: @0@'.format(get_option('docs')))
message('')
