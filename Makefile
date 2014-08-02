EXE = polarity.html

INCLUDES = $(wildcard graphics/*.hpp audio/*.hpp physics/*.hpp main/*.hpp)
SRCS = $(wildcard graphics/*.cpp physics/*.cpp main/*.cpp audio/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
#tinyxml2/tinyxml2.o libtmx-parser/src/tmxparser.o
# audio/*.cpp
CC = emcc
CXX = em++

CFLAGS = -pthread -g -O2 -I $(CURDIR)
#-Ilibtmx-parser/src -Itinyxml2
CXXFLAGS = -std=gnu++11 $(CFLAGS)
LDFLAGS = -s STB_IMAGE=1 --preload-file assets

$(EXE): $(OBJS) $(wildcard assets/*)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(OBJS): $(wildcard *.hpp) Makefile

