
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name
lib.name = grainmaker~

make-lib-executeable = yes

# input source file (class name == source file basename)
class.sources = grainmaker~.c
grainmaker~.class.sources = grain_scheduler.c
grainmaker~.class.sources += grain.c

lib.setup.sources = $(lib.name).c

# all extra files to be included in binary distribution of the library
datafiles = \
grainmaker~-help.pd \
sing.wav

# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'
include Makefile.pdlibbuilder

buildcheck: all
		test -e grainmaker~.$(extension)
installcheck: install
		test -e $(installpath)/grainmaker~.$(extension)
		
DESTDIR = /build
installpath :=$(DESTDIR)/$(lib.name)
