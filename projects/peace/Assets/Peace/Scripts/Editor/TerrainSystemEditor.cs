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
            var terrainSystem = terrainSystemGO.AddComponent<TerrainSystem>();
            terrainSystem.SetupDefaultLayers();
            Undo.RegisterCreatedObjectUndo(terrainSystemGO, "Created Terrain System");
        }


        private TerrainSystem _terrainSystem;

        void OnEnable()
        {
            _terrainSystem = (TerrainSystem) target;
        }

        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();

            string buttonText = _terrainSystem.IsEmpty() ? "Generate" : "Regenerate";

            GUI.enabled = !_terrainSystem.generating;
            if (GUILayout.Button(buttonText))
            {
                _terrainSystem.Regenerate();
            }
            GUI.enabled = true;
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

        public void OnSceneGUI()
        {
            float tw = _terrainSystem.tileWidth;
            var extensions = _terrainSystem.GetExtensionPossibility();

            List<Vector2Int> generate = new List<Vector2Int>();
            
            foreach (var extPos in extensions)
            {
                Vector3 buttonLoc = new Vector3((extPos.x + 0.5f) * tw, 0, (extPos.y + 0.5f) * tw);
                float pickSize = _terrainSystem.generating ? 0 : tw / 2;

                if (Handles.Button(buttonLoc, Quaternion.LookRotation(Vector3.up), tw / 2, pickSize, Handles.RectangleHandleCap))
                {
                    generate.Add(extPos);
                }
            }

            foreach(Vector2Int pos in generate)
            {
                Task.Run(() =>
                {
                    // Undo.RecordObject(myGameObject.transform, "Zero Transform Position");
                    _terrainSystem.GenerateTile(pos).RunSynchronously();
                });
            }
        }
    }
}
