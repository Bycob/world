using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Peace
{
    public static class Terrains
    {
        public static TerrainData ReadTerrainData(IntPtr handle)
        {
            TerrainData tData = new TerrainData();

            // Get height map
            int resolution = terrainGetResolution(handle);
            float[,] heights = new float[resolution, resolution];
            readTerrain(handle, heights, false);
            // <info> function SetHeightsDelayLOD to not regenerate lods if needed </info>
            tData.SetHeights(0, 0, heights);

            return tData;
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
                string layerTex = materialGetCustomMap(matHandle, "layer" + i);
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
        private static extern int terrainGetResolution(IntPtr terrainPtr);

        [DllImport("peace")]
        private static extern void readTerrain(IntPtr terrainPtr, [In, Out] float[,] value, bool applyBbox);

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

    public class TerrainSystem
    {
        private Dictionary<Vector2Int, Terrain> _terrains = new Dictionary<Vector2Int, Terrain>();


        public void AddTerrains(IEnumerable<Collector.CollectorNode> nodes, Collector collector)
        {
            HashSet<Vector2Int> toRemove = new HashSet<Vector2Int>(_terrains.Keys);

            foreach (var node in nodes)
            {
                string terrainName = node.Mesh;
                IntPtr terrainHandle = collector.GetTerrainHandle(terrainName);

                if (terrainHandle != IntPtr.Zero)
                {
                    // get bbox
                    BBox bbox = Terrains.terrainGetBBox(terrainHandle);
                    double size = bbox.xmax - bbox.xmin;
                    var coords = new Vector2Int((int)(bbox.xmin / size), (int)(bbox.ymin / size));

                    if (!_terrains.ContainsKey(coords))
                    {
                        var terrain = new Terrain();
                        terrain.terrainData = Terrains.ReadTerrainData(terrainHandle);
                        Terrains.ReadTerrainTextures(terrain, terrainHandle, collector);
                        Terrains.UpdateTerrainBBox(terrain, bbox);
                        _terrains.Add(coords, terrain);
                    }
                    else
                    {
                        toRemove.Remove(coords);
                    }
                }
            }

            foreach (var key in toRemove)
            {
                _terrains.Remove(key);
            }

            UpdateNeighbours();
        }

        private void UpdateNeighbours()
        {
            foreach (var key in _terrains.Keys)
            {
                Terrain current = _terrains[key];
                Terrain leftNeighbor, topNeighbor, rightNeighbor, bottomNeighbor;
                _terrains.TryGetValue(key + Vector2Int.left, out leftNeighbor);
                _terrains.TryGetValue(key + Vector2Int.up, out topNeighbor);
                _terrains.TryGetValue(key + Vector2Int.right, out rightNeighbor);
                _terrains.TryGetValue(key + Vector2Int.down, out bottomNeighbor);
                current.SetNeighbors(leftNeighbor, topNeighbor, rightNeighbor, bottomNeighbor);
            }
        }
    }
}