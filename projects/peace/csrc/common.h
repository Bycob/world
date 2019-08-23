
#ifndef PEACE_COMMON_H
#define PEACE_COMMON_H

#if defined(_WIN32) && !defined(DEBUG_NO_EXPORTS)
#if defined(peace_EXPORTS)
#define PEACE_EXPORT __declspec(dllexport)
#else
#define PEACE_EXPORT
#endif
#else /* defined (_WIN32) */
#define PEACE_EXPORT
#endif

typedef void *CollectorPtr;
typedef void *WorldPtr;
typedef void *SceneNodePtr;
typedef void *MeshPtr;
typedef void *MaterialPtr;
typedef void *TexturePtr;

#endif // PEACE_COMMON_H
