#include <zip.h>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3_image/SDL_image.h>

#include "Archive.hpp"
#include "ResPtr.hpp"

namespace sdlc
{

Archive::Archive(const char *fileName)
{
    int err;
    m_zip = zip_open(fileName, ZIP_RDONLY, &err);
    if (m_zip == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open archive file: %s", fileName);
    } else {
        zip_int64_t n = zip_get_num_entries(m_zip, 0);
        for (zip_int64_t i = 0; i < n; ++i) {
            zip_stat_t st;
            zip_stat_init(&st);
            zip_stat_index(m_zip, i, 0, &st);
            if (((st.valid & ZIP_STAT_SIZE) != 0) && ((st.valid & ZIP_STAT_NAME) != 0)) {
                m_entries[st.name] = {
                    .index = i,
                    .size = st.size,
                };
            } else {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid entry index: %li", i);
            }
        }
    }
}

ArchiveStreamBuffer::ArchiveStreamBuffer(zip_t* zip, int64_t index, size_t size)
{
    if (zip && size > 0) {
        zip_file_t *zf = zip_fopen_index(zip, index, 0);
        if (zf) {
            m_data = new char[size];
            m_size = zip_fread(zf, m_data, size);
            setg(m_data, m_data, m_data + m_size);
            zip_fclose(zf);
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open archive entry: %li", index);
        }
    }
}

ArchiveStreamBuffer::ArchiveStreamBuffer(const ArchiveStreamBuffer& other)
    : m_size(other.m_size)
{
    m_data = new char[m_size];
    std::memcpy(m_data, other.m_data, other.m_size);
}

ArchiveStreamBuffer::ArchiveStreamBuffer(ArchiveStreamBuffer&& other) noexcept
    : m_data(other.m_data), m_size(other.m_size)
{
    other.m_data = nullptr;
    other.m_size = 0;
}

SDL_IOStream * ArchiveStreamBuffer::getStream()
{
    return SDL_IOFromConstMem(m_data, m_size);
}

ArchiveStreamBuffer::~ArchiveStreamBuffer()
{
    delete[] m_data;
}

ArchiveStreamBuffer Archive::extract(const char* fileName)
{
    if (!m_entries.contains(fileName)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "File entry '%s' not found", fileName);
        return {nullptr, 0, 0};
    }
    ArchiveEntry& entry = m_entries[fileName];
    return {m_zip, entry.index, entry.size};
}

SDL_Texture * Archive::loadTexture(SDL_Renderer* renderer, const char *fileName)
{
    ArchiveStreamBuffer buffer = extract(fileName);
    if (buffer.isEmpty()) {
        return nullptr;
    }
    ResPtr<SDL_Surface> surface(IMG_Load_IO(buffer.getStream(), true));
    if (surface == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading image file: %s", fileName);
        return nullptr;
    } else {
        SDL_Log("Image loaded successfully: %s", fileName);
        return SDL_CreateTextureFromSurface(renderer, surface);
    }
}

Archive::~Archive()
{
    if (m_zip != nullptr) {
        zip_close(m_zip);
    }
}

} // sdlc