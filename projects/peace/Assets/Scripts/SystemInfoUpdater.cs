using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Profiling;
using UnityEngine.UI;

public class SystemInfoUpdater : MonoBehaviour
{
    public Text infoBox;
    
    // Update is called once per frame
    void Update()
    {
        // long unityMemory = Profiler.GetMonoHeapSizeLong();
        long reservedMemory = Profiler.GetTotalReservedMemoryLong();
        long unusedMemory = Profiler.GetTotalUnusedReservedMemoryLong();
        infoBox.text = (unusedMemory / 1e6).ToString("0.0") + "MB / "  + (reservedMemory / 1e6).ToString("0.0") + "MB";
    }
}
