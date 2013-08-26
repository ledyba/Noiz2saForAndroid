#****************************************************************************
#
# Makefile for TinyXml test.
# Lee Thomason
# www.grinninglizard.com
#
# This is a GNU make (gmake) makefile
#****************************************************************************

# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := NO

# PROFILE can be set to YES to include profiling info, or NO otherwise
PROFILE        := NO

# TINYXML_USE_STL can be used to turn on STL support. NO, then STL
# will not be used. YES will include the STL files.
TINYXML_USE_STL := NO

#****************************************************************************

NACL_TOOLCHAIN_ROOT=$(NACL_SDK_ROOT)/toolchain/linux_x86_newlib
NACL_TOOLCHAIN_ROOT32=$(NACL_TOOLCHAIN_ROOT)/i686-nacl/usr
NACL_TOOLCHAIN_ROOT64=$(NACL_TOOLCHAIN_ROOT)/x86_64-nacl/usr
NACL_GCC_PREFIX=$(NACL_TOOLCHAIN_ROOT)/bin/x86_64-nacl-

CC     = $(NACL_GCC_PREFIX)gcc
LD     = $(NACL_GCC_PREFIX)g++
CXX    = $(NACL_GCC_PREFIX)g++
AR     = $(NACL_GCC_PREFIX)ar
RANLIB = $(NACL_GCC_PREFIX)ranlib

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O3

LIBS		 :=

DEBUG_CXXFLAGS   := ${DEBUG_CFLAGS} 
RELEASE_CXXFLAGS := ${RELEASE_CFLAGS}

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  :=

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif

ifeq (YES, ${PROFILE})
   CFLAGS   := ${CFLAGS} -pg -O3
   CXXFLAGS := ${CXXFLAGS} -pg -O3
   LDFLAGS  := ${LDFLAGS} -pg
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************

ifeq (YES, ${TINYXML_USE_STL})
  DEFS := -DTIXML_USE_STL
else
  DEFS :=
endif

#****************************************************************************
# Include paths
#****************************************************************************

#INCS := -I/usr/include/g++-2 -I/usr/local/include
INCS :=


#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS} ${DEFS}

#****************************************************************************
# Source files
#****************************************************************************

SRCS := tinyxml.cpp tinyxmlparser.cpp xmltest.cpp tinyxmlerror.cpp tinystr.cpp

# Add on the sources for libraries
SRCS := ${SRCS}

OBJS32 := $(addsuffix .o32,$(basename ${SRCS}))
OBJS64 := $(addsuffix .o64,$(basename ${SRCS}))
#****************************************************************************
# Targets of the build
#****************************************************************************

all: $(OBJS32) $(OBJS64)


#****************************************************************************
# Output
#****************************************************************************

${OUTPUT32}: ${OBJS32}
	${LD} -o $@ ${LDFLAGS} ${OBJS32} ${LIBS} ${EXTRA_LIBS}
${OUTPUT64}: ${OBJS64}
	${LD} -o $@ ${LDFLAGS} ${OBJS64} ${LIBS} ${EXTRA_LIBS}

#****************************************************************************
# common rules
#****************************************************************************

# Rules for compiling source files to object files
%.o32 : %.cpp
	${CXX} -c -m32 ${CXXFLAGS} ${INCS} $< -o $@

%.o32 : %.c
	${CC} -c -m32 ${CFLAGS} ${INCS} $< -o $@

%.o64 : %.cpp
	${CXX} -c -m64 ${CXXFLAGS} ${INCS} $< -o $@

%.o64 : %.c
	${CC} -c -m64 ${CFLAGS} ${INCS} $< -o $@

clean:
	-rm -f core ${OBJS} ${OUTPUT}

depend:
	#makedepend ${INCS} ${SRCS}

tinyxml.o32: tinyxml.h tinystr.h
tinyxmlparser.o32: tinyxml.h tinystr.h
xmltest.o32: tinyxml.h tinystr.h
tinyxmlerror.o32: tinyxml.h tinystr.h

tinyxml.o64: tinyxml.h tinystr.h
tinyxmlparser.o64: tinyxml.h tinystr.h
xmltest.o64: tinyxml.h tinystr.h
tinyxmlerror.o64: tinyxml.h tinystr.h
