CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)
project(polarity)
SET(CMAKE_VERBOSE_MAKEFILE 1 CACHE BOOL "Enable verbose make output")

SET(POLARITY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/libs/box2d/Box2D ${CMAKE_CURRENT_SOURCE_DIR}/libs/box2d/Box2D ${CMAKE_CURRENT_SOURCE_DIR}/libs/libpng ${CMAKE_CURRENT_SOURCE_DIR}/libs/libtmxparser/src ${CMAKE_CURRENT_SOURCE_DIR}/libs/tinyxml2)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")


IF(NOT WIN32)
ADD_DEFINITIONS("-Wall -Wno-unused-function -Wno-unused-variable -fvisibility=hidden -DPOLARITY_SHARED")
ENDIF()

IF(NOT SDL_VERSION)
  SET(SDL_VERSION "SDL" CACHE STRING
      "Which version of sdl to use" FORCE
      )
ENDIF()

IF(SDL_VERSION STREQUAL "SDL2")
ADD_DEFINITIONS("-DUSE_SDL2")
SET(POLARITY_INCLUDE_DIRS ${POLARITY_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/libs/libsdl/include)
SET(POLARITY_LINK_LIBRARIES ${POLARITY_LINK_LIBRARIES} -lSDL2_mixer -lGL -lSDL2_ttf -lSDL2 -lSDL2main -lrt -lfreetype)
ELSE()
SET(POLARITY_LINK_LIBRARIES ${POLARITY_LINK_LIBRARIES} -lSDL_mixer -lGL -lSDL_ttf -lSDL -lSDLmain -lSDL_gfx -lrt -lfreetype)
ENDIF()
INCLUDE_DIRECTORIES(${POLARITY_INCLUDE_DIRS})

IF(NOT CMAKE_BUILD_TYPE)
#the below lines work around a bug when cmake first is configured without <CONFIG>
  SET(FORCE_USE_DEBUG_LOCATION "TRUE")
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Build types include: Default Debug Release None RelWithDebInfo." FORCE
      )
ENDIF()


SET(POLARITYGFX_EXT_SOURCES #            libs/tinyxml2/tinyxml2.cpp
            libs/libpng/example.c
            libs/libpng/png.c
            libs/libpng/pngerror.c
            libs/libpng/pngget.c
            libs/libpng/pngmem.c
            libs/libpng/pngpread.c
            libs/libpng/pngread.c
            libs/libpng/pngrio.c
            libs/libpng/pngrtran.c
            libs/libpng/pngrutil.c
            libs/libpng/pngset.c
            libs/libpng/pngtest.c
            libs/libpng/pngtrans.c
            libs/libpng/pngwio.c
            libs/libpng/pngwrite.c
            libs/libpng/pngwtran.c
            libs/libpng/pngwutil.c
            libs/zlib/adler32.c
            libs/zlib/compress.c
            libs/zlib/crc32.c
            libs/zlib/deflate.c
            libs/zlib/infback.c
            libs/zlib/inffast.c
            libs/zlib/inflate.c
            libs/zlib/inftrees.c
            libs/zlib/trees.c
            libs/zlib/uncompr.c
            libs/zlib/zutil.c)

