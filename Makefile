EXE = polarity.html

INCLUDES = $(wildcard graphics/*.hpp audio/*.hpp physics/*.hpp main/*.hpp)
SRCS = $(wildcard graphics/*.cpp physics/*.cpp main/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) libs/libtmx-parser/libs/tinyxml2/tinyxml2.o libs/libtmx-parser/src/tmxparser.o
# audio/*.cpp
CC = emcc
CXX = em++

CFLAGS = -pthread -g -O2 -Wno-warn-absolute-paths -I $(CURDIR) -I$(CURDIR)/libs/libtmx-parser/libs/tinyxml2 -I$(CURDIR)/libs/libtmx-parser/libs/tinyxml2
CXXFLAGS = -std=gnu++11 $(CFLAGS)
LDFLAGS = -s STB_IMAGE=1 --preload-file assets

$(EXE): $(OBJS) $(wildcard assets/*)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(OBJS): $(wildcard *.hpp) Makefile

