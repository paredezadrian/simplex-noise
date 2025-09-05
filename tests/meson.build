if get_option('tests')
  test_sources = [
    'test_basic.c',
    'test_config.c',
    'test_performance.c'
  ]
  
  foreach test_name : test_sources
    test_exe = executable(
      test_name.underscorify(),
      test_name,
      include_directories: include_directories('..', '../include'),
      link_with: libsimplex,
      dependencies: m_dep
    )
    
    test(test_name, test_exe)
  endforeach
endif
