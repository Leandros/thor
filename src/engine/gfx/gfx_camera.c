#include <config.h>
#include <libc/stdint.h>
#include <dbg/dbg_assert.h>
#include <dbg/dbg_log.h>
#include <gfx/gfx_camera.h>
#include <math/vec3.h>
#include <math.h>

#define DEGREES(rad)        (((rad) * 180.0f) / (float)M_PI)
#define RADIANS(deg)        (((float)M_PI * (deg)) / 180.0f)

void
gfx_camera_init(struct gfx_camera *cam)
{
    uintptr_t ptr = (uintptr_t)cam;
    dbg_assert((ptr & 15) == 0, "camera pointer not aligned at 16 bytes");

    memset(cam, 0x0, sizeof(struct gfx_camera));
    cam->up.y = 1.0f;
    cam->front.z = -1.0f;
}

void
gfx_camera_update(struct gfx_camera *cam)
{
    vec3 front;
    front.x = cosf(RADIANS(cam->yaw)) * cosf(RADIANS(cam->pitch));
    front.y = sinf(RADIANS(cam->pitch));
    front.z = sinf(RADIANS(cam->yaw)) * cosf(RADIANS(cam->pitch));
    cam->front = vec3_normalize(front);
    cam->right = vec3_normalize(vec3_cross(cam->front, cam->world_up));
    cam->up = vec3_normalize(vec3_cross(cam->right, cam->front));
}


void
gfx_camera_translate(struct gfx_camera *cam, vec3 v)
{
    cam->position = vec3_add(cam->position, v);
}


/* ========================================================================= */
/* Convenience Helpers                                                       */
/* ========================================================================= */
void
gfx_camera_rotate(struct gfx_camera *cam, float xOff, float yOff)
{
    xOff *= CAMERA_SENSITIVITY;
    yOff *= CAMERA_SENSITIVITY;

    cam->yaw += xOff;
    cam->pitch += yOff;

    if (cam->pitch > (CAMERA_FOV - 1.0f))
        cam->pitch = (CAMERA_FOV - 1.0f);
    if (cam->pitch < -(CAMERA_FOV - 1.0f))
        cam->pitch = -(CAMERA_FOV - 1.0f);

    gfx_camera_update(cam);
}

void
gfx_camera_move(struct gfx_camera *cam, int direction)
{
    float vel = CAMERA_MOVEMENT_SPEED * TICKRATE;
    if ((direction & DIRECTION_FORWARD))
        cam->position = vec3_add(cam->position, vec3_mul(cam->front, vel));
    if ((direction & DIRECTION_BACKWARD))
        cam->position = vec3_sub(cam->position, vec3_mul(cam->front, vel));
    if ((direction & DIRECTION_LEFT))
        cam->position = vec3_sub(cam->position, vec3_mul(cam->right, vel));
    if ((direction & DIRECTION_RIGHT))
        cam->position = vec3_add(cam->position, vec3_mul(cam->right, vel));
}

