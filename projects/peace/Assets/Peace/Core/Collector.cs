using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using UnityEngine;
using Object = UnityEngine.Object;

namespace Peace
{
    public class Collector
    {
        public enum Preset
        {
            SCENE = 1,
            ENGINE = 2
        }

        private IntPtr _handle;

        private HashSet<string> _newNodes;
        private Dictionary<string, CollectorNode> _nodes;
        private Dictionary<string, Mesh> _meshes;
        private Dictionary<string, Material> _materials;
        private Dictionary<string, Texture2D> _textures;
        private Dictionary<string, IntPtr> _terrainHandles;

        public CollectorStats LastStats = new CollectorStats();


        public Collector(Preset preset = Preset.SCENE)
        {
            _handle = createCollector((int) preset);

            _newNodes = new HashSet<string>();
            _nodes = new Dictionary<string, CollectorNode>();
            _meshes = new Dictionary<string, Mesh>();
            _materials = new Dictionary<string, Material>();
            _textures = new Dictionary<string, Texture2D>();
            _terrainHandles = new Dictionary<string, IntPtr>();

            SetPosition(Vector3.zero);
        }

        ~Collector()
        {
            freeCollector(_handle);
        }

        public void Reset()
        {
            _newNodes.Clear();
            _nodes.Clear();
            _meshes.Clear();
            _materials.Clear();
            _textures.Clear();
        }

        public void SetPosition(Vector3 position)
        {
        }

        private void GetChannel(int type, out string[] names, out IntPtr[] items)
        {
            int size = collectorGetChannelSize(_handle, type);
            IntPtr[] namesPtrArray = new IntPtr[size];
            IntPtr[] itemsArray = new IntPtr[size];
            collectorGetChannel(_handle, type, namesPtrArray, itemsArray);

            names = Util.ToStringArray(namesPtrArray);
            items = itemsArray;
        }

        public async Task CollectFirstPerson(World world, FirstPersonView view)
        {
            await Task.Run(() => collectFirstPerson(_handle, world._handle, view));
            await Collect(world);
        }

        public async Task CollectZone(World world, ZoneView view)
        {
            await Task.Run(() => collectZone(_handle, world._handle, view));
            await Collect(world);
        }

