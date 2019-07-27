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
            for (int i = 0; i < transform.childCount; ++i)
            {
                Destroy(transform.GetChild(i).gameObject);
            }
            
            foreach (var node in _collector.GetNodes())
            {
                Mesh mesh = _collector.GetMesh(node.Mesh);

                if (mesh != null)
                {
                    GameObject child = new GameObject(node.Mesh);
                    child.transform.SetParent(transform);
                    child.transform.localPosition = new Vector3((float)node.posX, (float)node.posZ, (float)node.posY);
                    child.transform.localScale = new Vector3((float)node.scaleX, (float)node.scaleZ, (float)node.scaleY);
                    child.transform.localEulerAngles = new Vector3((float)node.rotX, (float)node.rotZ, (float)node.rotY);
                    
                    MeshFilter meshFilter = child.AddComponent<MeshFilter>();
                    meshFilter.sharedMesh = mesh;

                    MeshRenderer meshRenderer = child.AddComponent<MeshRenderer>();

                    Material material = _collector.GetMaterial(node.Material);

                    if (material != null)
                    {
                        meshRenderer.material = material;
                    }
                    else
                    {
                        meshRenderer.material.shader = Shader.Find("Standard");
                    }
                }
            }
        }
        
        // Start is called before the first frame update
        void Start()
        {
            if (configLocation == "")
            {
                _world = World.CreateDemo();
            }
            else
            {
                _world = new World(configLocation);
            }
            
            _collector = new Collector();
            RunCollect();
        }

        void Update()
        {
            Vector3 newPos = tracking.transform.position;

            if (Vector3.Distance(newPos, _position) > 1 && !_collecting)
            {
                _position = newPos;
                // RunCollect();
            }
        }
    }

}
