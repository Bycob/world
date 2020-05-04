using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace Peace
{
    public class Assets
    {
        public static Mesh GetMesh(IntPtr handle)
        {
            int vertSize = 0, indicesSize = 0;
            readMeshSizes(handle, ref vertSize, ref indicesSize);

            double[] vertices = new double[vertSize];
            int[] indices = new int[indicesSize];
            
            readMesh(handle, vertices, indices);

            int vertCount = vertSize / 8;

            Vector3[] vertPositions = new Vector3[vertCount];
            Vector3[] vertNormals = new Vector3[vertCount];
            Vector2[] vertUVs = new Vector2[vertCount];
            
            for (int i = 0; i < vertCount; i++) {
                vertPositions[i] = new Vector3((float)vertices[i * 8 + 0], (float)vertices[i * 8 + 2], (float)vertices[i * 8 + 1]);
                vertNormals[i] = new Vector3((float)vertices[i * 8 + 3], (float)vertices[i * 8 + 5], (float)vertices[i * 8 + 4]);
                vertUVs[i] = new Vector2((float)vertices[i * 8 + 6], 1 - (float)vertices[i * 8 + 7]);
            }

            Mesh mesh = new Mesh();
            mesh.vertices = vertPositions;
            mesh.normals = vertNormals;
            mesh.uv = vertUVs;
            mesh.triangles = indices.Reverse().ToArray();

            return mesh;
        }

        public static Material GetMaterial(IntPtr handle)
        {
            MaterialDescription matDesc = readMaterial(handle);
            
            Material material = new Material(Shader.Find("Standard"));
            material.color = new Color((float)matDesc.Kdr, (float)matDesc.Kdg, (float)matDesc.Kdb);
            material.SetFloat("_Glossiness", .0f);
            // material.EnableKeyword("_SMOOTHNESS_TEXTURE_ALBEDO_CHANNEL_A");
            material.EnableKeyword("_SPECGLOSSMAP");
            
            if (matDesc.transparent)
            {
                // TODO does not work
                material.SetOverrideTag("RenderType", "Transparent");
            }
            return material;
        }

        public static string GetMaterialTexture(IntPtr handle)
        {
            MaterialDescription matDesc = readMaterial(handle);
            return matDesc.MapKd;
        }

        public static Texture2D GetTexture(IntPtr handle)
        {
            IntPtr dataPtr = IntPtr.Zero;
            int width = 0, height = 0, type = 0;
            readTexture(handle, ref dataPtr, ref width, ref height, ref type);
            
            TextureFormat format;
            switch (type)
            {
                case IM_GREY:
                    format = TextureFormat.R8;
                    break;
                case IM_RGB:
                    format = TextureFormat.RGB24;
                    break;
                case IM_RGBA:
                    format = TextureFormat.RGBA32;
                    break;
                default:
                    throw new DataException("Image type not supported: " + type);
            }
            Texture2D texture = new Texture2D(width, height, format, false);
            
            int size = width * height * type;
            byte[] data = texture.GetRawTextureData();
            Marshal.Copy(dataPtr, data, 0, size);
            texture.LoadRawTextureData(data);
            texture.Apply();

            return texture;
        }
        
        // Dll functions
        private const int IM_GREY = 1;
        private const int IM_RGB = 3;
        private const int IM_RGBA = 4;
        
        [StructLayout(LayoutKind.Sequential)]
        struct MaterialDescription
        {
            public string MapKd;
            public double Kdr, Kdg, Kdb;
            public double Ksr, Ksg, Ksb;
            public bool transparent;
        }

        [DllImport("peace")]
        private static extern void readMeshSizes(IntPtr meshPtr, ref int vertSize, ref int indicesSize);
        
        [DllImport("peace")]
        private static extern void readMesh(IntPtr meshPtr, [In, Out] double[] vertices, [In, Out] int[] indices);
        
        [DllImport("peace")]
        private static extern MaterialDescription readMaterial(IntPtr materialPtr);

        [DllImport("peace")]
        private static extern void readTexture(IntPtr texturePtr, ref IntPtr data,
            ref int width, ref int height, ref int type);
    }
}

