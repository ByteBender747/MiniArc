#pragma once

#include <filesystem>
#include "SpriteRenderer.hpp"

namespace sdlc
{

struct ProbabilityItem
{
    double probability;
    int index;
};

int probabilityList(const ProbabilityItem* items, unsigned int itemCount);
bool loadSpriteDefinitions(SpriteDefinitions &def, const std::filesystem::path &filePath);
std::filesystem::path resolveRelativeToExe(const std::string& relativePath);

} // namespace sdlc