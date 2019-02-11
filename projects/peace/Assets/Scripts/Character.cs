using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Character : MonoBehaviour
{
    public float ratio = 0.2f;
    public float speed = 0.07f;

    private Vector3 prevMousePosition;

    // Start is called before the first frame update
    void Start()
    {
        prevMousePosition = Input.mousePosition;
    }

    // Update is called once per frame
    void Update()
    {
        // view rotation
        Vector3 deltaPos = Input.mousePosition - prevMousePosition;
        transform.Rotate(new Vector3(0, deltaPos.x * ratio, 0), Space.World);
        transform.Rotate(new Vector3(- deltaPos.y * ratio, 0, 0), Space.Self);

        prevMousePosition = Input.mousePosition;

        // go forward
        if (Input.GetKey(KeyCode.W)) {
            transform.Translate(new Vector3(0, 0, speed));
        }
        else if (Input.GetKey(KeyCode.S)) {
            transform.Translate(new Vector3(0, 0, -speed));
        }
    }
}
