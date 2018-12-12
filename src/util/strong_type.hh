#ifndef STRONG_TYPE_HH_3A4059C222372A9EB2F4A653236A5730//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define STRONG_TYPE_HH_3A4059C222372A9EB2F4A653236A5730

#include <iosfwd>
#include <string>
#include <sstream>
#include <utility>

namespace Util {

template <typename Type, typename Name> class StrongType;

template <typename N, typename T>
constexpr N make_strong(const T&);

template <typename N, typename T>
constexpr N make_strong(T&&);

template <typename T, typename N>
constexpr const T& get_raw_value_from(const StrongType<T, N>&);

template <typename T, typename N>
class StrongType
{
public:
    using UnderlyingType = T;
    StrongType() = default;
    ~StrongType() = default;
    StrongType(const StrongType&) = default;
    StrongType(StrongType&&) = default;
    StrongType& operator=(const StrongType&) = default;
    StrongType& operator=(StrongType&&) = default;
private:
    explicit constexpr StrongType(const T& src) : value_(src) { }
    explicit StrongType(T&& src) : value_(std::move(src)) { }
    StrongType& operator=(const T& src) { value_ = src; return *this; }
    StrongType& operator=(T&& src) { std::swap(value_, src); return *this; }
    T value_;
    friend StrongType make_strong<>(const T&);
    friend StrongType make_strong<>(T&&);
    friend const T& get_raw_value_from<>(const StrongType&);
    friend std::ostream& operator<<(std::ostream& out, const StrongType& src)
    {
        return out << src.value_;
    }
};

template <typename N, typename T>
constexpr N make_strong(const T& src)
{
    return N(src);
}

template <typename N, typename T>
constexpr N make_strong(T&& src)
{
    return N(std::move(src));
}

template <typename T, typename N>
std::string strong_to_string(const StrongType<T, N>& src)
{
    std::ostringstream out;
    out << src;
    return out.str();
}

template <typename T, typename N>
constexpr const T& get_raw_value_from(const StrongType<T, N>& src)
{
    return src.value_;
}

}//namespace Util

#endif//STRONG_TYPE_HH_3A4059C222372A9EB2F4A653236A5730
