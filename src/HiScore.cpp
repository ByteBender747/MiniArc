#include <cstdint>
#include <sstream>
#include <fstream>
#include <string>

#include "MiniArc.hpp"
#include "Utility.hpp"

#include <SDL3/SDL.h>

const char* gameName = "MiniArc";
const char* orgName  = "PIB Games";

template <typename value_type>
static value_type parse_value(const std::string &sval)
{
    value_type value;
    std::istringstream ss(sval);
    ss >> value;
    if (ss.fail()) {
        throw std::runtime_error("Invalid attribute value");
    }
    return value;
}

int ScoreRating(const HiScoreTable &table, uint32_t score)
{
    int pos = -1;
    for (int i = 0; i < table.size(); ++i) {
        if (score > table[i].score) {
            pos = i;
            break;
        }
    }
    return pos;
}

static uint32_t GetMinScore(HiScoreTable &table)
{
    uint32_t minScore = UINT32_MAX;
    for (auto& entry : table) {
        minScore = std::min<uint32_t>(minScore, entry.score);
    }
    return minScore;
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
    std::filesystem::path fname = sdlc::GetSaveGameFolder(orgName, gameName) / "HiScore.txt";
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
    return GetMinScore(table);
}

void SaveHiScore(const HiScoreTable &table)
{
    std::filesystem::path fname = sdlc::GetSaveGameFolder(orgName, gameName) / "HiScore.txt";
    std::ofstream stream(fname);
    if (stream.is_open()) {
        for (auto& entry : table) {
            stream << entry.playerName;
            stream << '\t' << entry.score << std::endl;
        }
        stream.close();
    }
}