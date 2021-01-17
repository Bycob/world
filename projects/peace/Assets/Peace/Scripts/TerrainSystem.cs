using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading.Tasks;
using Peace.Serialization;
using UnityEngine;

namespace Peace
{
    public class TerrainSystem : MonoBehaviour
    {
        // Public variables
        [Header("Terrain parameters")]
        public int resolution = 2049;
        public int octavesCount = 11;
        public float tileWidth = 1000;
        public float minAltitude = -2000;
        public float maxAltitude = 4000;

        [Header("Biomes Parameters")]
        public double biomeDensity = 1.0;
        public double limitBrightness = 4;

        // Private variables
        private Dictionary<Vector2Int, GameObject> _terrainGOs = new Dictionary<Vector2Int, GameObject>();
        private Dictionary<Vector2Int, Terrain> _terrains = new Dictionary<Vector2Int, Terrain>();

        private World _world;

        private void InitWorld()
        {
            var worldDef = new WorldDef();

            worldDef.ground.terrainRes = resolution;
            worldDef.ground.textureRes = 3;
            worldDef.ground.minAltitude = minAltitude;
            worldDef.ground.maxAltitude = maxAltitude;
            worldDef.ground.tileSystem.maxLod = 0;
            worldDef.ground.tileSystem.baseSize.x = tileWidth;
            worldDef.ground.tileSystem.baseSize.y = tileWidth;

            var perlin = new PerlinTerrainGeneratorDef();
            perlin.perlinInfo.octaves = perlin.maxOctaves = octavesCount;
            worldDef.ground.workers_list.Add(perlin);

            var customMap = new CustomWorldRMModifierDef();
            customMap.biomeDensity = biomeDensity;
            customMap.limitBrightness = limitBrightness;
            worldDef.ground.workers_list.Add(customMap);

            var texturer = new MultilayerGroundTextureDef();
            // Texture 1
            texturer.layers.Add(new DistributionParams
            {
                ha = -1.0f,
                hb = 0.0f,
                hc = 1.0f,
                hd = 2.0f,
                dha = -1.0f,
                dhb = 0.0f,
                dhc = 1.0f,
                dhd = 2.0f,
                hmin = 0.0f,
                hmax = 1.0f,
                dhmin = 0.0f,
                dhmax = 1.0f,
                threshold = 0.05f,
                slopeFactor = 0.0f
            });
            texturer.texProvider.layers.Add(new ShaderDef("texture-rock.frag"));

            // Texture 2
            // I've got this bug: https://issuetracker.unity3d.com/issues/vulkan-amd-editor-crashes-with-vk-icdnegotiateloadericdinterfaceversion-when-changing-api-to-vulkan
            // only when I add texture 2
            // on draw(6) (multiple draw calls?)
            /*texturer.layers.Add(new DistributionParams
            {
                ha = 0.33000001311302187f,
                hb = 0.4000000059604645f,
                hc = 0.6000000238418579f,
                hd = 0.699999988079071f,
                dha = -1.0f,
                dhb = 0.0f,
                dhc = 0.4000000059604645f,
                dhd = 0.6000000238418579f,
                hmin = 0.0f,
                hmax = 1.0f,
                dhmin = 0.25f,
                dhmax = 0.6000000238418579f,
                threshold = 0.05000000074505806f,
                slopeFactor = 0.0f
            });
            texturer.texProvider.layers.Add(new ShaderDef("texture-grass.frag"));*/

            worldDef.ground.workers_list.Add(texturer);

            _world = new World(worldDef);
            _world.SetCacheLocation("Assets/_world/");
        }

        public async void Regenerate()
        {
            InitWorld();

            Vector2Int pos = new Vector2Int(0, 0);
            await GenerateTile(pos);
        }

        public async Task GenerateTile(Vector2Int tileCoords)
        {
            var collector = new Collector(Collector.Preset.ENGINE);

            var view = new ZoneView();
            const double margin = 0.05;
            float width = tileWidth;
            view.bbox.xmin = (tileCoords.x + margin) * width;
            view.bbox.ymin = (tileCoords.y + margin) * width;
            view.bbox.zmin = minAltitude;
            view.bbox.xmax = view.bbox.xmin + width * (1 - margin * 2);
            view.bbox.ymax = view.bbox.ymin + width * (1 - margin * 2);
            view.bbox.zmax = maxAltitude;
            view.resolution = resolution / tileWidth;

            Debug.Log("Start generating");
            await collector.CollectZone(_world, view);

            Debug.Log("Generated!");
            AddTerrains(collector);

            Debug.Log("Terrains Added");
        }

        public void Clear()
        {
            foreach (var key in _terrainGOs.Keys)
            {
                DestroyImmediate(_terrainGOs[key]);
            }

            _terrainGOs.Clear();
            _terrains.Clear();

            _world = null;
        }

        public void AddTerrains(Collector collector)
        {
            // Get new nodes
            HashSet<Vector2Int> toRemove = new HashSet<Vector2Int>(_terrains.Keys);

            foreach (var nodeName in collector.GetNewNodes())
            {
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
                        terrain.terrainData = new TerrainData();
                        terrainGO.AddComponent<TerrainCollider>().terrainData = terrain.terrainData;

                        Terrains.ReadTerrainData(terrain.terrainData, terrainHandle);
                        Terrains.ReadTerrainTextures(terrain, terrainHandle, collector);
                        Terrains.UpdateTerrainBBox(terrain, bbox);

                        _terrainGOs.Add(coords, terrainGO);
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
