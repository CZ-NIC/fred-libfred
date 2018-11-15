#ifndef NAMED_HH_3A4059C222372A9EB2F4A653236A5730//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define NAMED_HH_3A4059C222372A9EB2F4A653236A5730

#include <string>
#include <utility>

namespace Util {
namespace StrongType {

template <typename Type, typename Name> class Named;

template <typename N, typename T>
constexpr N make(const T&);

template <typename N, typename T>
constexpr N make(T&&);

template <typename T, typename N>
std::string to_string(const Named<T, N>&);

template <typename T>
std::string to_string(const T&);

template <typename T, typename N>
constexpr const T& get_raw_value_from(const Named<T, N>&);

template <typename T, typename N>
class Named
{
public:
    using UnderlyingType = T;
    Named() = default;
    ~Named() = default;
    Named(const Named&) = default;
    Named(Named&&) = default;
    Named& operator=(const Named&) = default;
    Named& operator=(Named&&) = default;
private:
    explicit constexpr Named(const T& src) : value_(src) { }
    explicit Named(T&& src) : value_(std::move(src)) { }
    Named& operator=(const T& src) { value_ = src; return *this; }
    Named& operator=(T&& src) { std::swap(value_, src); return *this; }
    T value_;
    friend Named make<>(const T&);
    friend Named make<>(T&&);
    friend std::string to_string<>(const Named&);
    friend const T& get_raw_value_from<>(const Named&);
};

template <typename N, typename T>
constexpr N make(const T& src)
{
    return N(src);
}

template <typename N, typename T>
constexpr N make(T&& src)
{
    return N(std::move(src));
}

template <typename T>
std::string to_string(const T& src)
{
    return std::to_string(src);
}

template <typename T, typename N>
std::string to_string(const Named<T, N>& src)
{
    return to_string(src.value_);
}

template <typename T, typename N>
constexpr const T& get_raw_value_from(const Named<T, N>& src)
{
    return src.value_;
}

}//namespace Util::StrongType
}//namespace Util

#endif//NAMED_HH_3A4059C222372A9EB2F4A653236A5730
