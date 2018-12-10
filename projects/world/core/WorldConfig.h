#pragma once

#if defined(_WIN32) && !defined(DEBUG_NO_EXPORTS)
// TODO rename WORLDAPI_EXPORT -> WORLD_EXPORT
#if defined(world_EXPORTS)
#define WORLDAPI_EXPORT __declspec(dllexport)
#else
#define WORLDAPI_EXPORT __declspec(dllimport)
#endif /* World_EXPORTS */
#else  /* defined (_WIN32) */
#define WORLDAPI_EXPORT
#endif

#define _USE_MATH_DEFINES
//#define ARMA_NO_DEBUG

// Warnings MSVC
#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif
