#pragma once

#include <filesystem>

namespace sdlc
{

std::filesystem::path resolveRelativeToExe(const std::string& relativePath);

} // namespace sdlc