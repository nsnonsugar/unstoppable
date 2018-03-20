#include <stdint.h>
#include <GLES/gl.h>

#include "platform_dependent.h"
#include "draw_thread.h"

namespace nonsugar{

void OnAppCmd(android_app *app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW: {
            std::shared_ptr<InitMsg> msg = std::make_shared<InitMsg>();
            msg->event_id = static_cast<int32_t>(nonsugar::DrawEventId::kInitDisplay);
            msg->app = app;
            SendMsg(MsgQueueIdDraw, msg);
            break;
        }

        default:
            break;
    }
}

int32_t OnInput(android_app *app, AInputEvent *event) {
    (void) app;
    (void) event;

    auto msg = std::make_shared<ThreadMsg>();
    msg->event_id = static_cast<int32_t>(nonsugar::DrawEventId::kStoppable);
    SendMsg(MsgQueueIdDraw, msg);
    return 0;
}

void InitDisplay(void *platform_data, EGLDisplay *out_display, EGLSurface *out_surface) {
     auto app = static_cast<android_app*>(platform_data);

    const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, 0, 0);

    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

    EGLSurface surface = eglCreateWindowSurface(display, config, app->window, nullptr);
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        return;
    }

    EGLint w, h;
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    *out_display = display;
    *out_surface = surface;

    glViewport(0, 0, w, h);

    return;
}

void PostFrontBuffer(EGLDisplay display, EGLSurface surface)
{
    eglSwapBuffers(display, surface);
}

} //namespace nonsugar