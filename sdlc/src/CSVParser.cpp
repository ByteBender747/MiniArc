#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>

#include "CSVParser.hpp"

namespace sdlc::csv
{

void parse(std::istream& stream, char delim, const std::function<void(const CellData& cdata)>& cb)
{
    int lineNum = 0;
    while (!stream.eof()) {
        std::string line;
        std::getline(stream, line);
        std::stringstream ss;
        ++lineNum;
        int index = 0;
        for (char ch : line) {
            if (ch != delim) {
                ss << ch;
            }
            else {
                cb(CellData(ss.str(), lineNum, index++));
                ss.str(std::string());
            }
        }
        std::string last = ss.str();
        if (!last.empty()) {
            cb(CellData(last, lineNum, index));
        }
    }
}

void parse(const std::string& input, char delim, const std::function<void(const CellData& cdata)>& cb)
{
    std::istringstream is(input);
    parse(is, delim, cb);
}

void parse(const std::filesystem::path& filepath, char delim, const std::function<void(const CellData& cdata)>& cb)
{
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("File: '" + filepath.string() + std::string("' not found!"));
    try {
        parse(file, delim, cb);
    }
    catch (...) {
        file.close();
        throw;
    }
    file.close();
}

} // namespace sdlc::csv