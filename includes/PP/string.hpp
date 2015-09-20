//
// Created by collio_v on 9/20/15.
//

#ifndef COMPILEDBNFWRITER_STRING_HPP
#define COMPILEDBNFWRITER_STRING_HPP


#include <string>


#define PP_DECAY_STRING_1(litteral_string, i)   \
    (sizeof(litteral_string) > (i) ? (litteral_string)[(i)] : 0)

#define PP_DECAY_STRING_4(literal_string, i)    \
    PP_DECAY_STRING_1(litteral_string, i+0),    \
    PP_DECAY_STRING_1(litteral_string, i+64),   \
    PP_DECAY_STRING_1(litteral_string, i+128),  \
    PP_DECAY_STRING_1(litteral_string, i+192),  \

#define PP_DECAY_STRING_16(literal_string, i)   \
    PP_DECAY_STRING_4(litteral_string, i+0),    \
    PP_DECAY_STRING_4(litteral_string, i+4),    \
    PP_DECAY_STRING_4(litteral_string, i+8),    \
    PP_DECAY_STRING_4(litteral_string, i+12),   \

#define PP_DECAY_STRING_64(literal_string, i)   \
    PP_DECAY_STRING_16(litteral_string, i+0),   \
    PP_DECAY_STRING_16(litteral_string, i+16),  \
    PP_DECAY_STRING_16(litteral_string, i+32),  \
    PP_DECAY_STRING_16(litteral_string, i+48),  \

#define PP_DECAY_STRING_256(literal_string, i)  \
    PP_DECAY_STRING_64(litteral_string, i+0),   \
    PP_DECAY_STRING_64(litteral_string, i+64),  \
    PP_DECAY_STRING_64(litteral_string, i+128), \
    PP_DECAY_STRING_64(litteral_string, i+192), \

#define PP_DECAY_STRING_1024(literal_string, i) \
    PP_DECAY_STRING_256(litteral_string, i+0),  \
    PP_DECAY_STRING_256(litteral_string, i+256),\
    PP_DECAY_STRING_256(litteral_string, i+512),\
    PP_DECAY_STRING_256(litteral_string, i+768),\

#define PPString(literal_string) PP::string_decay< PP::String<>, PP_DECAY_STRING_1024(literal_string, 0), 0 >::value

namespace PP { // For PreProcessing
    template <char head, char... tail>
    struct SubChar{
        using next = SubChar<tail...>;
        constexpr static const char get(int i) { return i ? next::get(i-1) : head; }
        constexpr static const int  find(char c, int it) { return c == head ? next::find(c, it + 1) : it; }
        constexpr static const int  find_last_of(char c) { return next::find_last_of(c) != -1 ? 1 + next::find_last_of(c) : (c == head ? 0 : -1); }
        constexpr static const bool in(char c) { return c == head ? next::in(c) : true; }
    };

    template <>
    struct SubChar<0> {
        constexpr static const char get(int) { return 0; }
        constexpr static const int  find(char, int) { return -1; }
        constexpr static const int  find_last_of(char) { return -1; }
        constexpr static const bool in(char) { return false; }
    };

    template <class PPString, class PPString>
    struct PPStringEqual;
    template <char ... str1, char ... str2>
    struct PPStringEqual< PP::String<str1...>, PP::String<str2...> >    { static const bool value = false; };
    template <char ... str>
    struct PPStringEqual< PP::String<str...>, PP::String<str...> >      { static const bool value = true; };

    template <char ... literal_string>
    class String {
    public:
        static const std::string value;

        constexpr static char       get(int i) { return SubChar<literal_string...>::get(i);}
        constexpr static const int  find(char c) { return SubChar<literal_string...>::find(c, 0); }
        constexpr static const int  find_last_of(char c) { return SubChar<literal_string...>::find_last_of(c); }
        constexpr static const bool in(char c) { return SubChar<literal_string>::in(c); }
        template <typename PPStringComp>
        struct                      equal { static const value = PPStringEqual< String<literal_string...>, PPStringComp >::value; };
    };

    template< class string, char ... litteral_string>       struct string_decay;
    template< char ... valid, char head, char ... tail >    struct string_decay< PP::String< valid... >, head, tail...>
    { using value = typename string_decay< String<valid..., head>, tail...>::value; };
    template< char ... valid, char ... tail>                struct string_decay< PP::String< valid...>, tail...>
    { using value = typename PP::String<valid..., 0>; };

    /**
     * The dynamic value
     */
    template <char ... literal_string >
    const std::string String<literal_string ... >::value({ literal_string... });
}

#endif //COMPILEDBNFWRITER_STRING_HPP
