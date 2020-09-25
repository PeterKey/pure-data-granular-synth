
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name
lib.name = jp_grainmaker

# input source file (class name == source file basename)
class.sources = grainmaker~.c
grainmaker~.class.sources = grain_scheduler.c
grainmaker~.grain_scheduler.class.sources = grain.c

# all extra files to be included in binary distribution of the library
datafiles =

# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'
include Makefile.pdlibbuilder