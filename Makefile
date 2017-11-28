.PHONY: all clean run
.SUFFIXES:

PROGRAM := gltut
PREFIX := $(HOME)/Desktop/gltut
CFLAGS := $(CFLAGS) -g -I$(PREFIX)/include -std=gnu99
CXXFLAGS := $(CXXFLAGS) -g -I$(PREFIX)/include -I./fmt -std=gnu++14
LDFLAGS := $(LDFLAGS) -g -L$(PREFIX)/lib \
	-framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreVideo \
	-lglfw3

CXX_SRCS := $(wildcard *.cpp)
CC_SRCS := $(wildcard fmt/fmt/*.cc)
C_SRCS := $(wildcard *.c)
CXX_OBJS := $(patsubst %.cpp,obj/%.cpp.o,$(CXX_SRCS))
CC_OBJS := $(patsubst fmt/fmt/%.cc,obj/%.cc.o,$(CC_SRCS))
C_OBJS := $(patsubst %.c,obj/%.c.o,$(C_SRCS))
SRCS := $(CXX_SRCS) $(C_SRCS)
OBJS := $(CXX_OBJS) $(CC_OBJS) $(C_OBJS)

all: obj obj/Depends.mk obj/$(PROGRAM)

obj:
	mkdir -p obj

-include obj/Depends.mk

obj/Depends.mk: $(SRCS)
	CXXFLAGS="$(CXXFLAGS)" CFLAGS="$(CFLAGS)" ./makedepend.sh $(SRCS)

obj/$(PROGRAM): $(OBJS)
	c++ -o $@ $(LDFLAGS) $^

obj/%.cpp.o:
	c++ -c -o $@ $(CXXFLAGS) $*.cpp

obj/%.cc.o:
	c++ -c -o $@ $(CXXFLAGS) fmt/fmt/$*.cc

obj/%.c.o:
	cc -c -o $@ $(CFLAGS) $*.c

run: all
	obj/gltut

clean:
	rm -rf obj

