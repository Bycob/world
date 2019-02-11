using System;
using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class WorldInterop : MonoBehaviour {

    [DllImport("cworld")]
    private static extern IntPtr createDemoWorld();

    [DllImport("cworld")]
    private static extern IntPtr createCollector();

    [DllImport("cworld")]
    private static extern void collect(IntPtr world, IntPtr collector);

    [DllImport("cworld")]
    private static extern IntPtr collectorGetChannel(IntPtr collectorPtr, int type);
    
    [DllImport("cworld")]
    private static extern int channelGetObjectsCount(IntPtr channelPtr);

    [DllImport("cworld")]
    private static extern void channelGetObjects(IntPtr channelPtr, [In, Out] IntPtr[] objects);
    
    [DllImport("cworld")]
    private static extern IntPtr objectGetMesh(IntPtr objectPtr);

    [DllImport("cworld")]
    private static extern void objectGetPosition(IntPtr objectPtr, [In, Out] double[] position);

    [DllImport("cworld")]
    private static extern int meshGetIndiceCount(IntPtr meshPtr);

    [DllImport("cworld")]
    private static extern void meshGetIndices(IntPtr meshPtr, [In, Out] int[] indices);

    [DllImport("cworld")]
    private static extern int meshGetVerticesSize(IntPtr meshPtr);

    [DllImport("cworld")]
    private static extern void meshGetVertices(IntPtr meshPtr, [In, Out] double[] vertices);


    public MeshFilter template;

    private IntPtr worldHandle;
    private IntPtr collectorHandle;

    // Start is called before the first frame update
    void Start()
    {
        worldHandle = createDemoWorld();
        collectorHandle = createCollector();
        collect(worldHandle, collectorHandle);

        IntPtr meshChannel = collectorGetChannel(collectorHandle, 2);
        
        IntPtr[] objects = new IntPtr[channelGetObjectsCount(meshChannel)];
        channelGetObjects(meshChannel, objects);
        
        foreach (IntPtr objectPtr in objects) {
            IntPtr meshPtr = objectGetMesh(objectPtr);
            double[] position = new double[3];
            objectGetPosition(objectPtr, position);

            int[] indices = new int[meshGetIndiceCount(meshPtr)];
            meshGetIndices(meshPtr, indices);

            double[] vertices = new double[meshGetVerticesSize(meshPtr)];
            meshGetVertices(meshPtr, vertices);

            int vertCount = vertices.Length / 8;
            Vector3[] backedVertices = new Vector3[vertCount];
            Vector3[] backedNormals = new Vector3[vertCount];
            
            for (int i = 0; i < vertCount; i++) {
                backedVertices[i] = new Vector3((float)vertices[i * 8 + 0], (float)vertices[i * 8 + 2], (float)vertices[i * 8 + 1]);
                backedNormals[i] = new Vector3((float)vertices[i * 8 + 3], (float)vertices[i * 8 + 5], (float)vertices[i * 8 + 4]);
            }

            Mesh mesh = new Mesh();
            mesh.vertices = backedVertices;
            mesh.normals = backedNormals;
            mesh.triangles = indices.Reverse().ToArray();

            MeshFilter machin = Instantiate(template);
            machin.gameObject.name = "Mesh" + indices.Length;
            machin.transform.SetParent(gameObject.transform);
            machin.transform.position = new Vector3((float)position[0], (float)position[2], (float)position[1]);

            machin.mesh = mesh;
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
