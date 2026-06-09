//
// Created by dev on 6/6/26.
//

#ifndef MYENGINE_STRINGHASH_H
#define MYENGINE_STRINGHASH_H
#include <cstdint>
#include <string_view>

class StringHash {
public:
    using HashType = uint32_t;

    constexpr StringHash() : StringHash(std::string_view("")) {}

    template <size_t N>
    constexpr StringHash(const char (&str)[N]) : StringHash(std::string_view(str, N - 1)) {}

    explicit constexpr StringHash(std::string_view str) : hash(compute(str)) {}

    constexpr operator HashType() const { return hash; }
private:
    HashType hash;

    static constexpr HashType compute(std::string_view str) {
        HashType prime = 16777619u;
        HashType basis = 2166136261u;
        for (char c : str) {
            basis ^= static_cast<HashType>(c);
            basis *= prime;
        }
        return basis;
    }
};

template <>
struct std::hash<StringHash> {
    constexpr std::size_t operator()(const StringHash& s) const noexcept {
        return static_cast<StringHash::HashType>(s);
    }
};
#endif //MYENGINE_STRINGHASH_H