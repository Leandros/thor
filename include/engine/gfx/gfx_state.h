/*!
 * \file gfx_state.h
 * \author Arvid Gerstmann
 * \date Nov 2016
 * \brief Contains the graphics state.
 * \copyright Copyright (c) 2016, Arvid Gerstmann. All rights reserved.
 */

#ifndef GFX_STATE_H
#define GFX_STATE_H

/* Contains gfx_device decleration for D3D11. */
#include <gfx/d3d11/gfx_d3d11.h>
#include <gfx/d3d11/gfx_d3d11_state.h>

struct gfx_state {
    struct gfx_device *device;
    struct gfx_resources *res;
    struct gfx_cmd_state *rqueue;
};

#endif /* GFX_STATE_H */