SET(POLARITYENGINE_EXT_SOURCES
             libs/tinyxml2/tinyxml2.cpp             
            libs/box2d/Box2D/Box2D/Collision/b2BroadPhase.cpp
            libs/box2d/Box2D/Box2D/Collision/b2CollideCircle.cpp
            libs/box2d/Box2D/Box2D/Collision/b2CollideEdge.cpp
            libs/box2d/Box2D/Box2D/Collision/b2CollidePolygon.cpp
            libs/box2d/Box2D/Box2D/Collision/b2Collision.cpp
            libs/box2d/Box2D/Box2D/Collision/b2Distance.cpp
            libs/box2d/Box2D/Box2D/Collision/b2DynamicTree.cpp
            libs/box2d/Box2D/Box2D/Collision/b2TimeOfImpact.cpp
            libs/box2d/Box2D/Box2D/Collision/Shapes/b2ChainShape.cpp
            libs/box2d/Box2D/Box2D/Collision/Shapes/b2CircleShape.cpp
            libs/box2d/Box2D/Box2D/Collision/Shapes/b2EdgeShape.cpp
            libs/box2d/Box2D/Box2D/Collision/Shapes/b2PolygonShape.cpp
            libs/box2d/Box2D/Box2D/Common/b2BlockAllocator.cpp
            libs/box2d/Box2D/Box2D/Common/b2Draw.cpp
            libs/box2d/Box2D/Box2D/Common/b2Math.cpp
            libs/box2d/Box2D/Box2D/Common/b2Settings.cpp
            libs/box2d/Box2D/Box2D/Common/b2StackAllocator.cpp
            libs/box2d/Box2D/Box2D/Common/b2Timer.cpp
            libs/box2d/Box2D/Box2D/Dynamics/b2Body.cpp
            libs/box2d/Box2D/Box2D/Dynamics/b2ContactManager.cpp
            libs/box2d/Box2D/Box2D/Dynamics/b2Fixture.cpp
            libs/box2d/Box2D/Box2D/Dynamics/b2Island.cpp
            libs/box2d/Box2D/Box2D/Dynamics/b2WorldCallbacks.cpp
            libs/box2d/Box2D/Box2D/Dynamics/b2World.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Contacts/b2CircleContact.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Contacts/b2Contact.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2GearJoint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2Joint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2MotorJoint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2MouseJoint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2RopeJoint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2WeldJoint.cpp
            libs/box2d/Box2D/Box2D/Dynamics/Joints/b2WheelJoint.cpp
            libs/box2d/Box2D/Box2D/Rope/b2Rope.cpp )


add_library(polaritygraphics SHARED
            ${POLARITYGFX_EXT_SOURCES}
            graphics/animation.cpp
            graphics/font_renderer.cpp
            graphics/image.cpp
            graphics/init.cpp

            graphics/opengl_canvas.cpp
            graphics/sdl_canvas.cpp
            util/async_io_task.cpp
            graphics/image.hpp
            graphics/init.hpp
            graphics/font_renderer.hpp
            graphics/sdl_canvas.hpp
            graphics/opengl_canvas.hpp
            graphics/canvas.hpp
            graphics/display_list.hpp
            graphics/font_manager.hpp
            graphics/matrix4x4.hpp
            graphics/rect.hpp
            graphics/animation.hpp)

add_library(polarityengine SHARED
            ${POLARITYENGINE_EXT_SOURCES}
            physics/contact.cpp
            physics/game_object.cpp
            physics/jumping_behavior.cpp
            physics/player_behavior.cpp
            main/game.cpp
            world/door_behavior.cpp
            world/layer_collection.cpp
            world/sounds.cpp
            world/world.cpp
            libs/libtmxparser/src/tmxparser.cpp
            graphics/animation.hpp
            graphics/canvas.hpp
            graphics/display_list.hpp
            graphics/font_manager.hpp
            graphics/font_renderer.hpp
            graphics/image.hpp
            graphics/init.hpp
            graphics/matrix4x4.hpp
            graphics/opengl_canvas.hpp
            graphics/rect.hpp
            graphics/sdl_canvas.hpp
            physics/behavior.hpp
            physics/contact.hpp
            physics/game_object.hpp
            physics/jumping_behavior.hpp
            physics/magnetic_behavior.hpp
            physics/player_behavior.hpp
            physics/vector.hpp
            main/game.hpp
            world/door_behavior.hpp
            world/layer_collection.hpp
            world/player.hpp
            world/sounds.hpp
            world/trigger.hpp
            world/world.hpp
            util/async_io_task.hpp
            util/lru_map.hpp
            libs/libtmxparser/src/tmxparser.h)


add_executable(polarity
            main/main.cpp)
SET_TARGET_PROPERTIES(polaritygraphics PROPERTIES COMPILE_FLAGS "-DPOLARITYGFX_BUILD")
SET_TARGET_PROPERTIES(polarityengine PROPERTIES COMPILE_FLAGS "-DPOLARITYENGINE_BUILD")
TARGET_LINK_LIBRARIES(polarity LINK_PUBLIC polarityengine)
TARGET_LINK_LIBRARIES(polarity LINK_PUBLIC polaritygraphics)
TARGET_LINK_LIBRARIES(polarityengine LINK_PUBLIC polaritygraphics)

FOREACH(POLARITY_LINK_LIBRARY IN ITEMS ${POLARITY_LINK_LIBRARIES})

TARGET_LINK_LIBRARIES(polaritygraphics LINK_PUBLIC ${POLARITY_LINK_LIBRARY})
ENDFOREACH(POLARITY_LINK_LIBRARY)