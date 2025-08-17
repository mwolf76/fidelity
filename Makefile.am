AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = fidelity
fidelity_SOURCES = main.cpp \
                   config.cpp \
                   connectivity_checker.cpp \
                   logger.cpp \
                   monitor.cpp \
                   network_service.cpp \
                   network_writer.cpp \
                   yaml_parser.cpp

# Set C++ standard and include yaml-cpp
AM_CXXFLAGS = -std=c++20 -Wall -Wextra $(YAML_CPP_CFLAGS)
fidelity_LDADD = $(YAML_CPP_LIBS)

# Clean up build artifacts
CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 compile config.guess config.sub \
                        configure depcomp install-sh missing