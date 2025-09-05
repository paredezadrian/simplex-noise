cmake_minimum_required(VERSION 3.12)

# Project information
project(simplex_noise
    VERSION 2.0.0
    DESCRIPTION "Advanced Pure C implementation of Simplex Noise algorithm"
    LANGUAGES C
)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build options
option(SIMPLEX_BUILD_SHARED "Build shared library" ON)
option(SIMPLEX_BUILD_STATIC "Build static library" ON)
option(SIMPLEX_BUILD_TESTS "Build test programs" ON)
option(SIMPLEX_BUILD_EXAMPLES "Build example programs" ON)
option(SIMPLEX_ENABLE_SIMD "Enable SIMD optimizations" OFF)
option(SIMPLEX_ENABLE_PROFILING "Enable performance profiling" OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SIMPLEX_SOURCES
    src/simplex_noise.c
)

set(SIMPLEX_HEADERS
    include/simplex_noise.h
)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Compile definitions
set(SIMPLEX_DEFINITIONS)
if(SIMPLEX_ENABLE_SIMD)
    list(APPEND SIMPLEX_DEFINITIONS SIMPLEX_ENABLE_SIMD)
    # Check for SIMD support
    include(CheckCCompilerFlag)
    check_c_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
    check_c_compiler_flag("-msse4.1" COMPILER_SUPPORTS_SSE41)
    check_c_compiler_flag("-mneon" COMPILER_SUPPORTS_NEON)
    
    if(COMPILER_SUPPORTS_AVX2)
        list(APPEND SIMPLEX_DEFINITIONS SIMPLEX_HAVE_AVX2)
        add_compile_options(-mavx2)
    elseif(COMPILER_SUPPORTS_SSE41)
        list(APPEND SIMPLEX_DEFINITIONS SIMPLEX_HAVE_SSE41)
        add_compile_options(-msse4.1)
    elseif(COMPILER_SUPPORTS_NEON)
        list(APPEND SIMPLEX_DEFINITIONS SIMPLEX_HAVE_NEON)
        add_compile_options(-mneon)
    endif()
endif()

if(SIMPLEX_ENABLE_PROFILING)
    list(APPEND SIMPLEX_DEFINITIONS SIMPLEX_ENABLE_PROFILING)
endif()

# Build static library
if(SIMPLEX_BUILD_STATIC)
    add_library(simplex_noise_static STATIC ${SIMPLEX_SOURCES})
    target_compile_definitions(simplex_noise_static PRIVATE ${SIMPLEX_DEFINITIONS})
    target_include_directories(simplex_noise_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    set_target_properties(simplex_noise_static PROPERTIES
        OUTPUT_NAME simplex_noise
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endif()

# Build shared library
if(SIMPLEX_BUILD_SHARED)
    add_library(simplex_noise_shared SHARED ${SIMPLEX_SOURCES})
    target_compile_definitions(simplex_noise_shared PRIVATE ${SIMPLEX_DEFINITIONS})
    target_include_directories(simplex_noise_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    set_target_properties(simplex_noise_shared PROPERTIES
        OUTPUT_NAME simplex_noise
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    
    # Link math library on Unix systems
    if(UNIX)
        target_link_libraries(simplex_noise_shared m)
    endif()
endif()

# Create alias for the primary library
if(SIMPLEX_BUILD_SHARED)
    add_library(simplex_noise ALIAS simplex_noise_shared)
elseif(SIMPLEX_BUILD_STATIC)
    add_library(simplex_noise ALIAS simplex_noise_static)
endif()

# Build test programs
if(SIMPLEX_BUILD_TESTS)
    enable_testing()
    
    # Basic functionality test
    add_executable(test_basic tests/test_basic.c)
    target_link_libraries(test_basic simplex_noise m)
    
    # Configuration system test
    add_executable(test_config tests/test_config.c)
    target_link_libraries(test_config simplex_noise m)
    
    # Performance benchmark test
    add_executable(test_performance tests/test_performance.c)
    target_link_libraries(test_performance simplex_noise m)
    
    # Add tests to CTest
    add_test(NAME basic_functionality COMMAND test_basic)
    add_test(NAME configuration_system COMMAND test_config)
    add_test(NAME performance_benchmark COMMAND test_performance)
endif()

# Build example programs
if(SIMPLEX_BUILD_EXAMPLES)
    # 2D noise example
    add_executable(example_2d examples/example_2d.c)
    target_link_libraries(example_2d simplex_noise m)
    
    # 3D noise example
    add_executable(example_3d examples/example_3d.c)
    target_link_libraries(example_3d simplex_noise m)
    
    # Configuration example
    add_executable(example_config examples/example_config.c)
    target_link_libraries(example_config simplex_noise m)
    
    # Fractal noise example
    add_executable(example_fractal examples/example_fractal.c)
    target_link_libraries(example_fractal simplex_noise m)
endif()

# Installation
if(SIMPLEX_BUILD_STATIC)
    install(TARGETS simplex_noise_static DESTINATION lib)
endif()

if(SIMPLEX_BUILD_SHARED)
    install(TARGETS simplex_noise_shared DESTINATION lib)
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/simplex_noise.h
    DESTINATION include
)

# Install pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/simplex_noise.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/simplex_noise.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/simplex_noise.pc
    DESTINATION lib/pkgconfig
)

# CPack configuration for packaging
include(CPack)
set(CPACK_PACKAGE_NAME "simplex-noise")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Adrian Paredez")
set(CPACK_PACKAGE_CONTACT "adrian@example.com")

# Platform-specific packaging
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP;DragNDrop")
else()
    set(CPACK_GENERATOR "ZIP;TGZ;DEB;RPM")
endif()

# Documentation
option(SIMPLEX_BUILD_DOCS "Build documentation" OFF)
if(SIMPLEX_BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                      ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(docs
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
    endif()
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "Simplex Noise Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C standard: ${CMAKE_C_STANDARD}")
message(STATUS "  Build shared library: ${SIMPLEX_BUILD_SHARED}")
message(STATUS "  Build static library: ${SIMPLEX_BUILD_STATIC}")
message(STATUS "  Build tests: ${SIMPLEX_BUILD_TESTS}")
message(STATUS "  Build examples: ${SIMPLEX_BUILD_EXAMPLES}")
message(STATUS "  Enable SIMD: ${SIMPLEX_ENABLE_SIMD}")
message(STATUS "  Enable profiling: ${SIMPLEX_ENABLE_PROFILING}")
message(STATUS "  Build documentation: ${SIMPLEX_BUILD_DOCS}")
message(STATUS "")
