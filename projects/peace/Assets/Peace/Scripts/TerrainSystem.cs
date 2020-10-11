using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Peace
{
    public class TerrainSystem : MonoBehaviour
    {
        // Public variables


        // Private variables
        private Dictionary<Vector2Int, GameObject> _terrainGOs = new Dictionary<Vector2Int, GameObject>();
        private Dictionary<Vector2Int, Terrain> _terrains = new Dictionary<Vector2Int, Terrain>();


        public void Clear()
        {
            foreach (var key in _terrainGOs.Keys)
            {
                DestroyImmediate(_terrainGOs[key]);
            }

            _terrainGOs.Clear();
            _terrains.Clear();
        }

        public void AddTerrains(Collector collector)
        {
            // Get new nodes
            HashSet<Vector2Int> toRemove = new HashSet<Vector2Int>(_terrains.Keys);

            foreach (var nodeName in collector.GetNewNodes())
            {
                Debug.Log(nodeName);

                var node = collector.GetNode(nodeName);
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
                        GameObject terrainGO = new GameObject("Terrain " + coords);
                        terrainGO.transform.SetParent(transform);
                        var terrain = terrainGO.AddComponent<Terrain>();

                        Terrains.ReadTerrainData(terrain.terrainData, terrainHandle);
                        Terrains.ReadTerrainTextures(terrain, terrainHandle, collector);
                        Terrains.UpdateTerrainBBox(terrain, bbox);

                        _terrainGOs.Add(coords, terrainGO);
                        _terrains.Add(coords, terrain);
                        Debug.Log("Done");
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

                Destroy(_terrainGOs[key]);
                _terrainGOs.Remove(key);
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
