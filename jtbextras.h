#pragma once

namespace JTB {

template <typename X, typename Y>
class ConstPair {
    const X xcontent;
    const Y ycontent;

public:
    /* constructor <= 01/13/24 18:41:35 */ 
    ConstPair(X& x, Y& y): xcontent(x), ycontent(y) {};
    ConstPair(X x, Y y): xcontent(x), ycontent(y) {};
    constexpr const X& first() const { return xcontent; }
    constexpr const Y& second() const { return ycontent; }
};

}
