EXE = polarity.html
EXTERNALS = libext.a
INCLUDES =  $(wildcard graphics/*.hpp audio/*.hpp physics/*.hpp main/*.hpp)
ARSRCS = $(wildcard libs/box2d/Box2D/Box2D/Collision/Shapes/*.cpp libs/box2d/Box2D/Box2D/Dynamics/*.cpp libs/box2d/Box2D/Box2D/Common/*.cpp  libs/box2d/Box2D/Box2D/Rope/*.cpp libs/box2d/Box2D/Box2D/Dynamics/Contacts/*.cpp libs/box2d/Box2D/Box2D/Dynamics/Joints/*.cpp libs/box2d/Box2D/Box2D/Collision/*.cpp)
AROBJS = $(patsubst %.cpp, %.o, $(ARSRCS)) libs/tinyxml2/tinyxml2.o 


SRCS = $(wildcard graphics/*.cpp physics/*.cpp main/*.cpp world/*.cpp libs/libtmxparser/src/*.cpp )
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
# audio/*.cpp
CC = emcc
CXX = em++
AR = emar
#-O2 

CFLAGS = -pthread -g -Wno-warn-absolute-paths -I $(CURDIR) -I$(CURDIR)/libs/box2d/Box2D/ -I$(CURDIR)/libs/tinyxml2 -I$(CURDIR)/libs/tinyxml2 -I$(CURDIR)/libs/libtmxparser/src
CXXFLAGS = -std=gnu++11 $(CFLAGS)
LDFLAGS = -s STB_IMAGE=1 --preload-file assets -s TOTAL_MEMORY=134217728 $(EXTERNALS)


$(EXE): $(OBJS) $(wildcard assets/*) $(EXTERNALS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(EXTERNALS): $(AROBJS)
	$(AR) cru $@ $(AROBJS)

$(OBJS): $(INCLUDES) Makefile

clean:
	rm -f $(OBJS)
	rm -f $(EXTERNALS)
	rm -f polarity.*

