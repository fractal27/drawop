# Makefile to build RAYLIB into $(LIB)/libraylib$(_lib_ext)

EXTERNAL ?= .
LIB := ../lib
RAYLIB_RELEASE_PATH := ../lib


$(LIB)/libraylib$(_lib_ext):
	 cd $(EXTERNAL)/raylib/src/ \
	&& RAYLIB_RELEASE_PATH=../../../lib $(MAKE)

ifdef _makefile_already_included
include Makefile.mk
endif
