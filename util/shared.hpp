#ifndef _POLARITY_UTIL_SHARED_HPP_
#define _POLARITY_UTIL_SHARED_HPP_

#define POLARITY_PLATFORM_WINDOWS 0
#define POLARITY_PLATFORM_LINUX   1
#define POLARITY_PLATFORM_MAC     2


#if defined(__WIN32__) || defined(_WIN32)
#  define POLARITY_PLATFORM POLARITY_PLATFORM_WINDOWS
#elif defined(__APPLE_CC__) || defined(__APPLE__)
#  define POLARITY_PLATFORM POLARITY_PLATFORM_MAC
#  ifndef __MACOSX__
#    define __MACOSX__
#  endif
#else
#  define POLARITY_PLATFORM POLARITY_PLATFORM_LINUX
#endif
#ifdef POLARITY_SHARED
# if POLARITY_PLATFORM == POLARITY_PLATFORM_WINDOW
#  ifdef POLARITYGFX_BUILD
#   define POLARITYGFX_EXPORT __declspec(dllexport)
#   define POLARITYGFX_FUNCTION_EXPORT __declspec(dllexport)
#  else
#   define POLARITYGFX_EXPORT __declspec(dllimport)
#   define POLARITYGFX_FUNCTION_EXPORT __declspec(dllimport)
#  endif
#  ifdef POLARITYENGINE_BUILD
#   define POLARITYENGINE_EXPORT __declspec(dllexport)
#   define POLARITYENGINE_FUNCTION_EXPORT __declspec(dllexport)
#  else
#   define POLARITYENGINE_EXPORT __declspec(dllimport)
#   define POLARITYENGINE_FUNCTION_EXPORT __declspec(dllimport)
#  endif
# else
#   define POLARITYGFX_EXPORT __attribute__ ((visibility("default")))
#   define POLARITYGFX_FUNCTION_EXPORT __attribute__ ((visibility("default")))
#   define POLARITYENGINE_EXPORT __attribute__ ((visibility("default")))
#   define POLARITYENGINE_FUNCTION_EXPORT __attribute__ ((visibility("default")))
# endif
#else
#define POLARITYGFX_EXPORT
#define POLARITYGFX_FUNCTION_EXPORT
#define POLARITYENGINE_EXPORT
#define POLARITYENGINE_FUNCTION_EXPORT
#endif

#endif
