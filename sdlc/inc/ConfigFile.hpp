#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <sstream>
#include <utility>
#include <vector>

namespace sdlc
{

class ConfigFile
{
public:
    ConfigFile() = default;
    ConfigFile(const std::filesystem::path &filePath);
    bool read(const std::filesystem::path &filePath);
    bool write(const std::filesystem::path &filePath);
    void beginSection(std::string_view section);
    void endSection();
    bool exists(std::string_view key);
    bool exists(std::string_view section, std::string_view key);
    void setValue(std::string_view key, std::string_view value);
    void setValue(std::string_view section, std::string_view key, std::string_view value);
    bool getValue(std::string &out, std::string_view key);
    bool getValue(std::string &out, std::string_view section, std::string_view key);
    template<typename value_type> 
    bool getValue(value_type &out, std::string_view section, std::string_view key) {
        value_type val;
        std::string sval;
        if (getValue(sval, section, key)) {
            std::istringstream ss(sval);
            ss >> val;
            if (!ss.fail()) {
                out = val;
                return true;
            }
        }
        return false;
    }
    template<typename value_type> 
    bool getValue(value_type &out, std::string_view key) {
        if (m_hasCurrentSection) {
            return getValue<value_type>(out, m_currentSection, key);
        }
        return false;
    }
    void setIgnoreCase(bool value) { m_ignoreCase = value; }
    [[nodiscard]] bool ignoreCase() const { return m_ignoreCase; }
protected:
    struct Item {
        std::string section;
        std::pair<std::string, std::string> keyValue;
    };
private:
    bool m_ignoreCase{false};
    bool m_hasCurrentSection{false};
    std::string m_currentSection;
    std::vector<Item> m_configItems;
};

} // namespace sdlc