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
                Vector3 buttonLoc = new Vector3((extPos.x + 0.5f) * tw, _terrainSystem.minAltitude, (extPos.y + 0.5f) * tw);
                float pickSize = _terrainSystem.generating ? 0 : tw / 2;

                if (Handles.Button(buttonLoc, Quaternion.LookRotation(Vector3.up), tw / 2, pickSize, Handles.RectangleHandleCap))
                {
                    generate.Add(extPos);
                }
            }

            if (!_terrainSystem.generating)
            { 
                foreach (Vector2Int pos in generate)
                {
                    _terrainSystem.GenerateTile(pos);
                }
            }
            else
            {
                foreach (Vector2Int pos in _terrainSystem.generatingCoords)
                {
                    DrawHandleBox(
                        new Vector3(pos.x * tw, _terrainSystem.minAltitude, pos.y * tw),
                        new Vector3(tw, _terrainSystem.maxAltitude - _terrainSystem.minAltitude, tw),
                        new Color(0, 0.8f, 0.9f, 0.5f)
                    );
                }
            }
        }

        private void DrawHandleBox(Vector3 o, Vector3 sizes, Color color)
        {
            Vector3 sx = Vector3.right * sizes.x;
            Vector3 sy = Vector3.up * sizes.y;
            Vector3 sz = Vector3.forward * sizes.z;

            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o, o + sx, o + sx + sy, o + sy }, color, color);
            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o, o + sz, o + sz + sy, o + sy }, color, color);
            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o, o + sx, o + sx + sz, o + sz }, color, color);
            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o + sx, o + sx + sy, o + sx + sy + sz, o + sx + sz }, color, color);
            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o + sy, o + sx + sy, o + sx + sy + sz, o + sy + sz }, color, color);
            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o + sz, o + sx + sz, o + sx + sy + sz, o + sy + sz }, color, color);
        }
    }
}

