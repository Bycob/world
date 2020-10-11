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

        public TileSystem tileSystem = new TileSystem();


            [NonSerialized]
        public List<IGroundWorkerDef> workers_list = new List<IGroundWorkerDef>();

        [SerializeField]
        private List<String> workers = new List<String>();


        public void OnBeforeSerialize()
        {
            WorldSerialization.SerializeList(workers_list, workers);
        }

        public void OnAfterDeserialize()
        {

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
    public class TileSystem
    {
        public int maxLod = 12;
        public int factor = 2;
        public Vector3 baseSize = new Vector3(6000, 6000, 0);
        public Vector3Int bufferRes = new Vector3Int(512, 512, 0);
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