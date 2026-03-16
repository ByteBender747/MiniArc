#pragma once

namespace sdlc
{

template <typename item_type>
struct PoolItem {
    item_type value;
    bool acquired{false};
};

} // namespace sdlc