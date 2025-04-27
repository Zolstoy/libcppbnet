#ifndef BNETCPP_CONFIG
#define BNETCPP_CONFIG

#if defined(_WIN32)
# define BNETCPP_SYSTEM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
# include "TargetConditionals.h"
# if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#  define BNETCPP_SYSTEM_IOS
# elif TARGET_OS_MAC
#  define BNETCPP_SYSTEM_MACOS
# else
#  error This Apple operating system is not supported by BNETCPP library
# endif
#elif defined(__unix__)
# if defined(__ANDROID__)
#  define BNETCPP_SYSTEM_ANDROID
# elif defined(__linux__)
#  define BNETCPP_SYSTEM_LINUX
# elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#  define BNETCPP_SYSTEM_FREEBSD
# else
#  error This UNIX operating system is not supported by BNETCPP library
# endif
#else
# error This operating system is not supported by BNETCPP library
#endif

#if !defined(BNETCPP_STATIC)
# if defined(BNETCPP_SYSTEM_WINDOWS)
#  define BNETCPP_SYMBOL_EXPORT __declspec(dllexport)
#  define BNETCPP_SYMBOL_IMPORT __declspec(dllimport)
#  ifdef _MSC_VER
#   pragma warning(disable : 4251)
#  endif
# else   // Linux, FreeBSD, Mac OS X
#  if __GNUC__ >= 4
#   define BNETCPP_SYMBOL_EXPORT __attribute__((__visibility__("default")))
#   define BNETCPP_SYMBOL_IMPORT __attribute__((__visibility__("default")))
#  else
#   define BNETCPP_SYMBOL_EXPORT
#   define BNETCPP_SYMBOL_IMPORT
#  endif
# endif
#else
# define BNETCPP_SYMBOL_EXPORT
# define BNETCPP_SYMBOL_IMPORT
#endif

#if defined(BNETCPP_BUILDING_THE_LIB)
# define BNETCPP_API BNETCPP_SYMBOL_EXPORT
#else
# define BNETCPP_API BNETCPP_SYMBOL_IMPORT
#endif

#endif