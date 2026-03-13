#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#endif

namespace sdl
{

static std::filesystem::path getExecutablePath()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return fs::path(buffer);
#elif __APPLE__
    char buffer[PATH_MAX];
    uint32_t size = PATH_MAX;
    _NSGetExecutablePath(buffer, &size);
    return fs::canonical(fs::path(buffer));
#elif __linux__
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    buffer[len] = '\0';
    return std::filesystem::path(buffer);
#endif
}

std::filesystem::path resolveRelativeToExe(const std::string& relativePath)
{
    std::filesystem::path exePath = getExecutablePath();
    std::filesystem::path exeDir = exePath.parent_path();
    return std::filesystem::canonical(exeDir / relativePath);
}

} // namespace sdl