using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using UnityEngine;
using Object = UnityEngine.Object;

namespace Peace
{
    public class Collector
    {
        private IntPtr _handle;

        private CollectorView _view;

        private HashSet<string> _newNodes;
        private Dictionary<string, CollectorNode> _nodes;
        private Dictionary<string, Mesh> _meshes;
        private Dictionary<string, Material> _materials;
        private Dictionary<string, Texture2D> _textures;
        

        public Collector()
        {
            _handle = createCollector();

            _newNodes = new HashSet<string>();
            _nodes = new Dictionary<string, CollectorNode>();
            _meshes = new Dictionary<string, Mesh>();
            _materials = new Dictionary<string, Material>();
            _textures = new Dictionary<string, Texture2D>();

            SetPosition(Vector3.zero);
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
            _view.X = position.x;
            _view.Y = position.z;
            _view.Z = position.y;
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

        public async Task Collect(World world)
        {
            await Task.Run(() => collect(_handle, world._handle, _view));
            
            // Get from native code
            IntPtr[] nodes, meshes, materials, textures;
            string[] nodeNames, meshNames, materialNames, textureNames;

            GetChannel(NODE_CHANNEL, out nodeNames, out nodes);
            GetChannel(MESH_CHANNEL, out meshNames, out meshes);
            GetChannel(MATERIAL_CHANNEL, out materialNames, out materials);
            GetChannel(TEXTURE_CHANNEL, out textureNames, out textures);

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
                _meshes.Remove(key);
            }
            
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
                _textures.Remove(key);
            }
            
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
                _materials.Remove(key);
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


        // Dll functions
        private const int NODE_CHANNEL = 0;
        private const int MESH_CHANNEL = 1;
        private const int MATERIAL_CHANNEL = 2;
        private const int TEXTURE_CHANNEL = 3;
        
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct CollectorNode
        {
            public string Mesh;
            public string Material;
            public double posX, posY, posZ;
            public double scaleX, scaleY, scaleZ;
            public double rotX, rotY, rotZ;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct CollectorView
        {
            public double X, Y, Z;
        }
        
        [DllImport("peace")]
        private static extern IntPtr createCollector();
        
        [DllImport("peace")]
        private static extern void collect(IntPtr collector, IntPtr world, CollectorView view);

        [DllImport("peace")]
        private static extern int collectorGetChannelSize(IntPtr collectorPtr, int type);
        
        [DllImport("peace")]
        private static extern void collectorGetChannel(IntPtr collectorPtr, int type, 
            [In, Out] IntPtr[] names, [In, Out] IntPtr[] items);


        [DllImport("peace")]
        private static extern CollectorNode readNode(IntPtr nodePtr);
    }
}