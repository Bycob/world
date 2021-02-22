using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Peace.Serialization;
using UnityEditor;
using UnityEngine;

namespace Peace
{
    [ExecuteInEditMode]
    public class TerrainSystem : MonoBehaviour
    {
        // DONE allow removal of terrains by user
        // DONE allow reloading of terrainsystem and integration of terrains
        // DONE add a WorldTerrain component to store information on created terrains
        // TODO fix no texture on terrain when reloading from cache
        // TODO make user able to modify texture for the whole terrain (by referencing unity textures instead of anonymous images)
        // TODO make custom editors for colors
        // TODO make unity able to save scene and Ctrl Z when adding a new terrain
        // TODO Add UI for when the terrain is generating

        // For later?
        // TODO make biomes usable by modifying the biome map
        // TODO make custom editor for DistributionParams

        [System.Serializable]
        public struct Layer
        {
            public DistributionParams distribParams;
            public Color4dDef color;
            public ShaderDef shader;
        }

        // Public variables
        [Header("Terrain parameters")]
        public int resolution = 2049;
        public int octavesCount = 11;
        public float persistence = 0.5f;
        public float frequency = 4;
        public float tileWidth = 1000;
        public float minAltitude = -2000;
        public float maxAltitude = 4000;

        [Header("Biomes Parameters")]
        public bool useBiomes = true;
        public double biomeDensity = 1.0;
        public double limitBrightness = 4;

        [Header("Texture layers parameters")]
        public List<Layer> layers = new List<Layer>();

        // Private variables
        private Dictionary<Vector2Int, GameObject> _terrainGOs = new Dictionary<Vector2Int, GameObject>();
        private Dictionary<Vector2Int, Terrain> _terrains = new Dictionary<Vector2Int, Terrain>();
        
        private string _cacheLocation = "Assets/_world/";

        public bool generating { get; private set; } = false;

        private World _world;
        

        void OnEnable()
        {
            if (_world == null)
            {
                InitWorld();
                ReconnectGeneratedTerrains();
            }
        }

        public void SetupDefaultLayers()
        {
            // Rock
            layers.Add(new Layer
            {
                distribParams = new DistributionParams
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
                    slopeFactor = 0.0f,
                },
                color = new Color4dDef
                {
                    r = 0.3,
                    g = 0.3,
                    b = 0.3,
                    a = 1.0,
                },
                shader = new ShaderDef("texture-rock.frag")
            });

            // Sand
            layers.Add(new Layer
            {
                distribParams = new DistributionParams
                {
                    ha = -1.0f,
                    hb = 0.0f,
                    hc = 0.4000000059604645f,
                    hd = 0.44999998807907107f,
                    dha = -1.0f,
                    dhb = 0.0f,
                    dhc = 0.4000000059604645f,
                    dhd = 0.6000000238418579f,
                    hmin = 0.0f,
                    hmax = 1.0f,
                    dhmin = 0.0f,
                    dhmax = 1.0f,
                    threshold = 0.05000000074505806f,
                    slopeFactor = 0.0f
                },
                color = new Color4dDef
                {
                    r = 0.75,
                    g = 0.7,
                    b = 0.6,
                    a = 1.0,
                },
                shader = new ShaderDef("texture-sand.frag")
            });

            // Soil
            layers.Add(new Layer
            {
                distribParams = new DistributionParams
                {
                    ha = 0.33000001311302187f,
                    hb = 0.4000000059604645f,
                    hc = 0.6000000238418579f,
                    hd = 0.75f,
                    dha = -1.0f,
                    dhb = 0.0f,
                    dhc = 0.44999998807907107f,
                    dhd = 0.6499999761581421f,
                    hmin = 0.0f,
                    hmax = 0.8500000238418579f,
                    dhmin = 0.25f,
                    dhmax = 0.8500000238418579f,
                    threshold = 0.05000000074505806f,
                    slopeFactor = 0.0f
                },
                color = new Color4dDef
                {
                    r = 0.13,
                    g = 0.09,
                    b = 0.06,
                    a = 1.0,
                },
                shader = new ShaderDef("texture-soil.frag")
            });

            // Grass
            layers.Add(new Layer
            {
                distribParams = new DistributionParams
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
                },
                color = new Color4dDef
                {
                    r = 0.2,
                    g = 0.6,
                    b = 0.3,
                    a = 1.0,
                },
                shader = new ShaderDef("texture-grass.frag")
            });
            
