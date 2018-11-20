#pragma once

#if defined(_WIN32) && !defined(DEBUG_NO_EXPORTS)
#if defined(vkworld_EXPORTS)
#define VKWORLD_EXPORT __declspec(dllexport)
#else
#define VKWORLD_EXPORT __declspec(dllimport)
#endif /* vkworld_EXPORTS */
#else  /* defined (_WIN32) */
#define VKWORLD_EXPORT
#endif

#define _USE_MATH_DEFINES

// Warnings MSVC
#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif
