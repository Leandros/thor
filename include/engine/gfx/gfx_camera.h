/*!
 * \file gfx_camera.h
 * \author Arvid Gerstmann
 * \date Nov 2016
 * \brief Camera.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_CAMERA_H
#define GFX_CAMERA_H

#include <math/vec3.h>
#include <math/mat4.h>
#include <sys/sys_macros.h>

/*!
 * \brief Camera structure.
 * \warning \b Has to be aligned at 16-bytes boundaries!
 */
struct gfx_camera {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;

    float yaw;
    float pitch;

    /* To keep alignment. */
    float __[2];
};


/* ========================================================================= */
/* Constants                                                                 */
/* ========================================================================= */
#define CAMERA_MOVEMENT_SPEED   20.0f
#define CAMERA_SENSITIVITY      0.25f
#define CAMERA_FOV              90.0f

/*!
 * \def DIRECTION_FORWARD
 * \def DIRECTION_BACKWARD
 * \def DIRECTION_LEFT
 * \def DIRECTION_RIGHT
 * \brief Movement direction bitmask
 */
#define DIRECTION_FORWARD       0x1
#define DIRECTION_BACKWARD      0x2
#define DIRECTION_LEFT          0x4
#define DIRECTION_RIGHT         0x8


/*!
 * \brief Initialize camera.
 * \param camera Pointer to camera structure.
 */
void
gfx_camera_init(struct gfx_camera *camera);

/*
 * \brief Update all internally translated / rotated or otherwise modified states.
 * \param cam Pointer to camera structure.
 */
void
gfx_camera_update(struct gfx_camera *cam);

/*!
 * \brief Translate the camera by \c v.
 * \param camera Pointer to camera structure.
 * \param v Vector3 by which the camera is translated.
 */
void
gfx_camera_translate(struct gfx_camera *camera, vec3 v);

/*!
 * \brief Calculate the look-at matrix.
 * \param camera Pointer to camera structure.
 * \return Look-At Matrix
 */
INLINE mat4
gfx_camera_view(struct gfx_camera *cam)
{
    vec3 forward = vec3_add(cam->position, cam->front);
    return mat4_lookat(cam->position, forward, cam->up);
}

/*!
 * \brief Calculate the projection matrix.
 * \param cam Pointer to camera structure.
 * \return Projection-Matrix.
 */
INLINE mat4
gfx_camera_proj(struct gfx_camera *cam)
{
    /* TODO: Remove hardcoded values. */
    float aspect = 1280.0f / 720.0f;
    return mat4_perspective(90.0f, aspect, 0.1f, 300.0f);
}


/* ========================================================================= */
/* Convenience Helpers                                                       */
/* ========================================================================= */
/*!
 * \brief Rotate the camera.
 * \param camera Pointer to camera structure.
 * \param xOff Offset of mouse x-position
 * \param yOff Offset of mouse y-position
 */
void
gfx_camera_rotate(struct gfx_camera *camera, float xOff, float yOff);

/*!
 * \brief Move the camera with a pre-defined speed.
 * \param camera Pointer to camera structure.
 * \param direction Direction of movement, one of DIRECTION_XXX
 * \remark Very simple and easy to use helper to move the camera, use
 *         gfx_camera_translate() for full control.
 */
void
gfx_camera_move(struct gfx_camera *camera, int direction);

#endif /* GFX_CAMERA_H */

