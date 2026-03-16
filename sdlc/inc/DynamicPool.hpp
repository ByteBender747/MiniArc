#pragma once

#include <vector>
#include <functional>

#include "PoolItem.hpp"

namespace sdlc
{

template <typename item_type>
class DynamicPool
{
public:
    using ForEachCallback = std::function<void(PoolItem<item_type>&)>;
    using iterator = typename std::vector<PoolItem<item_type>>::iterator;
    using const_iterator = typename std::vector<PoolItem<item_type>>::const_iterator;

public:
    DynamicPool() {
        m_data.reserve(16);
    }

    // Iterator methods - delegate to std::vector
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
    std::size_t size() const {
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
        // First, try to find a free item
        for (std::size_t i = 0; i < m_data.size(); ++i) {
            if (!m_data[i].acquired) {
                m_data[i].acquired = true;
                return &m_data[i].value;
            }
        }
        
        // No free item available, add a new one
        m_data.emplace_back(PoolItem<item_type>{});
        m_data.back().acquired = true;
        return &m_data.back().value;
    }

    void foreachAcquired(ForEachCallback cb) {
        for (std::size_t i = 0; i < m_data.size(); ++i) {
            if (m_data[i].acquired) {
                cb(m_data[i]);
            }
        }
    }

    bool release(const item_type* value) {
        for (std::size_t i = 0; i < m_data.size(); ++i) {
            if (value == &m_data[i].value) {
                m_data[i].acquired = false;
                return true;
            }
        }
        return false;
    }

    void clear() {
        for (std::size_t i = 0; i < m_data.size(); ++i) {
            m_data[i].acquired = false;
        }
    }

    std::size_t count() const {
        std::size_t result = 0;
        for (std::size_t i = 0; i < m_data.size(); ++i) {
            if (m_data[i].acquired) {
                ++result;
            }
        }
        return result;
    }

    void reserve(std::size_t capacity) {
        m_data.reserve(capacity);
    }

    std::size_t capacity() const {
        return m_data.capacity();
    }

private:
    std::vector<PoolItem<item_type>> m_data;
};

} // namespace sdlc
