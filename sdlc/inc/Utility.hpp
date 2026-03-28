#pragma once

#include <filesystem>
#include "SpriteRenderer.hpp"

#include <SDL3/SDL_render.h>

namespace sdlc
{

struct ProbabilityItem
{
    double probability;
    int index;
};

int ProbabilityList(const ProbabilityItem* items, unsigned int itemCount);
bool LoadSpriteDefinitions(SpriteDefinitions &def, const std::filesystem::path &filePath);
std::filesystem::path ResolveRelativeToExe(const std::string& relativePath);
SDL_Texture* LoadTexture(SDL_Renderer* renderer, const std::filesystem::path &filePath);

} // namespace sdlc