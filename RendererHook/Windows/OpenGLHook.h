#pragma once
#include <Windows.h>
#include "../BaseHook.h"
class OpenGLHook : public BaseHook
{
public:
#define OPENGL_DLL "opengl32.dll"

    using wglSwapBuffers_t = BOOL(WINAPI*)(HDC);

    virtual ~OpenGLHook();

    bool StartHook();
    static OpenGLHook* Instance();
    virtual const char* GetLibName() const;
    void LoadFunctions(wglSwapBuffers_t pfnwglSwapBuffers);

private:
    OpenGLHook();

    void ResetRenderState();
    void PrepareForOverlay(HDC hDC);

    // Hook to render functions
    static BOOL WINAPI MywglSwapBuffers(HDC hDC);

    wglSwapBuffers_t wglSwapBuffers;

private:
    static OpenGLHook* _inst;

    // Variables
    bool hooked;
    bool initialized;
};

