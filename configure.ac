AC_INIT([fidelity], [1.0], [bug-report@address])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_LANG([C++])

# Set C++20 standard
CXXFLAGS="$CXXFLAGS -std=c++20"

# Check for pkg-config
PKG_PROG_PKG_CONFIG

# Check for yaml-cpp
PKG_CHECK_MODULES([YAML_CPP], [yaml-cpp], [], [
    AC_MSG_ERROR([yaml-cpp library is required. Please install libyaml-cpp-dev])
])

# Check for required headers
AC_CHECK_HEADERS([iostream string thread chrono cstdlib fstream sstream unistd.h sys/types.h sys/wait.h])

# Check for pthread support
AC_SEARCH_LIBS([pthread_create], [pthread], [], [
    AC_MSG_ERROR([pthread library is required])
])

AC_CONFIG_FILES([
 Makefile
])
AC_OUTPUT