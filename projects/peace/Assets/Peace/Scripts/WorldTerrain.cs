using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Peace
{
    [ExecuteInEditMode]
    public class WorldTerrain : MonoBehaviour
    {
        public TerrainSystem terrainSystem;
        public Vector2Int coords;

        void OnDestroy()
        {
            if (terrainSystem)
            {
                terrainSystem.OnTerrainDeleted(coords);
            }
        }
    }
}
