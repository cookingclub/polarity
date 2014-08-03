EXE = polarity.html

INCLUDES =  $(wildcard graphics/*.hpp audio/*.hpp physics/*.hpp main/*.hpp)
SRCS = $(wildcard graphics/*.cpp physics/*.cpp main/*.cpp world/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) libs/libtmx-parser/libs/tinyxml2/tinyxml2.o libs/libtmx-parser/src/tmxparser.o
# audio/*.cpp
CC = emcc
CXX = em++
#-O2 
CFLAGS = -pthread -g -Wno-warn-absolute-paths -I $(CURDIR) -I$(CURDIR)/libs/box2d/Box2D/ -I$(CURDIR)/libs/libtmx-parser/libs/tinyxml2 -I$(CURDIR)/libs/libtmx-parser/libs/tinyxml2
CXXFLAGS = -std=gnu++11 $(CFLAGS)
LDFLAGS = -s STB_IMAGE=1 --preload-file assets static_libs/libBox2D.a

$(EXE): $(OBJS) $(wildcard assets/*)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(OBJS): $(wildcard *.hpp) Makefile

clean:
	rm $(OBJS)
	rm polarity.*

