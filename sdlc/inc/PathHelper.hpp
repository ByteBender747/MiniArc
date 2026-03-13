#pragma once

#include <filesystem>

namespace sdl
{

std::filesystem::path resolveRelativeToExe(const std::string& relativePath);

} // namespace sdl