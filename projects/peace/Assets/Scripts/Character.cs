using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Character : MonoBehaviour
{
    public const float MAX_SPEED_FACTOR = 4f;
    public const float MIN_SPEED_FACTOR = 0f;
    public const float REF_SPEED = 1f;

    public float sensitivity = 1.0f;
    public float speedFactor = 3f;

    public Canvas ui;
    public Slider speedSlider;
    public Text positionText;

    private bool inGame;

    // Start is called before the first frame update
    void Start()
    {
        SetInGame(true);

        if (speedSlider)
        {
            speedSlider.minValue = MIN_SPEED_FACTOR;
            speedSlider.maxValue = MAX_SPEED_FACTOR;
            speedSlider.value = speedFactor;
        }
    }

    public void SetSpeedExponent(float exp)
    {
        speedFactor = Mathf.Clamp(exp, MIN_SPEED_FACTOR, MAX_SPEED_FACTOR);
        if (speedSlider)
        {
            speedSlider.value = speedFactor;
        }
    }

    public void ToggleInGame()
    {
        SetInGame(!inGame);
    }

    public void SetInGame(bool inGame)
    {
        this.inGame = inGame;
        Cursor.lockState = inGame ? CursorLockMode.Locked : CursorLockMode.None;

        if (ui)
        {
            ui.gameObject.SetActive(!inGame);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (inGame)
        {
            // Adapt speed
            SetSpeedExponent(speedFactor + Input.mouseScrollDelta.y * 0.1f);

            // view rotation
            Vector2 deltaPos = new Vector2(Input.GetAxisRaw("Mouse X"), Input.GetAxisRaw("Mouse Y"));
            transform.Rotate(new Vector3(0, deltaPos.x * sensitivity, 0), Space.World);
            transform.Rotate(new Vector3(-deltaPos.y * sensitivity, 0, 0), Space.Self);

            // Moving
            float speed = REF_SPEED * Mathf.Pow(10, speedFactor);
            float step = speed * Time.deltaTime;

            float forward = Input.GetAxisRaw("Vertical");
            float side = Input.GetAxisRaw("Horizontal");
            Vector3 moving = new Vector3(side, 0, forward);
            moving.Normalize();
            transform.Translate(moving * step);
        }
        
        // Change in game mode
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            ToggleInGame();
        }
        if (Input.GetKeyDown(KeyCode.F1)) {
            Debug.Log("Screenshot!");
            ScreenCapture.CaptureScreenshot("screenshot.png");
        }

        if (positionText)
        {
            positionText.text = transform.position.ToString();
        }
    }
}
