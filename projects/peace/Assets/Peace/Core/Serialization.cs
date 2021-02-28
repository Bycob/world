using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using UnityEngine;

namespace Peace.Serialization
{
    // TODO Find real lib for json serialization
    // An idea: use yaml, which is simpler

    public static class WorldSerialization
    {
        private const string DEL = "$";

        /** Serialize a world definition object to be sent to World API.
         * This function supports serialization of polymorphic objects,
         * as it is not supported by Unity. */
        public static string ToJson(object obj)
        {
            string jsonStr = JsonUtility.ToJson(obj);
            Regex rx = new Regex("\"\\$(.*?)\\$\"", RegexOptions.Compiled);
            var match = rx.Match(jsonStr);

            while (match.Success)
            {
                string subobj = match.Groups[1].Value.Replace("\\\"", "\"");
                jsonStr = rx.Replace(jsonStr, subobj, 1);
                match = rx.Match(jsonStr);
            }

            return jsonStr;
        }

        /** This function may be called from OnBeforeSerialize callback. It
         * transforms a list of polymorphic objects to a list of their JSON
         * representation. Then this list is processed by ToJson() into a list
         * of json objects. */
        public static void SerializeList<T>(List<T> from, List<string> to)
        {
            to.Clear();

            foreach (var el in from)
            {
                to.Add(DEL + ToJson(el) + DEL);
            }
        }
    }

    [Serializable]
    public class WorldDef
    {
        public HeightmapGroundDef ground = new HeightmapGroundDef();

        public List<String> nodes = new List<String>();
    }

    [Serializable]
    public class HeightmapGroundDef : ISerializationCallbackReceiver
    {
        [SerializeField]
        public string type = "HeightmapGround";

        public double minAltitude = -2000.0;
        public double maxAltitude = 4000.0;
        public int terrainRes = 65;
        public int textureRes = 128;
        public int texPixSize = 4;

        public TileSystem tileSystem = new TileSystem()
        {
            maxLod = 12,
            factor = 2,
            baseSize = new Vector3(6000, 6000, 0),
            bufferRes = new Vector3Int(512, 512, 0)
        };


        [NonSerialized]
        public List<IGroundWorkerDef> workers_list = new List<IGroundWorkerDef>();

        [SerializeField]
        private List<string> workers = new List<string>();


        public void OnBeforeSerialize()
        {
            WorldSerialization.SerializeList(workers_list, workers);
        }

        public void OnAfterDeserialize()
        {
            // TODO transfer workers content to workers_list
        }
    }

    [Serializable]
    public class IGroundWorkerDef {}

    [Serializable]
    public class PerlinTerrainGeneratorDef : IGroundWorkerDef
    {
        public string type = "PerlinTerrainGenerator";

        public int maxOctaves = 11;

        public PerlinInfo perlinInfo = new PerlinInfo();
    }

    [Serializable]
    public class CustomWorldRMModifierDef : IGroundWorkerDef
    {
        public string type = "CustomWorldRMModifier";

        public double biomeDensity = 1.0;
        public double limitBrightness = 4;

        public TileSystem tileSystem = new TileSystem()
        {
            maxLod = 0,
            factor = 2,
            baseSize = new Vector3(4e5f, 4e5f, 0.0f),
            bufferRes = new Vector3Int(400, 400, 0)
        };
    }

    [Serializable]
    public class MultilayerGroundTextureDef : IGroundWorkerDef
    {
        public string type = "MultilayerGroundTexture";

        public uint distribResolution = 257;
        public bool generateTexture = false;

        public List<DistributionParams> layers = new List<DistributionParams>();

        public ITextureProviderDef texProvider = new GroundTextureGeneratorDef();
    }

    [Serializable]
    public class ITextureProviderDef
    {
        public string type;
  
        public int texWidth = 256;

        public List<ShaderDef> layers = new List<ShaderDef>();
        public List<Color4dDef> colors = new List<Color4dDef>();
    }

    [Serializable]
    public class GroundTextureGeneratorDef : ITextureProviderDef
    {
        public GroundTextureGeneratorDef()
        {
            type = "GroundTextureGenerator";
        }
    }

    [Serializable]
    public class VkwGroundTextureGeneratorDef : ITextureProviderDef
    {
        public VkwGroundTextureGeneratorDef()
        {
            type = "VkwGroundTextureGenerator";
        }
    }

    [Serializable]
    public class ShaderDef
    {
        public string fragmentPath;
        public List<ShaderParam> @params = new List<ShaderParam>();

        public ShaderDef(string fragmentPath = "", IEnumerable<ShaderParam> pparams = null)
        {
            this.fragmentPath = fragmentPath;
            if (pparams != null)
            {
                @params.AddRange(pparams);
            }
        }
    }

    [Serializable]
    public struct TileSystem
    {
        public int maxLod;
        public int factor;
        public Vector3 baseSize;
        public Vector3Int bufferRes;
    }

    [Serializable]
    public struct DistributionParams
    {
        public float ha;
        public float hb;
        public float hc;
        public float hd;
        public float dha;
        public float dhb;
        public float dhc;
        public float dhd;
        public float hmin;
        public float hmax;
        public float dhmin;
        public float dhmax;
        public float threshold;
        public float slopeFactor;
    }

    [Serializable]
    public struct ShaderParam
    {
        public string name;
        public int type;
        public string value;
    }

    [Serializable]
    public struct Color4dDef
    {
        public double r;
        public double g;
        public double b;
        public double a;
    }

    [Serializable]
    public class PerlinInfo
    {
        public int octaves = 5;
        public double persistence = 0.45;
        public bool repeatable = false;
        public int reference = 0;
        public double frequency = 4;
    }
}