# Original SDL-GL-basic Makefile template written by Hans de Ruiter
# Modified by Chris Brenton
#
# License:
# This source code can be used and/or modified without restrictions.
# It is provided as is and the author disclaims all warranties, expressed
# or implied, including, without limitation, the warranties of
# merchantability and of fitness for any purpose. The user must assume the
# entire risk of using the Software.
CC     = g++ -std=c++11

CCACHE_EXISTS := $(shell ccache -V)
ifdef CCACHE_EXISTS
  CC := ccache $(CC)
endif

#CC     = gfilt /cand:L -m32
CP     = cp
RM     = rm -rf
KILL   = killall -9
SHELL  = /bin/bash
MAKE   = make --no-print-directory
TIME   = time
JOBS   = $(shell nproc)

IFLAGS    =
LFLAGS    =
DEBUG     = -ggdb
OPTIMIZE  = -O3 -funroll-loops
#ERROR     = -Wconversion -Werror
CFLAGS    = $(OPTIMIZE) -Wall -c $(DEBUG) $(ERROR) $(IFLAGS)
LDFLAGS   = $(OPTIMIZE) $(DEBUG) $(ERROR) $(LFLAGS)

TARGET = extrude
ARGS   =

# Additional linker libraries
LIBS = $(LIBFLAGS)
FRAMEWORKS = -lGL -lGLU -lsfml-window -lsfml-graphics -lsfml-system

# Modify makeflags
MAKEFLAGS+=-j$(JOBS)

# -------------------------------------------------------------

# The source files
SRCS = $(wildcard ./*.cpp ./*/*.cpp)

OBJS = $(SRCS:.cpp=.o)

# Rules for building
all: $(TARGET)

$(TARGET): engine $(OBJS)
	$(CC) $(FRAMEWORKS) $(OBJS) $(LDFLAGS) -o $@

engine:
	$(MAKE) -C engine all

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: lib
lib:
	$(shell) ./lib.sh

run: $(TARGET)
	./$(TARGET) $(ARGS)

gdb:
	gdb --args ./$(TARGET) $(ARGS)

vg:   valgrind

valgrind:
	valgrind --tool=memcheck --leak-check=full ./$(TARGET) $(ARGS)

clean:
	$(RM) $(TARGET) $(OBJS)
	$(MAKE) -C engine clean

killall:
	$(KILL) $(TARGET)

full:
	$(MAKE) clean
	$(MAKE) run