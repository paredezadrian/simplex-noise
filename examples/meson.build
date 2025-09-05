if get_option('examples')
  example_sources = [
    'example_2d.c',
    'example_3d.c',
    'example_config.c',
    'example_fractal.c'
  ]
  
  foreach example_name : example_sources
    executable(
      example_name.underscorify(),
      example_name,
      include_directories: include_directories('..', '../include'),
      link_with: libsimplex,
      dependencies: m_dep,
      install: true
    )
  endforeach
endif
