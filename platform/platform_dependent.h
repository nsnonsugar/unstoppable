#ifndef platform_dependent_h_
#define platform_dependent_h_
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#include "Message.h"

namespace nonsugar {

struct InitMsg : ThreadMsg{
    android_app *app;
};

int32_t OnInput(android_app *app, AInputEvent *event);

void OnAppCmd(android_app *app, int32_t cmd);

void InitDisplay(void *platform_data, EGLDisplay *out_display, EGLSurface *out_surface);

void PostFrontBuffer(EGLDisplay display, EGLSurface surface);
} //namespace nonsugar

#endif /* platform_dependent_h_ */