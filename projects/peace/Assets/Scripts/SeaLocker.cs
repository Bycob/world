using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SeaLocker : MonoBehaviour
{
    [SerializeField]
    private Transform follow;
    
    // Update is called once per frame
    void Update()
    {
        Vector3 pos = transform.position;
        pos.x = follow.position.x;
        pos.z = follow.position.z;
        transform.position = pos;
    }
}
