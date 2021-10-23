#ifndef POTATO_CORE_PLATFORM_HPP
#define POTATO_CORE_PLATFORM_HPP

#if ( defined(_WIN32) || defined(_WIN64) )
#    define WINDOWS
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#    define LINUX
#endif

#ifdef WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <Windows.h>
#endif

#endif
