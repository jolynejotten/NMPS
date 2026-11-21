#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// ###
// Platform Globals
// ###
static bool running = true;
static HWND window = NULL;
static void (*button_callback)() = nullptr;

void platform_set_button_callback(void (*callback)()) {
    button_callback = callback;
}

// ###
// Platform Declarations
// ###
bool platform_create_window(int width, int height, const char* title);
void platform_update_window();
bool platform_is_running();

// ###
// Windows Platform
// ###
#define ID_BUTTON_TEST 1

LRESULT CALLBACK windows_window_callback(
    HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {

        case WM_CREATE: {
            CreateWindowA(
                "BUTTON", "Click me",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                20, 20, 120, 30,
                window,
                (HMENU)(INT_PTR)ID_BUTTON_TEST,
                GetModuleHandleA(0),
                NULL
            );
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_BUTTON_TEST) {
                if (button_callback) {
                    button_callback();
                }
            }
            return 0;
        }

        case WM_CLOSE: {
            DestroyWindow(window);
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProcA(window, msg, wParam, lParam);
    }
}

// ###
// Create Window
// ###
bool platform_create_window(int width, int height, const char* title)
{
    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc = {};
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = title;
    wc.lpfnWndProc = windows_window_callback;

    if (!RegisterClassA(&wc)) return false;

    window = CreateWindowExA(
        0,
        title,
        title,
        WS_OVERLAPPEDWINDOW,
        100, 100,
        width, height,
        NULL, NULL,
        instance,
        NULL
    );

    if (window == NULL)
        return false;

    ShowWindow(window, SW_SHOW);

    return true;
}

// ###
// Update Window
// ###
void platform_update_window()
{
    MSG msg;

    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {

        if (msg.message == WM_QUIT) {
            running = false;
            break;
        }

        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

// ###
// Check Running State
// ###
bool platform_is_running() {
    return running;
}

