using UnityEngine;

namespace LOTN
{
    public sealed class LOTNTouchInput : MonoBehaviour
    {
        public static Vector2 Move { get; private set; }
        public static Vector2 Look { get; private set; }
        public static bool Sprint { get; private set; }
        public static bool InteractPressed { get; private set; }

        private int moveFinger = -1;
        private int lookFinger = -1;
        private Vector2 moveOrigin;
        private Vector2 lookPrevious;
        private const float Radius = 105f;

        private void Update()
        {
            Look = Vector2.zero;
            InteractPressed = false;
            Sprint = false;
            if (Input.touchCount == 0) { Move = Vector2.zero; moveFinger = -1; lookFinger = -1; return; }

            for (int i = 0; i < Input.touchCount; i++)
            {
                Touch t = Input.GetTouch(i);
                Vector2 p = t.position;
                if (t.phase == TouchPhase.Began)
                {
                    if (p.x < Screen.width * 0.48f && p.y < Screen.height * 0.72f && moveFinger < 0)
                    { moveFinger = t.fingerId; moveOrigin = p; }
                    else if (p.x >= Screen.width * 0.48f && p.y < Screen.height * 0.88f && lookFinger < 0)
                    { lookFinger = t.fingerId; lookPrevious = p; }
                }

                if (t.fingerId == moveFinger)
                {
                    Vector2 d = (p - moveOrigin) / Radius;
                    Move = Vector2.ClampMagnitude(d, 1f);
                    if (t.phase == TouchPhase.Ended || t.phase == TouchPhase.Canceled) { moveFinger = -1; Move = Vector2.zero; }
                }
                if (t.fingerId == lookFinger)
                {
                    if (t.phase == TouchPhase.Moved) Look += (p - lookPrevious) * 0.12f;
                    lookPrevious = p;
                    if (t.phase == TouchPhase.Ended || t.phase == TouchPhase.Canceled) lookFinger = -1;
                }
                if (p.x > Screen.width * 0.72f && p.y > Screen.height * 0.78f && t.phase == TouchPhase.Stationary) Sprint = true;
                if (p.x > Screen.width * 0.84f && p.y > Screen.height * 0.58f && t.phase == TouchPhase.Began) InteractPressed = true;
            }
        }

        private void OnGUI()
        {
            if (Event.current.type != EventType.Repaint) return;
            float s = Mathf.Clamp(Screen.width / 900f, 0.7f, 1.25f);
            GUI.color = new Color(0.9f, 0.95f, 1f, 0.28f);
            GUI.Box(new Rect(28*s, Screen.height - 190*s, 150*s, 150*s), "MOVE");
            GUI.Box(new Rect(Screen.width - 205*s, Screen.height - 150*s, 170*s, 105*s), "SPRINT");
            GUI.Box(new Rect(Screen.width - 115*s, Screen.height - 275*s, 85*s, 85*s), "USE");
            GUI.color = Color.white;
        }
    }
}
