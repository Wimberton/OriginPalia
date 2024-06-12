#include "OpenGLHook.h"
#include "WindowsHook.h"
#include "../RendererDetector.h"
#include "../OverlayBase.h"
#include "../ImGui/imgui.h"
#include "../ImGui/impls/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include "../Macros.h"

OpenGLHook *OpenGLHook::_inst = nullptr;

bool OpenGLHook::StartHook() {
    bool res = true;
    if (!hooked) {
        if (!WindowsHook::Instance()->StartHook())
            return false;

        GLenum err = glewInit();

        if (err == GLEW_OK) {
            PRINT_DEBUG("Hooked OpenGL\n");

            hooked = true;
            RendererDetector::Instance().RendererFound(this);

            UnhookAll();
            BeginHook();
            HookFuncs(std::make_pair<void **, void *>(&(PVOID &)wglSwapBuffers, &OpenGLHook::MywglSwapBuffers));
            EndHook();

            OverlayBase::Instance->HookReady();
        } else {
            PRINT_DEBUG("Failed to hook OpenGL\n");
            /* Problem: glewInit failed, something is seriously wrong. */
            PRINT_DEBUG("Error: %s\n", glewGetErrorString(err));
            res = false;
        }
    }
    return true;
}

void OpenGLHook::ResetRenderState() {
    if (initialized) {
        ImGui_ImplOpenGL3_Shutdown();
        WindowsHook::Instance()->ResetRenderState();
        ImGui::DestroyContext();

        initialized = false;
    }
}

// Try to make this function and overlay's proc as short as possible or it might
// affect game's fps.
void OpenGLHook::PrepareForOverlay(HDC hDC) {
    HWND hWnd = WindowFromDC(hDC);

    if (hWnd != WindowsHook::Instance()->GetGameHwnd())
        ResetRenderState();

    if (!initialized) {
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.IniFilename = NULL;

        ImGui_ImplOpenGL3_Init();

        OverlayBase::Instance->CreateFonts();

        initialized = true;
    }

    if (ImGui_ImplOpenGL3_NewFrame()) {
        WindowsHook::Instance()->PrepareForOverlay(hWnd);

        ImGui::NewFrame();

        OverlayBase::Instance->OverlayProc();

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

BOOL WINAPI OpenGLHook::MywglSwapBuffers(HDC hDC) {
    OpenGLHook::Instance()->PrepareForOverlay(hDC);
    return OpenGLHook::Instance()->wglSwapBuffers(hDC);
}

OpenGLHook::OpenGLHook() : initialized(false), hooked(false), wglSwapBuffers(nullptr) {
    _library = LoadLibrary(OPENGL_DLL);
}

OpenGLHook::~OpenGLHook() {
    PRINT_DEBUG("OpenGL Hook removed\n");

    if (initialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
    }

    FreeLibrary(reinterpret_cast<HMODULE>(_library));

    _inst = nullptr;
}

OpenGLHook *OpenGLHook::Instance() {
    if (_inst == nullptr)
        _inst = new OpenGLHook;

    return _inst;
}

const char *OpenGLHook::GetLibName() const { return OPENGL_DLL; }

void OpenGLHook::LoadFunctions(wglSwapBuffers_t pfnwglSwapBuffers) { wglSwapBuffers = pfnwglSwapBuffers; }
