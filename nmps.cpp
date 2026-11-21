#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace std;
namespace fs = std::filesystem;

#include "Shared/nymera_lib.h" 

// ###
// Platform Declarations
// ###
bool platform_create_window(int width, int height, const char* title);
void platform_update_window();
bool platform_is_running();

// ###
// Program Manager Declarations
// ###
fs::path exeFolder();
fs::path findFile(const string& folderName, const string& fileName);

void BootPrograms();

bool BuildProgram(const fs::path& cpp);
void RunProgram(const fs::path& cpp);
void LaunchProgram(const fs::path& address);


void platform_set_button_callback(void (*callback)());

// ###
// Program Globals
// ###
fs::path address_nsc;

// ###
// Program Manager
// ###

// Locate NMPS executable folder
fs::path exeFolder()
{
    char buf[MAX_PATH];

    GetModuleFileNameA(NULL, buf, MAX_PATH);

    return fs::path(buf).parent_path();
}

// Locate a specific program
fs::path findFile(const string& folderName, const string& fileName)
{
    fs::path p = exeFolder() / "Programs" / folderName / fileName;

    if (fs::exists(p)) return p;

    return fs::path();
}

// Find available programs
void BootPrograms()
{
    address_nsc = findFile("NSC", "nsc.cpp");

    if (!address_nsc.empty()) cout << "NSC - Booted" << endl;
    else cout << "NSC - Not Found" << endl;
}

// Build program
bool BuildProgram(const fs::path& cpp)
{
    fs::path exe = cpp;
    exe.replace_extension(".exe");

    string cmd = "g++ -g -std=gnu++17 \"" +
                 cpp.string() + "\" -o \"" +
                 exe.string() + "\"";

    return system(cmd.c_str()) == 0;
}

// Run program
void RunProgram(const fs::path& cpp)
{
    fs::path exe = cpp;
    exe.replace_extension(".exe");

    string cmd = "start \"\" \"" + exe.string() + "\"";

    system(cmd.c_str());
}

// Launch program
void LaunchProgram(const fs::path& address)
{
    if (address.empty()) {
        cout << "Not found." << endl;
        return;
    }

    cout << "Building..." << endl;

    if (BuildProgram(address)) {
        RunProgram(address);
    }
    else {
        cout << "Build failed." << endl;
    }
}

void launch_nsc_button()
{
    LaunchProgram(address_nsc);
    SM_TRACE("NSC - Launched");
}

// ###
// Main
// ###
int main()
{
    platform_set_button_callback(launch_nsc_button);

    if (!platform_create_window(1200, 720, "NMPS")) return 1;

    BootPrograms();

    while (platform_is_running()) {
        platform_update_window();
    }

    return 0;
}
