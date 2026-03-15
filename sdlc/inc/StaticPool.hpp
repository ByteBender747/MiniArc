#pragma once

#include <array>
#include <functional>

namespace sdlc
{

template <typename item_type>
struct PoolItem {
    item_type value;
    bool acquired{false};
};

template <typename item_type, std::size_t elements>
class StaticPool
{
public:
    using ForEachCallback = std::function<void(PoolItem<item_type>&)>;
    using iterator = typename std::array<PoolItem<item_type>, elements>::iterator;
    using const_iterator = typename std::array<PoolItem<item_type>, elements>::const_iterator;

public:
    // Iterator methods - delegate to std::array
    iterator begin() {
        return m_data.begin();
    }
    iterator end() {
        return m_data.end();
    }
    const_iterator begin() const {
        return m_data.begin();
    }
    const_iterator end() const {
        return m_data.end();
    }
    const_iterator cbegin() const {
        return m_data.cbegin();
    }
    const_iterator cend() const {
        return m_data.cend();
    }

    // Size methods
    constexpr std::size_t size() const {
        return m_data.size();
    }

    // Access operators
    PoolItem<item_type>& operator[](std::size_t index) {
        return m_data[index];
    }
    const PoolItem<item_type>& operator[](std::size_t index) const {
        return m_data[index];
    }

    [[nodiscard]] item_type* acquire() {
        for (std::size_t i = 0; i < elements; ++i) {
            if (!m_data[i].acquired) {
                m_data[i].acquired = true;
                return &m_data[i].value;
            }
        }
        return nullptr;
    }

    void foreachAcquired(ForEachCallback cb) {
        for (std::size_t i = 0; i < elements; ++i) {
            if (m_data[i].acquired) {
                cb(m_data[i]);
            }
        }
    }

    bool release(const item_type* value) {
        for (auto i = 0; i < elements; ++i) {
            if (value == &m_data[i].value) {
                m_data[i].acquired = false;
                return true;
            }
        }
        return false;
    }

    void clear() {
        for (std::size_t i = 0; i < elements; ++i) {
            m_data[i].acquired = false;
        }
    }

    std::size_t count() const {
        std::size_t result = 0;
        for (auto i = 0; i < elements; ++i) {
            if (m_data[i].acquired) {
                ++result;
            }
        }
        return result;
    }

private:
    std::array<PoolItem<item_type>, elements> m_data;
};

} // namespace sdlc