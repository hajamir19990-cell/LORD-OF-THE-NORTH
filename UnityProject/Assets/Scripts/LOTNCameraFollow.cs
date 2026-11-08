using UnityEngine;

namespace LOTN
{
    public sealed class LOTNCameraFollow : MonoBehaviour
    {
        public Transform target;
        public Vector3 offset = new Vector3(0f, 4.2f, -6.5f);
        public float lookHeight = 1.5f;
        public float sensitivity = 2.2f;
        private float yaw;
        private float pitch = 18f;

        private void LateUpdate()
        {
            if (target == null) return;
            Vector2 look = LOTNTouchInput.Look;
            yaw += look.x * sensitivity;
            pitch = Mathf.Clamp(pitch - look.y * sensitivity, 5f, 48f);
            if (Input.GetMouseButton(1))
            {
                yaw += Input.GetAxis("Mouse X") * 3f;
                pitch = Mathf.Clamp(pitch - Input.GetAxis("Mouse Y") * 3f, 5f, 48f);
            }
            Quaternion rotation = Quaternion.Euler(pitch, yaw, 0f);
            Vector3 desired = target.position + rotation * offset;
            if (Physics.Linecast(target.position + Vector3.up * lookHeight, desired, out RaycastHit hit, ~0, QueryTriggerInteraction.Ignore))
                desired = hit.point + hit.normal * 0.2f;
            transform.position = desired;
            transform.rotation = Quaternion.LookRotation(target.position + Vector3.up * lookHeight - transform.position, Vector3.up);
        }
    }
}
