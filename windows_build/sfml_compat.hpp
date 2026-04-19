#ifndef SFML_COMPAT_HPP
#define SFML_COMPAT_HPP

#ifdef __cplusplus
#include <string>
#include <cstring>
#endif

/**
 * Technical Compatibility Patch for SFML 2.x + Clang/libc++
 * This provides full char_traits specializations for non-standard character types
 * used internally by SFML (unsigned char, short, int).
 */

#if defined(__cplusplus) && defined(_LIBCPP_VERSION)
namespace std {

template<typename T>
struct sfml_char_traits {
    using char_type = T;
    using int_type = unsigned int;
    using off_type = streamoff;
    using pos_type = streampos;
    using state_type = mbstate_t;

    static void assign(char_type& c1, const char_type& c2) noexcept { c1 = c2; }
    static bool eq(char_type c1, char_type c2) noexcept { return c1 == c2; }
    static bool lt(char_type c1, char_type c2) noexcept { return c1 < c2; }

    static int compare(const char_type* s1, const char_type* s2, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            if (lt(s1[i], s2[i])) return -1;
            if (lt(s2[i], s1[i])) return 1;
        }
        return 0;
    }

    static size_t length(const char_type* s) {
        size_t len = 0;
        while (!eq(s[len], char_type(0))) ++len;
        return len;
    }

    static const char_type* find(const char_type* s, size_t n, const char_type& a) {
        for (size_t i = 0; i < n; ++i) if (eq(s[i], a)) return s + i;
        return nullptr;
    }

    static char_type* move(char_type* s1, const char_type* s2, size_t n) {
        return (char_type*)std::memmove(s1, s2, n * sizeof(char_type));
    }

    static char_type* copy(char_type* s1, const char_type* s2, size_t n) {
        return (char_type*)std::memcpy(s1, s2, n * sizeof(char_type));
    }

    static char_type* assign(char_type* s, size_t n, char_type a) {
        for (size_t i = 0; i < n; ++i) assign(s[i], a);
        return s;
    }

    static int_type not_eof(int_type e) noexcept { return e == eof() ? 0 : e; }
    static char_type to_char_type(int_type e) noexcept { return char_type(e); }
    static int_type to_int_type(char_type c) noexcept { return int_type(c); }
    static bool eq_int_type(int_type e1, int_type e2) noexcept { return e1 == e2; }
    static constexpr int_type eof() noexcept { return int_type(-1); }
};

template<> struct char_traits<unsigned char> : sfml_char_traits<unsigned char> {};
template<> struct char_traits<unsigned short> : sfml_char_traits<unsigned short> {};
template<> struct char_traits<unsigned int> : sfml_char_traits<unsigned int> {};

} // namespace std
#endif

#endif // SFML_COMPAT_HPP
