#pragma once

#include <cstddef>
#include <iterator>

namespace sdl {

template <typename item_type, std::size_t elements>
class StaticPool {
public:
    struct PoolItem {
        item_type value;
        bool acquired{false};
    };

    struct Iterator {
        // Interator tags for internal use and STL
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = PoolItem;
        using pointer           = PoolItem*; // or also value_type*
        using reference         = PoolItem&; // or also value_type&

        Iterator(PoolItem* ptr) : m_ptr(ptr) {
        }

        // Access operators
        reference operator*() const {
            return *m_ptr;
        }
        pointer operator->() {
            return m_ptr;
        }

        // Prefix increment
        Iterator& operator++() {
            ++m_ptr;
            return *this;
        }

        // Postfix increment
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        // Comparaison
        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a.m_ptr == b.m_ptr;
        };
        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return a.m_ptr != b.m_ptr;
        };

    private:
        PoolItem* m_ptr;
    };

public:
    [[nodiscard]] item_type* acquire() {
        for (auto i = 0; i < elements; ++i) {
            if (!m_data[i].acquired) {
                m_data[i].acquired = true;
                return &m_data[i].value;
            }
        }
        return nullptr;
    }

    constexpr Iterator begin() {
        return Iterator(&m_data[0]);
    }

    constexpr Iterator end() {
        return Iterator(&m_data[elements-1]);
    }

    constexpr std::size_t size() const {
        return elements;
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

    bool empty() const {
        bool result = true;
        for (auto i = 0; i < elements; ++i) {
            if (m_data[i].acquired) {
                result = false;
                break;
            }
        }
        return result;
    }

    void clear() {
        for (auto i = 0; i < elements; ++i) {
            m_data[i].acquired = false;
        }
    }

    void release(const item_type* value) {
        for (auto i = 0; i < elements; ++i) {
            if (value == &m_data[i].value) {
                m_data[i].acquired = false;
            }
        }
    }

    constexpr PoolItem& operator[](std::size_t index) {
        return m_data[index];
    }

private:
    PoolItem m_data[elements];
};

} // namespace sdl
