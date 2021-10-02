#include <engine.h>
#include <mem/re_mem.h>
#include <sys/sys_windows.h>
#include <gfx/gfx_renderdoc.h>
#include <renderdoc/renderdoc_app.h>

struct gfx_renderdoc {
    HMODULE dll;
    RENDERDOC_API_1_0_0 *api;
};

int
gfx_renderdoc_attached(void)
{
    return GetModuleHandleA("renderdoc") != NULL;
}

struct gfx_renderdoc *
gfx_renderdoc_get(void)
{
    HMODULE dll;
    RENDERDOC_API_1_0_0 *api;
    int (*get_api)(RENDERDOC_Version, void **);
    struct gfx_renderdoc *ctx;

    /* ==== Initialize RenderDoc API ==== */
    if ((dll = GetModuleHandleA("renderdoc")) == NULL)
        return NULL;

    get_api = (int (*)(RENDERDOC_Version, void **))
        GetProcAddress(dll, "RENDERDOC_GetAPI");
    if (get_api == NULL)
        return ELOG(("RenderDoc: GetProcAddress() FAILED")), NULL;

    if (!get_api(eRENDERDOC_API_Version_1_0_0, &api))
        return ELOG(("RenderDoc: get_api() FAILED")), NULL;

    /* ==== Setup ==== */
    api->UnloadCrashHandler();


    ctx = mem_malloc(sizeof(struct gfx_renderdoc));
    ctx->dll = dll;
    ctx->api = api;
    return ctx;
}

void
gfx_renderdoc_capture(struct gfx_renderdoc *ctx)
{
    if (!ctx)
        return;

    ctx->api->TriggerCapture();
}

