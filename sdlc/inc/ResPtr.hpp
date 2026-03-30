#pragma once

#include <SDL3/SDL_iostream.h>
#include <utility> // for std::exchange

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

namespace sdlc
{

template<typename resT>
class ResPtr
{
public:
    ResPtr() : m_res(nullptr) {}
    explicit ResPtr(resT* pointer) : m_res(pointer) {}

    ~ResPtr() {
        if (m_res) freeRes(m_res);
    }

    // Delete copy operations
    ResPtr(const ResPtr&) = delete;
    ResPtr& operator=(const ResPtr&) = delete;

    // Move constructor
    ResPtr(ResPtr&& other) noexcept : m_res(std::exchange(other.m_res, nullptr)) {}

    // Move assignment
    ResPtr& operator=(ResPtr&& other) noexcept {
        if (this != &other) {
            if (m_res) freeRes(m_res);
            m_res = std::exchange(other.m_res, nullptr);
        }
        return *this;
    }

    // Assignment from raw pointer
    ResPtr& operator=(resT* rhs) {
        if (m_res != rhs) {
            if (m_res) freeRes(m_res);
            m_res = rhs;
        }
        return *this;
    }

    // Accessors
    operator resT*() const {
        return m_res;
    }
    resT* get() const {
        return m_res;
    }

    // Release ownership without freeing
    resT* release() {
        return std::exchange(m_res, nullptr);
    }

    // Replace the managed resource
    void reset(resT* ptr = nullptr) {
        if (m_res) freeRes(m_res);
        m_res = ptr;
    }

    explicit operator bool() const {
        return m_res != nullptr;
    }

private:
    static inline void freeRes(void* res) {
        SDL_free(res);
    }

    static inline void freeRes(SDL_IOStream* res) {
        SDL_CloseIO(res);
    }   

    static inline void freeRes(SDL_Texture* res) {
        SDL_DestroyTexture(res);
    }

    static inline void freeRes(SDL_Surface* res) {
        SDL_DestroySurface(res);
    }

    static inline void freeRes(SDL_Renderer* res) {
        SDL_DestroyRenderer(res);
    }

    static inline void freeRes(SDL_AudioStream* res) {
        SDL_DestroyAudioStream(res);
    }

private:
    resT* m_res;
};

} // namespace sdlc