            // Snow
            layers.Add(new Layer
            {
                distribParams = new DistributionParams
                {
                    ha = 0.6499999761581421f,
                    hb = 0.800000011920929f,
                    hc = 1.0f,
                    hd = 2.0f,
                    dha = -1.0f,
                    dhb = 0.0f,
                    dhc = 0.5f,
                    dhd = 0.699999988079071f,
                    hmin = 0.0f,
                    hmax = 1.0f,
                    dhmin = 0.0f,
                    dhmax = 1.0f,
                    threshold = 0.05000000074505806f,
                    slopeFactor = 0.0f
                },
                color = new Color4dDef
                {
                    r = 0.95,
                    g = 0.95,
                    b = 0.96,
                    a = 1.0,
                },
                shader = new ShaderDef("texture-snow.frag")
            });
        }

        public bool IsEmpty()
        {
            return _terrains.Count == 0;
        }

        public async void Regenerate()
        {
            if (generating)
            {
                return;
            }

            List<Vector2Int> deleted = new List<Vector2Int>(_terrainGOs.Keys);
            
            foreach (var key in deleted)
            {
                DeleteTerrain(key);
            }

            if (_world != null)
            {
                _world.Dispose();
                Util.DeleteTree(new DirectoryInfo(_cacheLocation));
            }

            InitWorld();

            if (deleted.Count == 0)
            {
                deleted.Add(new Vector2Int(0, 0));
            }

            foreach (var pos in deleted)
            {
                await GenerateTile(pos);
            }

            AssetDatabase.Refresh();
        }

        public async Task GenerateTile(Vector2Int tileCoords)
        {
            if (generating)
            {
                return;
            }
            generating = true;

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

            await collector.CollectZone(_world, view);

            AddTerrains(collector);
            generating = false;
        }

        public List<Vector2Int> GetExtensionPossibility()
        {
            List<Vector2Int> extensions = new List<Vector2Int>();
            Vector2Int[] directions = new Vector2Int[]
            {
                Vector2Int.left,
                Vector2Int.right,
                Vector2Int.down,
                Vector2Int.up,
            };

            foreach (Vector2Int pos in _terrains.Keys)
            {
                foreach (var dir in directions)
                {
                    Vector2Int extPos = pos + dir;

                    if (!_terrains.ContainsKey(extPos))
                    {
                        extensions.Add(extPos);
                    }
                }
            }

            return extensions;
        }

        private void InitWorld(bool vulkan = false)
        {
            var worldDef = new WorldDef();

            worldDef.ground.terrainRes = resolution;
            // TODO check what value should have this parameter
            worldDef.ground.textureRes = 3;
            worldDef.ground.minAltitude = minAltitude;
            worldDef.ground.maxAltitude = maxAltitude;
            worldDef.ground.tileSystem.maxLod = 0;
            worldDef.ground.tileSystem.baseSize.x = tileWidth;
            worldDef.ground.tileSystem.baseSize.y = tileWidth;

            var perlin = new PerlinTerrainGeneratorDef();
            perlin.perlinInfo.octaves = perlin.maxOctaves = octavesCount;
            perlin.perlinInfo.frequency = frequency;
            perlin.perlinInfo.persistence = persistence;
            worldDef.ground.workers_list.Add(perlin);

            if (useBiomes)
            {
                var customMap = new CustomWorldRMModifierDef();
                customMap.biomeDensity = biomeDensity;
                customMap.limitBrightness = limitBrightness;
                worldDef.ground.workers_list.Add(customMap);
            }

            var texturer = new MultilayerGroundTextureDef();
            foreach (Layer layer in layers)
            {
                texturer.layers.Add(layer.distribParams);
            }

            if (vulkan)
            {
                var texProvider = new VkwGroundTextureGeneratorDef();
                texturer.texProvider = texProvider;

                foreach (Layer layer in layers)
                {
                    // I've got this bug: https://issuetracker.unity3d.com/issues/vulkan-amd-editor-crashes-with-vk-icdnegotiateloadericdinterfaceversion-when-changing-api-to-vulkan
                    // only when I add the second texture
                    // on draw(6) (multiple draw calls?)
                    texProvider.layers.Add(layer.shader);
                }
            }
            else
            {
                var texProvider = new GroundTextureGeneratorDef();
                texturer.texProvider = texProvider;

                foreach (Layer layer in layers)
                {
                    texProvider.colors.Add(layer.color);
                }
            }

            worldDef.ground.workers_list.Add(texturer);

            _world = new World(worldDef);
            _world.SetCacheLocation(_cacheLocation);
        }

        /** \param removeOld: Remove terrains that are not in the collector. */
        private void AddTerrains(Collector collector, bool removeOld = false)
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
                        var worldTerrain = terrainGO.AddComponent<WorldTerrain>();
                        worldTerrain.terrainSystem = this;
                        worldTerrain.coords = coords;
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

            if (removeOld)
            {
                foreach (var key in toRemove)
                {
                    DeleteTerrain(key);
                }
            }

            UpdateNeighbours();
        }

        /** After deserializing, add already generated terrains. */
        private void ReconnectGeneratedTerrains()
        {
            int childCount = transform.childCount;

            for (int i = 0; i < childCount; ++i)
            {
                GameObject terrainGO = transform.GetChild(i).gameObject;
                Terrain terrain = terrainGO.GetComponent<Terrain>();
                WorldTerrain worldTerrain = terrainGO.GetComponent<WorldTerrain>();
                
                if (terrain != null && worldTerrain != null)
                {
                    Vector2Int pos = worldTerrain.coords;
                    _terrainGOs.Add(pos, terrainGO);
                    _terrains.Add(pos, terrain);
                }
            }
        }

        private void DeleteTerrain(Vector2Int coords)
        {
            _terrainGOs[coords].GetComponent<WorldTerrain>().terrainSystem = null;
            // TODO if in edit mode... else...
            DestroyImmediate(_terrainGOs[coords]);
            OnTerrainDeleted(coords);
        }

        internal void OnTerrainDeleted(Vector2Int coords)
        {
            _terrainGOs.Remove(coords);
            _terrains.Remove(coords);
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
