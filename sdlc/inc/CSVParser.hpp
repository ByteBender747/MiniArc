#pragma once

#include <filesystem>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>

namespace sdlc::csv {

class CellData {
public:
    CellData(const std::string& data, int line, int index)
        : m_data(data)
        , m_line(line)
        , m_index(index) {
    }
    template <typename TValue>
    TValue value() const {
        TValue result {};
        std::istringstream is(m_data);
        is >> result;
        if (is.fail()) {
            std::stringstream ss;
            ss << "Invalid value in line: " << m_line << " at cell: " << m_index;
            throw std::invalid_argument(ss.str());
        }
        return result;
    }
    [[nodiscard]] int line() const {
        return m_line;
    }
    [[nodiscard]] int index() const {
        return m_index;
    }
    [[nodiscard]] const std::string& value() const {
        return m_data;
    }

private:
    int m_line, m_index;
    const std::string m_data;
};

void parse(std::istream& stream, char delim, const std::function<void(const CellData& cdata)>& cb);
void parse(const std::string& input, char delim, const std::function<void(const CellData& cdata)>& cb);
void parse(const std::filesystem::path& filepath, char delim, const std::function<void(const CellData& cdata)>& cb);

} // namespace csv