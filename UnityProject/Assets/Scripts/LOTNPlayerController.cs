using UnityEngine;

namespace LOTN
{
    [RequireComponent(typeof(CharacterController))]
    public sealed class LOTNPlayerController : MonoBehaviour
    {
        public float walkSpeed = 3.4f;
        public float sprintSpeed = 5.4f;
        public float turnSpeed = 12f;
        public float gravity = -22f;
        private CharacterController controller;
        private LOTNPlayerStats stats;
        private Vector3 verticalVelocity;

        private void Awake()
        {
            controller = GetComponent<CharacterController>();
            stats = GetComponent<LOTNPlayerStats>();
        }

        private void Update()
        {
            Vector2 input = LOTNTouchInput.Move;
            if (input.sqrMagnitude < 0.01f)
                input = new Vector2(Input.GetAxisRaw("Horizontal"), Input.GetAxisRaw("Vertical"));
            input = Vector2.ClampMagnitude(input, 1f);

            Transform cam = Camera.main != null ? Camera.main.transform : transform;
            Vector3 forward = Vector3.ProjectOnPlane(cam.forward, Vector3.up).normalized;
            Vector3 right = Vector3.ProjectOnPlane(cam.right, Vector3.up).normalized;
            Vector3 move = forward * input.y + right * input.x;

            bool sprinting = (LOTNTouchInput.Sprint || Input.GetKey(KeyCode.LeftShift)) && input.sqrMagnitude > 0.05f;
            float speed = sprinting && (stats == null || stats.SpendStamina(1)) ? sprintSpeed : walkSpeed;
            if (!sprinting && stats != null && Time.frameCount % 6 == 0) stats.RestoreStamina(1);

            if (move.sqrMagnitude > 0.01f)
            {
                Quaternion target = Quaternion.LookRotation(move, Vector3.up);
                transform.rotation = Quaternion.Slerp(transform.rotation, target, turnSpeed * Time.deltaTime);
            }
            if (controller.isGrounded && verticalVelocity.y < 0) verticalVelocity.y = -2f;
            verticalVelocity.y += gravity * Time.deltaTime;
            controller.Move((move.normalized * speed + verticalVelocity) * Time.deltaTime);
        }
    }
}
