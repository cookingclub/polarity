INCLUDES =  $(wildcard graphics/*.hpp audio/*.hpp physics/*.hpp main/*.hpp)
ARSRCS = $(wildcard libs/box2d/Box2D/Box2D/Collision/Shapes/*.cpp libs/box2d/Box2D/Box2D/Dynamics/*.cpp libs/box2d/Box2D/Box2D/Common/*.cpp  libs/box2d/Box2D/Box2D/Rope/*.cpp libs/box2d/Box2D/Box2D/Dynamics/Contacts/*.cpp libs/box2d/Box2D/Box2D/Dynamics/Joints/*.cpp libs/box2d/Box2D/Box2D/Collision/*.cpp libs/libpng/*.c libs/zlib/[^g]*.c)

SRCS = $(wildcard graphics/*.cpp physics/*.cpp main/*.cpp world/*.cpp libs/libtmxparser/src/*.cpp )
# audio/*.cpp
ifdef NATIVE
O_EXT = .native.o
EXE_EXT =
AR_EXT = .native.a

CC = gcc
CXX = g++
AR = ar
LDFLAGS = -lSDL -lSDL_mixer -lSDL_gfx -lSDL_image -lSDL_ttf $(EXTERNALS)
else
O_EXT = .em.o
EXE_EXT = .html
AR_EXT = .em.a
CC = emcc
CXX = em++
AR = emar
LDFLAGS = -s TOTAL_MEMORY=134217728 $(EXTERNALS)
# --preload-file assets
endif

EXE = polarity$(EXE_EXT)
EXTERNALS = libs/libext$(AR_EXT)

AROBJS = $(patsubst %.c, %$(O_EXT), $(patsubst %.cpp, %$(O_EXT), $(ARSRCS))) libs/tinyxml2/tinyxml2$(O_EXT)
OBJS = $(patsubst %.cpp, %$(O_EXT), $(SRCS))

CFLAGS = -fno-exceptions -pthread -g -Wall -Wextra -Wno-unused-parameter -Wno-warn-absolute-paths -I $(CURDIR) -I$(CURDIR)/libs/box2d/Box2D/ -I$(CURDIR)/libs/tinyxml2 -I$(CURDIR)/libs/tinyxml2 -I$(CURDIR)/libs/libtmxparser/src -I$(CURDIR)/libs/zlib -I$(CURDIR)/libs/libpng
CXXFLAGS = -std=gnu++11 $(CFLAGS)

%$(O_EXT): %.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

%$(O_EXT): %.c
	$(CC) $(CFLAGS) $< -c -o $@

$(EXE): $(OBJS) $(wildcard assets/*) $(EXTERNALS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(EXTERNALS): $(AROBJS)
	$(AR) cru $@ $(AROBJS)

$(OBJS): $(INCLUDES) Makefile

clean:
	rm -f $(OBJS) $(AROBJS)
	rm -f $(EXTERNALS)
	rm -f $(EXE)

