
#if defined(_WIN32) && !defined(DEBUG_NO_EXPORTS)
#if defined(world_cwrapper_EXPORTS)
#define WORLDC_EXPORT __declspec(dllexport)
#else
#define WORLDC_EXPORT __declspec(dllimport)
#endif
#else  /* defined (_WIN32) */
#define WORLDC_EXPORT
#endif


#ifdef __cplusplus
extern "C" {
#endif
	typedef void* HANDLE;

    WORLDC_EXPORT HANDLE createDemoWorld();

	WORLDC_EXPORT HANDLE createCollector();

	WORLDC_EXPORT void collect(HANDLE world, HANDLE collector);

	WORLDC_EXPORT HANDLE collectorGetChannel(HANDLE collector, int type);

	WORLDC_EXPORT int channelGetObjectsCount(HANDLE channelPtr);
	WORLDC_EXPORT void channelGetObjects(HANDLE channelPtr, HANDLE* meshes);

	WORLDC_EXPORT HANDLE objectGetMesh(HANDLE object);
	WORLDC_EXPORT void objectGetPosition(HANDLE object, double *position);
	WORLDC_EXPORT void objectGetScale(HANDLE object, double *scale);

	WORLDC_EXPORT int meshGetIndiceCount(HANDLE meshPtr);
	WORLDC_EXPORT void meshGetIndices(HANDLE mesh, int* indices);

	// Gives vertices size in double
	WORLDC_EXPORT int meshGetVerticesSize(HANDLE meshPtr);
	WORLDC_EXPORT void meshGetVertices(HANDLE mesh, double* vertices);
#ifdef __cplusplus
}
#endif
