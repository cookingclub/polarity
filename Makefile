EXE = polarity.html
EXTERNALS = libs/libext.a
INCLUDES =  $(wildcard graphics/*.hpp audio/*.hpp physics/*.hpp main/*.hpp)
ARSRCS = $(wildcard libs/box2d/Box2D/Box2D/Collision/Shapes/*.cpp libs/box2d/Box2D/Box2D/Dynamics/*.cpp libs/box2d/Box2D/Box2D/Common/*.cpp  libs/box2d/Box2D/Box2D/Rope/*.cpp libs/box2d/Box2D/Box2D/Dynamics/Contacts/*.cpp libs/box2d/Box2D/Box2D/Dynamics/Joints/*.cpp libs/box2d/Box2D/Box2D/Collision/*.cpp)
AROBJS = $(patsubst %.cpp, %.o, $(ARSRCS)) libs/tinyxml2/tinyxml2.o 
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

SRCS = $(wildcard graphics/*.cpp physics/*.cpp main/*.cpp world/*.cpp libs/libtmxparser/src/*.cpp )
# audio/*.cpp
ifdef NATIVE
CC = gcc
CXX = g++
AR = ar
LDFLAGS = -lSDL -lSDL_mixer -lSDL_gfx -lSDL_image -lSDL_ttf $(EXTERNALS)
else
CC = emcc
CXX = em++
AR = emar
LDFLAGS = -s STB_IMAGE=1 --preload-file assets -s TOTAL_MEMORY=134217728 $(EXTERNALS)
endif
#-O2 

CFLAGS = -fno-exceptions -pthread -g -Wall -Wextra -Wno-unused-parameter -Wno-warn-absolute-paths -I $(CURDIR) -I$(CURDIR)/libs/box2d/Box2D/ -I$(CURDIR)/libs/tinyxml2 -I$(CURDIR)/libs/tinyxml2 -I$(CURDIR)/libs/libtmxparser/src
CXXFLAGS = -std=gnu++11 $(CFLAGS)


$(EXE): $(OBJS) $(wildcard assets/*) $(EXTERNALS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(EXTERNALS): $(AROBJS)
	$(AR) cru $@ $(AROBJS)

$(OBJS): $(INCLUDES) Makefile

clean:
	rm -f $(OBJS) $(AROBJS)
	rm -f $(EXTERNALS)
	rm -f polarity.*

