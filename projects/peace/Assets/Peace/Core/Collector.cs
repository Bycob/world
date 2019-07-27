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

        private List<CollectorNode> _nodes;
        private Dictionary<string, Mesh> _meshes;
        private Dictionary<string, Material> _materials;
        private Dictionary<string, Texture2D> _textures;
        

        public Collector()
        {
            _handle = createCollector();
            
            _nodes = new List<CollectorNode>();
            _meshes = new Dictionary<string, Mesh>();
            _materials = new Dictionary<string, Material>();
            _textures = new Dictionary<string, Texture2D>();

            SetPosition(Vector3.zero);
        }

        public void Reset()
        {
            _nodes.Clear();
            _meshes.Clear();
            _materials.Clear();
            _textures.Clear();
        }

        public void SetPosition(Vector3 position)
        {
            _view.X = position.x;
            _view.Y = position.y;
            _view.Z = position.z;
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
            Reset();
            
            await Task.Run(() => collect(_handle, world._handle, _view));

            IntPtr[] nodes, meshes, materials, textures;
            string[] nodeNames, meshNames, materialNames, textureNames;

            GetChannel(NODE_CHANNEL, out nodeNames, out nodes);
            GetChannel(MESH_CHANNEL, out meshNames, out meshes);
            GetChannel(MATERIAL_CHANNEL, out materialNames, out materials);
            GetChannel(TEXTURE_CHANNEL, out textureNames, out textures);
            
            foreach (IntPtr nodePtr in nodes)
            {
                _nodes.Add(readNode(nodePtr));
            }

            for (int i = 0; i < meshes.Length; ++i)
            {
                _meshes[meshNames[i]] = Assets.GetMesh(meshes[i]);
            }
            
            for (int i = 0; i < textures.Length; ++i)
            {
                _textures[textureNames[i]] = Assets.GetTexture(textures[i]);
            }

            for (int i = 0; i < materials.Length; ++i)
            {
                Material material = Assets.GetMaterial(materials[i]);
                Texture2D texture;
                if (_textures.TryGetValue(Assets.GetMaterialTexture(materials[i]), out texture))
                {
                    material.mainTexture = texture;
                }
                _materials[materialNames[i]] = material;
            }
        }

        public IEnumerable<CollectorNode> GetNodes()
        {
            return _nodes;
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