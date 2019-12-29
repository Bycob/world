using System;

namespace Peace
{
    [Serializable]
    public class RealTimeWorldStatistics
    {
        public int removed;
        public int added;
        public float updateTime;

        public CollectorStats collectorStats;
    }

    [Serializable]
    public class CollectorStats
    {
        public int meshesAdded;
        public int meshesRemoved;
        public int texturesAdded;
        public int texturesRemoved;
        public double interopTime;
        public double nodesTime;
        public double meshesTime;
        public double texturesTime;
        public double materialsTime;
        public double totalTime;
    }
}