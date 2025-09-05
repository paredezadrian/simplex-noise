AC_PREREQ([2.69])
AC_INIT([simplex-noise], [2.0.0], [adrian@example.com])
AC_CONFIG_SRCDIR([src/simplex_noise.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])

# Package information
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_DISABLE_STATIC

# Check for C compiler
AC_PROG_CC
AC_PROG_CC_C99

# Check for required programs
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LN_S

# Check for libraries
AC_CHECK_LIB([m], [sin], [LIBS="$LIBS -lm"])

# Check for header files
AC_CHECK_HEADERS([stdint.h stdlib.h string.h math.h time.h])

# Compiler flags
if test "x$GCC" = "xyes"; then
    CFLAGS="$CFLAGS -Wall -Wextra -Wpedantic -Werror"
fi

# Check for SIMD support
AC_ARG_ENABLE([simd],
    [AS_HELP_STRING([--enable-simd], [Enable SIMD optimizations])],
    [enable_simd=$enableval],
    [enable_simd=no])

if test "x$enable_simd" = "xyes"; then
    AC_DEFINE([SIMPLEX_ENABLE_SIMD], [1], [Enable SIMD optimizations])
    
    # Check for specific SIMD instruction sets
    AC_CHECK_C_COMPILER_FLAG([-mavx2], [HAVE_AVX2=yes], [HAVE_AVX2=no])
    AC_CHECK_C_COMPILER_FLAG([-msse4.1], [HAVE_SSE41=yes], [HAVE_SSE41=no])
    AC_CHECK_C_COMPILER_FLAG([-mneon], [HAVE_NEON=yes], [HAVE_NEON=no])
    
    if test "x$HAVE_AVX2" = "xyes"; then
        AC_DEFINE([SIMPLEX_HAVE_AVX2], [1], [Have AVX2 support])
        CFLAGS="$CFLAGS -mavx2"
    elif test "x$HAVE_SSE41" = "xyes"; then
        AC_DEFINE([SIMPLEX_HAVE_SSE41], [1], [Have SSE4.1 support])
        CFLAGS="$CFLAGS -msse4.1"
    elif test "x$HAVE_NEON" = "xyes"; then
        AC_DEFINE([SIMPLEX_HAVE_NEON], [1], [Have NEON support])
        CFLAGS="$CFLAGS -mneon"
    fi
fi

# Check for profiling support
AC_ARG_ENABLE([profiling],
    [AS_HELP_STRING([--enable-profiling], [Enable performance profiling])],
    [enable_profiling=$enableval],
    [enable_profiling=no])

if test "x$enable_profiling" = "xyes"; then
    AC_DEFINE([SIMPLEX_ENABLE_PROFILING], [1], [Enable performance profiling])
fi

# Check for tests
AC_ARG_ENABLE([tests],
    [AS_HELP_STRING([--enable-tests], [Build test programs])],
    [enable_tests=$enableval],
    [enable_tests=yes])

# Check for examples
AC_ARG_ENABLE([examples],
    [AS_HELP_STRING([--enable-examples], [Build example programs])],
    [enable_examples=$enableval],
    [enable_examples=yes])

# Check for documentation
AC_ARG_ENABLE([docs],
    [AS_HELP_STRING([--enable-docs], [Build documentation])],
    [enable_docs=$enableval],
    [enable_docs=no])

if test "x$enable_docs" = "xyes"; then
    AC_CHECK_PROG([DOXYGEN], [doxygen], [doxygen], [no])
    if test "x$DOXYGEN" = "xno"; then
        AC_MSG_WARN([doxygen not found - documentation will not be built])
    fi
fi

# Output files
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    tests/Makefile
    examples/Makefile
    simplex_noise.pc
])

AC_OUTPUT
