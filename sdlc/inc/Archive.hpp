#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <zip.h>
#include <streambuf>
#include <unordered_map>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_iostream.h>

namespace sdlc {

class ArchiveStreamBuffer : public std::streambuf
{
public:
    ArchiveStreamBuffer(const ArchiveStreamBuffer& other);
    ArchiveStreamBuffer(ArchiveStreamBuffer&& other) noexcept;
    ArchiveStreamBuffer(zip_t* zip, int64_t index, size_t size);
    ~ArchiveStreamBuffer() override;
    SDL_IOStream* getStream();
    bool isEmpty() { return m_data == nullptr && m_size == 0; };
private:
    char* m_data{nullptr};
    size_t m_size{0};
};

class Archive
{
public:
    Archive(const char* fileName);
    virtual ~Archive();
    ArchiveStreamBuffer extract(const char* fileName);
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const char *fileName);
    bool failed() const { return m_zip == nullptr; }
private:
    struct ArchiveEntry
    {
        int64_t index;
        uint64_t size;
    };
    zip_t* m_zip;
    std::unordered_map<std::string, ArchiveEntry> m_entries;    
};

} // sdlc