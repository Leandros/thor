#include <gfx/ogl/GL/gl3w.h>
#include <gfx/ogl/gfx_api.h>
#include <dbg/dbg_log.h>
#include <config.h>

/* SDL2 Variables */
/* SDL_Window *sdl_win; */
/* SDL_GLContext glctx; */

int gfx_init(void)
{
    /* TODO: Do this initialisation in the gfx/ogl package, and package it
     * into a .so / .dll, to get dynamic render backend switching.
     */
    /* make sure we get an OpenGL 3.2 context */
    /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); */
    /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2); */
    /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); */

    /* /1* Double buffering, and 24-bit zbuffer *1/ */
    /* SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); */
    /* SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); */

    /* sdl_win = SDL_CreateWindow(GAME_NAME, */
    /*             GAME_WINPOS_X, GAME_WINPOS_Y, */
    /*             GAME_WINSIZE_X, GAME_WINSIZE_Y, */
    /*             SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI); */
    /* if (sdl_win == NULL) { */
    /*     ELOG(("Error: Creating Window: %s\n", SDL_GetError())); */
    /*     goto err1; */
    /* } */

    /* glctx = SDL_GL_CreateContext(sdl_win); */
    /* if (glctx == NULL) { */
    /*     ELOG(("Error: Creating OpenGL Context: %s\n", SDL_GetError())); */
    /*     goto err2; */
    /* } */

    /* if (gl3wInit()) { */
    /*     ELOG(("Error: Initializing OpenGL function pointers\n")); */
    /*     goto err3; */
    /* } */

    /* if (!gl3wIsSupported(3, 2)) { */
    /*     ELOG(("Error: OpenGL 3.2 not supported\n")); */
    /*     goto err3; */
    /* } */

    /* ILOG(("OpenGL Version: %s\n", glGetString(GL_VERSION))); */
    /* ILOG(("OpenGL Vendor: %s\n", glGetString(GL_VENDOR))); */
    /* ILOG(("OpenGL Renderer: %s\n", glGetString(GL_RENDERER))); */
    /* ILOG(("OpenGL GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION))); */

    /* /1* set background color to black once, and switch buffers. */
    /*  * If not done, the default state is broken on some operating systems. */
    /*  *1/ */
    /* gfx_clearcolor(0.0f, 0.0f, 0.0f, 1.0f); */
    /* gfx_clear(); */
    /* gfx_swapbuffer(); */

    /* return 0; */

/* err3: */
    /* SDL_GL_DeleteContext(glctx); */
/* err2: */
    /* SDL_DestroyWindow(sdl_win); */
/* err1: */
    return 1;
}

void gfx_shutdown(void)
{
    /* SDL_GL_DeleteContext(glctx); */
    /* SDL_DestroyWindow(sdl_win); */
}


void gfx_clearcolor(f32 r, f32 g, f32 b, f32 a)
{
    /* glClearColor(r, g, b, a); */
}

void gfx_clear(void)
{
    /* glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); */
}

void gfx_set_swapinterval(int interval)
{
    /* SDL_GL_SetSwapInterval(interval); */
}

void gfx_swapbuffer(void)
{
    /* SDL_GL_SwapWindow(sdl_win); */
}
