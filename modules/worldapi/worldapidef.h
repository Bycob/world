#pragma once

#if defined (_WIN32) && !defined(DEBUG_NO_EXPORTS)
#if defined(worldapi_EXPORTS)
#define  WORLDAPI_EXPORT __declspec(dllexport)
#else
#define  WORLDAPI_EXPORT __declspec(dllimport)
#endif /* WorldAPI_EXPORTS */
#else /* defined (_WIN32) */
#define WORLDAPI_EXPORT
#endif

//#define ARMA_NO_DEBUG

// Warnings MSVC
#ifdef _MSC_VER
#pragma warning(disable:4251)
#endif
