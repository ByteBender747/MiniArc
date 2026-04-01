#include <fstream>
#include <string>
#include <cstring>
#include <string_view>

#include "ConfigFile.hpp"

namespace sdlc
{

// Helper function to trim whitespace from both ends of a string
static std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

static bool caseInsensitiveEqual(std::string_view s1, std::string_view s2)
{
    return strcasecmp(s1.data(), s2.data()) == 0;
}

ConfigFile::ConfigFile(const std::filesystem::path &filePath)
{
    read(filePath);
}

bool ConfigFile::read(const std::filesystem::path &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    m_configItems.clear();
    std::string currentSection;
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        // Check if it's a section header
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            currentSection = trim(currentSection);
            continue;
        }

        // Parse key=value pairs
        size_t equalPos = line.find('=');
        if (equalPos != std::string::npos) {
            std::string key = trim(line.substr(0, equalPos));
            std::string value = trim(line.substr(equalPos + 1));

            Item item;
            item.section = currentSection;
            item.keyValue = std::make_pair(key, value);
            m_configItems.push_back(item);
        }
    }

    return true;
}

bool ConfigFile::write(const std::filesystem::path &filePath)
{
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::string currentSection;

    for (const auto& item : m_configItems) {
        // Write section header if section changed
        if (item.section != currentSection) {
            if (!currentSection.empty())
                file << "\n"; // Add blank line between sections

            if (!item.section.empty())
                file << "[" << item.section << "]\n";

            currentSection = item.section;
        }

        // Write key=value pair
        file << item.keyValue.first << "=" << item.keyValue.second << "\n";
    }

    return true;
}

void ConfigFile::setValue(std::string_view section, std::string_view key, std::string_view value)
{
    // Try to find existing entry and update it
    for (auto& item : m_configItems) {
        if (item.section == section && item.keyValue.first == key) {
            item.keyValue.second = value;
            return;
        }
    }

    // If not found, add new entry
    Item item;
    item.section = section;
    item.keyValue = std::make_pair(std::string(key), std::string(value));
    m_configItems.push_back(item);
}

bool ConfigFile::getValue(std::string &out, std::string_view section, std::string_view key)
{
    for (const auto& item : m_configItems) {
        if (m_ignoreCase) {
            if (caseInsensitiveEqual(item.section, section) && caseInsensitiveEqual(item.keyValue.first, key)) {
                out = item.keyValue.second;
                return true;
            }
        } else {
            if (item.section == section && item.keyValue.first == key) {
                out = item.keyValue.second;
                return true;
            }
        }
    }
    return false;
}

bool ConfigFile::exists(std::string_view section, std::string_view key)
{
    for (const auto& item : m_configItems) {
        if (m_ignoreCase) {
            if (caseInsensitiveEqual(item.section, section) && caseInsensitiveEqual(item.keyValue.first, key)) {
                return true;
            }
        } else {
            if (item.section == section && item.keyValue.first == key) {
                return true;
            }
        }
    }
    return false;
}

bool ConfigFile::exists(std::string_view key)
{
    if (m_hasCurrentSection) {
        return exists(m_currentSection, key);
    }
    return false;
}

bool ConfigFile::getValue(std::string &out, std::string_view key)
{
    if (m_hasCurrentSection) {
        return getValue(out, m_currentSection, key);
    }
    return false;
}

void ConfigFile::setValue(std::string_view key, std::string_view value)
{
    if (m_hasCurrentSection) {
        setValue(m_currentSection, key, value);
    }
}

void ConfigFile::beginSection(std::string_view section)
{
    m_currentSection = section;
    m_hasCurrentSection = true;
}

void ConfigFile::endSection()
{
    m_currentSection = std::string();
    m_hasCurrentSection = false;
}

} // namespace sdlc
