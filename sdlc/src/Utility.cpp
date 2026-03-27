#include <vector>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include "Utility.hpp"
#include "Random.hpp"
#include "CSVParser.hpp"
#include "SpriteRenderer.hpp"

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#elif __linux__
#include <unistd.h>
#include <climits>
#endif

namespace sdlc
{
int probabilityList(const ProbabilityItem *items, unsigned int itemCount)
{
    std::vector<ProbabilityItem> probabilities(itemCount);
    for (unsigned int i = 0; i < itemCount; ++i) {
        probabilities[i] = items[i];
    }
    std::ranges::sort(probabilities.begin(), probabilities.end(),
                      [](const ProbabilityItem &a, const ProbabilityItem &b) {
                          return a.probability < b.probability;
                      });
    Random rng;
    for (const auto &[probability, index]: probabilities) {
        if (rng.chance(probability)) {
            return index;
        }
    }
    return -1;
}

bool loadSpriteDefinitions(SpriteDefinitions &def, const std::filesystem::path &filePath)
{
    bool result = true;
    SDL_Rect rect;
    std::string name;
    try {
        csv::parse(filePath, ',', [&](const csv::CellData &cdata) {
            switch (cdata.index()) {
                case 0:
                    name = cdata.value();
                    break;
                case 1:
                    rect.x = cdata.value<int>();
                    break;
                case 2:
                    rect.y = cdata.value<int>();
                    break;
                case 3:
                    rect.w = cdata.value<int>();
                    break;
                case 4:
                    rect.h = cdata.value<int>();
                    if (def.contains(name)) {
                        std::cerr << "Error parsing file: '" << filePath.filename() << "'" << std::endl;
                        std::cerr << "Line: " << cdata.line() << " at cell: " << cdata.index()
                                  << "Element: '" << name << "' already existing!" << std::endl;
                        result = false;
                        return;
                    }
                    def[name] = rect;
                    break;
            }
        });
    } catch (const std::exception &e) {
        std::cerr << "Error parsing file: '" << filePath.filename() << "'" << std::endl;
        std::cerr << e.what() << '\n';
        result = false;
    }
    return result;
}

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
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';
    return std::filesystem::path(buffer);
#endif
}

std::filesystem::path resolveRelativeToExe(const std::string &relativePath)
{
    std::filesystem::path exePath = getExecutablePath();
    std::filesystem::path exeDir = exePath.parent_path();
    return std::filesystem::canonical(exeDir / relativePath);
}
} // namespace sdlc
