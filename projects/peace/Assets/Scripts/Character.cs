using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Character : MonoBehaviour
{
    public float ratio = 1.0f;
    public float speed = 0.07f;

    private bool lockCursor;
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
        Vector2 deltaPos = new Vector2(Input.GetAxisRaw("Mouse X"), Input.GetAxisRaw("Mouse Y"));
        transform.Rotate(new Vector3(0, deltaPos.x * ratio, 0), Space.World);
        transform.Rotate(new Vector3(- deltaPos.y * ratio, 0, 0), Space.Self);

        // go forward
        if (Input.GetKey(KeyCode.W)) {
            transform.Translate(new Vector3(0, 0, speed));
        }
        else if (Input.GetKey(KeyCode.S)) {
            transform.Translate(new Vector3(0, 0, -speed));
        }
        
        // Change cursor mode
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            lockCursor = !lockCursor;
            Cursor.lockState = lockCursor ? CursorLockMode.Locked : CursorLockMode.None;
        }
        
        prevMousePosition = Input.mousePosition;
    }
}
