using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;

namespace Peace
{
    
    public class RealTimeWorld : MonoBehaviour
    {
        public String configLocation = "";
        public GameObject tracking;

        private Collector _collector;
        private World _world;
        private Vector3 _position;

        private bool _collecting;

        private Queue<GameObject> _objectPool;
        private List<GameObject> _objectsUsed;
        
        private async void RunCollect()
        {
            _collecting = true;
            _collector.SetPosition(_position);
            await _collector.Collect(_world);
            UpdateFromCollector();
            _collecting = false;
        }

        private void UpdateFromCollector()
        {
            for (int i = _objectsUsed.Count - 1; i >= 0; --i)
            {
                GameObject used = _objectsUsed[i];

                if (!_collector.HasNode(used.name))
                {
                    _objectsUsed.RemoveAt(i);
                    used.SetActive(false);
                    _objectPool.Enqueue(used);
                }
            }
            
            foreach (var nodeKey in _collector.GetNewNodes())
            {
                var node = _collector.GetNode(nodeKey);
                Mesh mesh = _collector.GetMesh(node.Mesh);

                if (mesh != null)
                {
                    GameObject child = AllocateObject(nodeKey);
                    child.transform.localPosition = new Vector3((float)node.posX, (float)node.posZ, (float)node.posY);
                    child.transform.localScale = new Vector3((float)node.scaleX, (float)node.scaleZ, (float)node.scaleY);
                    child.transform.localEulerAngles = new Vector3((float)node.rotX, (float)node.rotZ, (float)node.rotY);
                    
                    MeshFilter meshFilter = child.GetComponent<MeshFilter>();
                    meshFilter.sharedMesh = mesh;

                    MeshRenderer meshRenderer = child.GetComponent<MeshRenderer>();

                    Material material = _collector.GetMaterial(node.Material);

                    if (material != null)
                    {
                        meshRenderer.material = material;
                    }
                    else
                    {
                        meshRenderer.material.shader = Shader.Find("Standard");
                    }

                    _objectsUsed.Add(child);
                }
            }
        }

        private GameObject AllocateObject(string name)
        {
            GameObject obj;

            if (_objectPool.Count != 0)
            {
                obj = _objectPool.Dequeue();
                obj.SetActive(true);
                obj.name = name;
            }
            else
            {
                obj = new GameObject(name);
                obj.transform.SetParent(transform);
                obj.AddComponent<MeshFilter>();
                obj.AddComponent<MeshRenderer>();
            }
            return obj;
        }
        
        // Start is called before the first frame update
        void Start()
        {
            _objectPool = new Queue<GameObject>();
            _objectsUsed = new List<GameObject>();

            if (configLocation == "")
            {
                _world = World.CreateDemo();
            }
            else
            {
                _world = new World(configLocation);
            }
            
            _collector = new Collector();
        }

        void Update()
        {
            Vector3 newPos = tracking.transform.position;

            if (Vector3.Distance(newPos, _position) > 1 && !_collecting)
            {
                _position = newPos;
                RunCollect();
            }
        }
    }

}
