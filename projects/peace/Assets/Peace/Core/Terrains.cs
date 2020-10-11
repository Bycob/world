using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Peace
{
    public static class Terrains
    {
        public static void ReadTerrainData(TerrainData tData, IntPtr handle)
        {
            // Get height map
            int resolution = terrainGetResolution(handle);
            tData.heightmapResolution = resolution;
            float[,] heights = new float[resolution, resolution];
            readTerrain(handle, heights, false);
            // <info> function SetHeightsDelayLOD to not regenerate lods if needed </info>
            tData.SetHeights(0, 0, heights);
        }

        public static void ReadTerrainTextures(Terrain terrain, IntPtr terrainHandle, Collector collector)
        {
            TerrainData tData = terrain.terrainData;
            IntPtr matHandle = terrainGetMaterial(terrainHandle);
            int texCount = terrainGetLayerCount(terrainHandle);

            TerrainLayer[] layers = new TerrainLayer[texCount];

            float[,,] alphamaps = null;
            for (int i = 0; i < texCount; ++i)
            {
                // Distribution
                string distribTex = materialGetCustomMap(matHandle, "distribution" + i);
                IntPtr distribHandle = collector.GetTerrainHandle(distribTex);

                if (distribHandle != IntPtr.Zero)
                {
                    int distribRes = terrainGetResolution(distribHandle);

                    if (alphamaps == null)
                    {
                        alphamaps = new float[texCount, distribRes, distribRes];
                        tData.alphamapResolution = distribRes;
                    }

                    float[,] alphamap = new float[distribRes, distribRes];
                    readTerrain(distribHandle, alphamap, false);

                    // TODO find more elegant way to copy data (direct copy from C++, use of Array.Copy...)
                    for (int x = 0; x < distribRes; ++x)
                    {
                        for (int y = 0; y < distribRes; ++y)
                        {
                            alphamaps[i, x, y] = alphamap[x, y];
                        }
                    }
                }

                // Texture
                string layerTex = materialGetCustomMap(matHandle, "texture" + i);
                TerrainLayer layer = new TerrainLayer
                {
                    diffuseTexture = collector.GetTexture(layerTex)
                };
                layers[i] = layer;
            }

            if (texCount != 0)
            {
                tData.SetAlphamaps(0, 0, alphamaps);
                tData.terrainLayers = layers;
            }
        }

        internal static void UpdateTerrainBBox(Terrain terrain, BBox bbox)
        {
            TerrainData tData = terrain.terrainData;
            tData.size = new Vector3(
                (float)(bbox.xmax - bbox.xmin),
                (float)(bbox.zmax - bbox.zmin),
                (float)(bbox.ymax - bbox.ymin)
                );
            terrain.transform.position = new Vector3((float) bbox.xmin, (float) bbox.zmin, (float) bbox.ymin);
        }



        // Dll functions
        [DllImport("peace")]
        private static extern void readTerrain(IntPtr terrainPtr, [Out] float[,] value, bool applyBbox);

        [DllImport("peace")]
        private static extern int terrainGetResolution(IntPtr terrainPtr);

        [DllImport("peace")]
        private static extern IntPtr terrainGetMaterial(IntPtr terrainPtr);

        [DllImport("peace")]
        private static extern int terrainGetLayerCount(IntPtr terrainPtr);

        // TODO Find a way to make this private
        [DllImport("peace")]
        internal static extern BBox terrainGetBBox(IntPtr terrainPtr);

        [DllImport("peace")]
        private static extern string materialGetCustomMap(IntPtr materialPtr, string mapName);

    }
}