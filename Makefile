# 'install' program location 
INSTALL ?= install

# Location where the package is installed by 'make install' 
prefix ?= /usr/local

# Destination root (/ is used if empty) 
DESTDIR ?= 

# Compiler flags to link shared library 
LINK_DLL_FLAGS ?=  -dynamiclib  

# C++ compiler 
CXX = g++

# Standard flags for C++ 
CXXFLAGS ?= 

# Standard preprocessor flags (common for CC and CXX) 
CPPFLAGS ?= 

# Standard linker flags 
LDFLAGS ?= 

UNAME := $(shell uname)

PLATFORM_LDFLAGS ?= 

CPPDEPS = -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD -MP
HELIX_CXXFLAGS = -Ijava -Ideps/libpropeller/include -I/System/Library/Frameworks/JavaVM.framework/Headers -g -D_THREAD_SAFE -pthread -fPIC \
	-DPIC $(CPPFLAGS) $(CXXFLAGS)
	HELIX_OBJECTS =  \
	obj/helix_helix.o 
	
all: obj
obj:
	@mkdir -p obj

### Targets: ###

all: libhelix

install: install_helix 

clean: 
	rm -f obj/*.o
	rm -f obj/*.d
	rm -f obj/libhelix.so
	-(cd deps && $(MAKE) clean)

libpropeller: 
	cd deps/libpropeller && make

libhelix: libpropeller $(HELIX_OBJECTS) 
	$(CXX) $(LINK_DLL_FLAGS) -fPIC -o obj/libhelix.jnilib $(HELIX_OBJECTS)  -pthread -Ldeps/libpropeller/obj $(LDFLAGS)  -lpropeller  $(PLATFORM_LDFLAGS)

install_helix: obj/libhelix.so
	$(INSTALL) -d $(DESTDIR)$(prefix)/lib
	$(INSTALL) -m 644 obj/libhelix.so $(DESTDIR)$(prefix)/lib

obj/helix_helix.o: src/helix.cpp
	$(CXX) -c -o $@ $(HELIX_CXXFLAGS) $(CPPDEPS) $<


.PHONY: all install uninstall clean install_helix


# Dependencies tracking:
-include obj/*.d

