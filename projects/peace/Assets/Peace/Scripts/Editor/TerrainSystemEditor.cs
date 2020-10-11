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
                CreateTerrain();
            }

            if (GUILayout.Button("Cleanup"))
            {
                _terrainSystem.Clear();
            }
        }


        public async Task CreateTerrain()
        {
            var world = new World(CreateTerrainWorldDef());
            var collector = new Collector(Collector.Preset.ENGINE);

            ZoneView view = new ZoneView();
            int width = 1000;
            view.bbox.xmin = 0;
            view.bbox.ymin = 0;
            view.bbox.zmin = -2000;
            view.bbox.xmax = width;
            view.bbox.ymax = width;
            view.bbox.zmax = 4000;
            view.resolution = 10;

            Debug.Log("Start generating");
            await collector.CollectZone(world, view);

            Debug.Log("Generated!");
            _terrainSystem.AddTerrains(collector);

            Debug.Log("Terrains Added");
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

        private static WorldDef CreateTerrainWorldDef()
        {
            var worldDef = new WorldDef();

            var perlin = new PerlinTerrainGeneratorDef();
            perlin.perlinInfo.octaves = perlin.maxOctaves = 11;

            worldDef.ground.terrainRes = 2049;
            worldDef.ground.textureRes = 3;
            worldDef.ground.tileSystem.maxLod = 0;
            worldDef.ground.workers_list.Add(perlin);

            return worldDef;
        }
    }
}
