#include <iostream>
#include <string>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

#include "nymera_lib.h"

// ###
// Platform Globals
// ###
static bool running = true;

// ###
// Platform Functions
// ###
bool platform_create_window(int width, int height, const char* title);
void platform_update_window();

// ###
// Windows Platform
// ###
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// ###
// Windows Globals
// ###
static HWND window;

// ###
// Platform Implementations
// ###
LRESULT CALLBACK windows_window_callback(HWND window, UINT msg,
                                         WPARAM wParam, LPARAM lParam){
    LRESULT result = 0;

    switch(msg){
        case WM_CLOSE:{
            running = false;
            break;
        }
        default:
            // Let windows handle the default input for now(The tutorial says?)
            result = DefWindowProcA(window, msg, wParam, lParam);
    }

    return result;
}

bool platform_create_window(int width, int height, const char* title){
    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc = {};
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // Look of the cursor (Arrow)
    wc.lpszClassName = title;  // Class ID, Not the actual title. But I'll use it as it is.
    wc.lpfnWndProc = windows_window_callback; // Callback for Input into the Window
    
    if(!RegisterClassA(&wc)) return false;

    // WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
    int dwStyle = WS_OVERLAPPEDWINDOW;

    window = CreateWindowExA(0, title, // This References lpszClassName from wc
                                  title, // This is the actual Title
                                  dwStyle,
                                  100,
                                  100,
                                  width,
                                  height,
                                  NULL, // Parent
                                  NULL, // Menu
                                  instance,
                                  NULL); // lpParam

    if(window == NULL) return false;
    
    ShowWindow(window, SW_SHOW);

    return true;
}

void platform_update_window(){
    MSG msg;

    while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE)){
        TranslateMessage(&msg);
        DispatchMessageA(&msg); // Calls the callback specified when creating the window
    }
}


// Locating the nmps.exe
fs::path exeFolder(){
    char buf[MAX_PATH];
    GetModuleFileNameA(NULL, buf, MAX_PATH);
    return fs::path(buf).parent_path();
}

// Locate the specific program
fs::path findFile(const string& folderName, const string& fileName) {
    fs::path p = exeFolder() / "programs" / folderName / fileName;
    if (fs::exists(p)) return p;
    return fs::path();
}

fs::path address_nsc;

void BootPrograms(){
    //NSC
    address_nsc = findFile("NSC", "nsc.cpp");
    if (!address_nsc.empty()) cout << "NSC - Booted" << endl;
    else cout << "NSC - Not Found" << endl;
}

bool BuildProgram(const fs::path& cpp){
    fs::path exe = cpp;
    exe.replace_extension(".exe");

    string cmd = "\"g++ -std=c++17 \"" + cpp.string() + "\" -o \"" + exe.string() + "\"\"";
    return system(cmd.c_str()) == 0;
}

void RunProgram(const fs::path& cpp){
    fs::path exe = cpp;
    exe.replace_extension(".exe");

    string cmd = "\"\"" + exe.string() + "\"\"";
    system(cmd.c_str());
}

void LaunchProgram(const fs::path& address){
    if (address.empty()) {
        cout << "Not found." << endl;
        return;
    }
    cout << "Building..." << endl;
    if (BuildProgram(address)) RunProgram(address);
    else cout << "Build failed." << endl;
}


int main(){
    platform_create_window(1200, 720, "NMPS");

    BootPrograms();

    while (running) {
        platform_update_window();
        SM_TRACE("Test");

    }
    return 0;
}

#endif


