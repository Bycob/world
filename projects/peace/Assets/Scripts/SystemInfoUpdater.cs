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
        long unityMemory = Profiler.GetMonoHeapSizeLong();
        infoBox.text = unityMemory.ToString();
    }
}