        private async Task Collect(World world)
        {
            Stopwatch sw = new Stopwatch();
            sw.Start();

            // Get from native code
            IntPtr[] nodes = new IntPtr[0], meshes = new IntPtr[0], materials = new IntPtr[0], textures = new IntPtr[0], terrains = new IntPtr[0];
            string[] nodeNames = new string[0], meshNames = new string[0], materialNames = new string[0], textureNames = new string[0], terrainNames = new string[0];

            await Task.Run(() =>
            {
                GetChannel(NODE_CHANNEL, out nodeNames, out nodes);
                GetChannel(MESH_CHANNEL, out meshNames, out meshes);
                GetChannel(MATERIAL_CHANNEL, out materialNames, out materials);
                GetChannel(TEXTURE_CHANNEL, out textureNames, out textures);
                GetChannel(TERRAIN_CHANNEL, out terrainNames, out terrains);
            });

            double l = LastStats.interopTime = sw.Elapsed.TotalMilliseconds;


            // Update scene nodes
            _newNodes.Clear();
            HashSet<string> toRemove = new HashSet<string>(_nodes.Keys);

            for (int i = 0; i < nodes.Length; ++i)
            {
                if (!_nodes.ContainsKey(nodeNames[i]))
                {
                    _nodes.Add(nodeNames[i], readNode(nodes[i]));
                    _newNodes.Add(nodeNames[i]);
                }
                else
                {
                    toRemove.Remove(nodeNames[i]);
                }
            }

            foreach (var key in toRemove)
            {
                _nodes.Remove(key);
            }

            LastStats.nodesTime = sw.Elapsed.TotalMilliseconds - l;
            l = sw.Elapsed.TotalMilliseconds;


            // Update meshes
            toRemove = new HashSet<string>(_meshes.Keys);

            for (int i = 0; i < meshes.Length; ++i)
            {
                if (!_meshes.ContainsKey(meshNames[i]))
                {
                    _meshes.Add(meshNames[i], Assets.GetMesh(meshes[i]));
                }
                else
                {
                    toRemove.Remove(meshNames[i]);
                }
            }

            foreach (var key in toRemove)
            {
                Object.Destroy(_meshes[key]);
                _meshes.Remove(key);
            }

            LastStats.meshesTime = sw.Elapsed.TotalMilliseconds - l;
            l = sw.Elapsed.TotalMilliseconds;


            // Update textures
            toRemove = new HashSet<string>(_textures.Keys);

            for (int i = 0; i < textures.Length; ++i)
            {
                if (!_textures.ContainsKey(textureNames[i]))
                {
                    _textures.Add(textureNames[i], Assets.GetTexture(textures[i]));
                }
                else
                {
                    toRemove.Remove(textureNames[i]);
                }
            }

            foreach (var key in toRemove)
            {
                Object.Destroy(_textures[key]);
                _textures.Remove(key);
            }

            LastStats.texturesTime = sw.Elapsed.TotalMilliseconds - l;
            l = sw.Elapsed.TotalMilliseconds;


            // Update materials
            toRemove = new HashSet<string>(_materials.Keys);

            for (int i = 0; i < materials.Length; ++i)
            {
                if (!_materials.ContainsKey(materialNames[i]))
                {
                    Material material = Assets.GetMaterial(materials[i]);
                    Texture2D texture;
                    if (_textures.TryGetValue(Assets.GetMaterialTexture(materials[i]), out texture))
                    {
                        material.mainTexture = texture;
                    }
                    _materials[materialNames[i]] = material;
                }
                else
                {
                    toRemove.Remove(materialNames[i]);
                }
            }

            foreach (var key in toRemove)
            {
                Object.Destroy(_materials[key]);
                _materials.Remove(key);
            }

            sw.Stop();
            LastStats.materialsTime = sw.Elapsed.TotalMilliseconds - l;
            LastStats.totalTime = sw.Elapsed.TotalMilliseconds;


            // Update terrains
            toRemove = new HashSet<string>(_terrainHandles.Keys);

            for (int i = 0; i < terrains.Length; ++i)
            {
                if (!_terrainHandles.ContainsKey(terrainNames[i]))
                {
                    _terrainHandles[terrainNames[i]] = terrains[i];
                }
                else
                {
                    toRemove.Remove(terrainNames[i]);
                }
            }

            foreach (var key in toRemove)
            {
                _terrainHandles.Remove(key);
            }
        }

        public IEnumerable<string> GetNewNodes()
        {
            return _newNodes;
        }

        public bool HasNode(string key)
        {
            return _nodes.ContainsKey(key);
        }

        public CollectorNode GetNode(string key)
        {
            return _nodes[key];
        }

        public Mesh GetMesh(string key)
        {
            Mesh mesh;
            return _meshes.TryGetValue(key, out mesh) ? mesh : null;
        }

        public Material GetMaterial(string key)
        {
            Material material;
            return _materials.TryGetValue(key, out material) ? material : null;
        }

        public Texture2D GetTexture(string key)
        {
            Texture2D texture;
            return _textures.TryGetValue(key, out texture) ? texture : null;
        }

        internal IntPtr GetTerrainHandle(string key)
        {
            IntPtr handle;
            return _terrainHandles.TryGetValue(key, out handle) ? handle : IntPtr.Zero;
        }


        // Dll functions
        private const int NODE_CHANNEL = 0;
        private const int MESH_CHANNEL = 1;
        private const int MATERIAL_CHANNEL = 2;
        private const int TEXTURE_CHANNEL = 3;
        private const int TERRAIN_CHANNEL = 4;

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct CollectorNode
        {
            public string Mesh;
            public string Material;
            public double posX, posY, posZ;
            public double scaleX, scaleY, scaleZ;
            public double rotX, rotY, rotZ;
        }

        [DllImport("peace")]
        private static extern IntPtr createCollector(int preset);

        [DllImport("peace")]
        private static extern void freeCollector(IntPtr handle);

        [DllImport("peace")]
        private static extern void collectFirstPerson(IntPtr collector, IntPtr world, FirstPersonView view);

        [DllImport("peace")]
        private static extern void collectZone(IntPtr collector, IntPtr world, ZoneView view);

        [DllImport("peace")]
        private static extern int collectorGetChannelSize(IntPtr collectorPtr, int type);

        [DllImport("peace")]
        private static extern void collectorGetChannel(IntPtr collectorPtr, int type,
            [In, Out] IntPtr[] names, [In, Out] IntPtr[] items);


        [DllImport("peace")]
        private static extern CollectorNode readNode(IntPtr nodePtr);
    }
}