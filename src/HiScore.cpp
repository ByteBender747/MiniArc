#include <cstdint>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>

#include "MiniArc.hpp"

#include <SDL3/SDL.h>

const char* gameName = "MiniArc";
const char* orgName  = "PIB Games";

template <typename value_type>
static inline value_type parse_value(std::string sval)
{
    value_type value;
    std::istringstream ss(sval);
    ss >> value;
    if (ss.fail()) {
        throw std::runtime_error("Invalid attribute value");
    }
    return value;
}

static void LoadDefaultScoreBoard(HiScoreTable &table)
{
    table[0] = { "Velo" , 21520 };
    table[1] = { "Jax"  , 13560 };
    table[2] = { "Koda" , 8180  };
    table[3] = { "Nova" , 6250  };
    table[4] = { "Zale" , 5430  };
    table[5] = { "Miro" , 4100  };
    table[6] = { "Echo" , 2500  };
    table[7] = { "Rumi" , 2200  };
    table[8] = { "Flint", 1200  };
    table[9] = { "Lumi" , 560   };
}

uint32_t LoadHiScore(HiScoreTable &table)
{
    uint32_t minScore = UINT32_MAX;
    char* userPath(SDL_GetPrefPath(orgName, gameName));
    std::string fname(userPath);
    fname += "HiScore.txt";
    std::ifstream stream(fname, std::fstream::in);
    std::string line;
    int line_count = 0;
    LoadDefaultScoreBoard(table);
    if (stream.is_open()) {
        while (getline(stream, line)) {
            std::istringstream iss(line);
            std::string name, score;
            getline(iss, name, '\t');
            getline(iss, score, '\t');
            uint32_t score_value = parse_value<uint32_t>(score);
            minScore = std::min(minScore, score_value);
            table[line_count] = {
                .playerName = name,
                .score = score_value
            };
            ++line_count;
            if (line_count >= table.size()) {
                break;
            }
        }
        stream.close();
    }
    SDL_free(userPath);
    return minScore;
}

void SaveHiScore(HiScoreTable &table)
{
    // Sort the table from highest to lowest score
    std::sort(table.begin(), table.end(), [](const HiScoreRecord& a, const HiScoreRecord& b) {
        return a.score > b.score;
    });
    
    char* userPath = SDL_GetPrefPath(orgName, gameName);
    std::string fname(userPath);
    fname += "HiScore.txt";
    std::ofstream stream(fname);
    if (stream.is_open()) {
        for (auto& entry : table) {
            stream << entry.playerName;
            stream << '\t' << entry.score << std::endl;
        }
        stream.close();
    }
    SDL_free(userPath);
}