using System.Collections;
using System.Collections.Generic;
using System.Threading.Tasks;
using Peace.Serialization;
using UnityEditor;
using UnityEngine;

namespace Peace
{
    [CustomEditor(typeof(TerrainSystem))]
    public class TerrainSystemEditor : Editor
    {
        [MenuItem("GameObject/World/Terrain System", false, 0)]
        public static void CreateTerrainSystem()
        {
            GameObject terrainSystemGO = new GameObject("Terrain System");
            terrainSystemGO.AddComponent<TerrainSystem>();
        }


        private TerrainSystem _terrainSystem;

        void OnEnable()
        {
            _terrainSystem = (TerrainSystem) target;
        }

        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();

            if (GUILayout.Button("Generate"))
            {
                _terrainSystem.Regenerate();
            }

            if (GUILayout.Button("Cleanup"))
            {
                _terrainSystem.Clear();
            }
        }

        private void AddTestTerrain()
        {
            GameObject terrainGO = new GameObject("Terrain");
            terrainGO.transform.SetParent(_terrainSystem.transform);
            var terrain = terrainGO.AddComponent<Terrain>();

            var tData = new TerrainData();
            tData.heightmapResolution = 255;
            tData.size = new Vector3(20, 20, 20);

            terrain.terrainData = tData;
            terrainGO.AddComponent<TerrainCollider>().terrainData = terrain.terrainData;
        }
    }
}
