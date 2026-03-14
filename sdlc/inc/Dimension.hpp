#pragma  once

namespace sdlc
{

template <typename ValueT>
struct Dimension {
    ValueT width, height;
    Dimension() : width(0), height(0) {};
    Dimension(ValueT w, ValueT h) : width(w), height(h) {};
};

}