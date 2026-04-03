
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <cstdint>
#include <vector>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include "Utility.hpp"
#include "Random.hpp"
#include "CSVParser.hpp"
#include "ResPtr.hpp"
#include "SpriteRenderer.hpp"
#include "MathFunc.hpp"

#include <SDL3/SDL.h>

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

int ProbabilityList(const ProbabilityItem *items, unsigned int itemCount)
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

std::filesystem::path GetSaveGameFolder(const char* orgName, const char* gameName)
{
    char* folder = SDL_GetPrefPath(orgName, gameName);
    std::filesystem::path result = folder;
    SDL_free(folder);
    return result;
}

void RenderEllipse(SDL_Renderer *renderer, float x, float y, float r1, float r2, int slices)
{
    float phase{0};
    SDL_FPoint points[361];
    slices = clamp<int>(slices, 1, 360);
    for (int i = 0; i < slices + 1; ++i) {
        points[i].x = x + r1 * SDL_cosf(phase);
        points[i].y = y + r2 * SDL_sinf(phase);
        phase += static_cast<float>(tau) / static_cast<float>(slices);
    }
    SDL_RenderLines(renderer, points, slices + 1);
}

SDL_Texture* LoadTexture(SDL_Renderer* renderer, const std::filesystem::path &filePath)
{
    std::string tmpfilePath = filePath.string();
    ResPtr<SDL_Surface> surface;
    SDL_Texture* texture = nullptr;
    if (filePath.extension() == ".bmp") {
        SDL_Log("Loading BMP texture file: %s", tmpfilePath.c_str());
        surface = SDL_LoadBMP(filePath.string().c_str());
    } else if (filePath.extension() == ".png") {
        SDL_Log("Loading PNG texture file: %s", tmpfilePath.c_str());
        surface = SDL_LoadPNG(filePath.string().c_str());
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Unsupported image format for file: %s", tmpfilePath.c_str());
    }
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL_CreateTextureFromSurface() failed: %s", SDL_GetError());
        }
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to load image file: %s", tmpfilePath.c_str());
        SDL_LogTrace(SDL_LOG_CATEGORY_SYSTEM, "SDL_GetError(): %s", SDL_GetError());
    }
    return texture;
}

bool LoadSpriteDefinitions(SpriteDefinitions &def, const std::filesystem::path &filePath)
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

static std::filesystem::path GetExecutablePath()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::filesystem::path(buffer);
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

std::filesystem::path ResolveRelativeToExe(const std::filesystem::path &relativePath)
{
    std::filesystem::path exePath = GetExecutablePath();
    std::filesystem::path exeDir = exePath.parent_path();
    return std::filesystem::canonical(exeDir / relativePath);
}

} // namespace sdlc
