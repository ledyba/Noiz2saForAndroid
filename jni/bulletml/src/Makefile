CC = $(NACL_SDK_ROOT)/toolchain/linux_x86_newlib/bin/x86_64-nacl-g++
INCLUDES = -I.
LIBS = tinyxml/tinyxml.o tinyxml/tinyxmlparser.o tinyxml/tinyxmlerror.o
CXXFLAGS = -O3 -W -Wall -ansi -pedantic
CFLAGS = -O3

CPPFLAGS32 = -m32
CPPFLAGS64 = -m64
OBJS32 = bulletmlparser-tinyxml.o32 bulletmlparser.o32 bulletmltree.o32 calc.o32 formula-variables.o32 bulletmlrunner.o32 bulletmlrunnerimpl.o32
OBJS64 = bulletmlparser-tinyxml.o64 bulletmlparser.o64 bulletmltree.o64 calc.o64 formula-variables.o64 bulletmlrunner.o64 bulletmlrunnerimpl.o64

all: libbulletml32.a libbulletml64.a

libbulletml32.a: $(OBJS32)
	$(MAKE) -C tinyxml
	$(AR) -r libbulletml32.a *.o32 tinyxml/tiny*.o32

libbulletml64.a: $(OBJS64)
	$(MAKE) -C tinyxml
	$(AR) -r libbulletml64.a *.o64 tinyxml/tiny*.o64

clean:
	rm -f *.o32 *.o64 *.a
	$(MAKE) -C tinyxml clean

calc.cpp: calc.yy
	bison -y   calc.yy && mv y.tab.c calc.cpp

$(OBJS32): %.o32: %.cpp
	$(CC) -c $(CPPFLAGS32) $(CXXFLAGS) $(INCLUDES) $< -o $@

$(OBJS64): %.o64: %.cpp
	$(CC) -c $(CPPFLAGS64) $(CXXFLAGS) $(INCLUDES) $< -o $@

doxy:
	doxygen

distdir:

