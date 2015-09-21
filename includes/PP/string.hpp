//
// Created by collio_v on 9/20/15.
//

#ifndef COMPILEDBNFWRITER_STRING_HPP
#define COMPILEDBNFWRITER_STRING_HPP


#include <string>


#define PP_DECAY_STRING_1(literal_string, i)   \
    (sizeof(literal_string) > (i) ? (literal_string)[(i)] : 0)

#define PP_DECAY_STRING_4(literal_string, i)    \
    PP_DECAY_STRING_1(literal_string, i+0),    \
    PP_DECAY_STRING_1(literal_string, i+1),   \
    PP_DECAY_STRING_1(literal_string, i+2),  \
    PP_DECAY_STRING_1(literal_string, i+3)  \

#define PP_DECAY_STRING_16(literal_string, i)   \
    PP_DECAY_STRING_4(literal_string, i+0),    \
    PP_DECAY_STRING_4(literal_string, i+4),    \
    PP_DECAY_STRING_4(literal_string, i+8),    \
    PP_DECAY_STRING_4(literal_string, i+12)   \

#define PP_DECAY_STRING_64(literal_string, i)   \
    PP_DECAY_STRING_16(literal_string, i+0),   \
    PP_DECAY_STRING_16(literal_string, i+16),  \
    PP_DECAY_STRING_16(literal_string, i+32),  \
    PP_DECAY_STRING_16(literal_string, i+48)  \

#define PP_DECAY_STRING_256(literal_string, i)  \
    PP_DECAY_STRING_64(literal_string, i+0),   \
    PP_DECAY_STRING_64(literal_string, i+64),  \
    PP_DECAY_STRING_64(literal_string, i+128), \
    PP_DECAY_STRING_64(literal_string, i+192) \

#define makePPString(literal_string) PP::string_decay< PP::String<>, PP_DECAY_STRING_256(literal_string, 0), 0 >::value

namespace PP { // For PreProcessing
    template <char ... literal_string>
    class String;

    template <char head, char... tail>
    struct SubChar{
        using next = SubChar<tail...>;
        constexpr static const char get(int i) { return i ? next::get(i-1) : head; }
        constexpr static const int  find(char c, int it) { return c == head ? it : next::find(c, it + 1); }
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

    template <class, class>
    struct PPStringEqual;
    template <char ... str1, char ... str2>
    struct PPStringEqual< PP::String<str1...>, PP::String<str2...> >    { static const bool value = false; };
    template <char ... str>
    struct PPStringEqual< PP::String<str...>, PP::String<str...> >      { static const bool value = true; };

    template<int end, int it, typename PPString, char ... tail>
    struct PPStringSplitMid;
    template<int end, int it, char ... valid, char head, char ... tail>
    struct PPStringSplitMid<end, it, PP::String<valid...>, head, tail...> { using result = typename PPStringSplitMid<end, it + 1, PP::String<valid..., head>, tail...>::result; };
    template<int end, char ... valid, char head, char ... tail>
    struct PPStringSplitMid<end, end, PP::String<valid...>, head, tail...> { using result = PP::String<valid..., head>; };
    template<int end, int it, char ... valid>
    struct PPStringSplitMid<end, it, PP::String<valid...>> { using result = PP::String<valid...>; };

    constexpr const bool isItSuperior(int a, int b) { return a > b; }
    template<int beg, int end, class PPString, typename verifier = void>
    struct PPStringSplitBeg;
    template<int beg, int end, char head, char... tail>
    struct PPStringSplitBeg<beg, end, PP::String<head, tail...>, typename std::enable_if<PP::isItSuperior(beg, 0), void >::type > { using result = typename PPStringSplitBeg<beg - 1, end, PP::String<tail...> >::result; };
    template <int end, char ... tail>
    struct PPStringSplitBeg<0, end, PP::String<tail...>> { using result = typename PPStringSplitMid<end, 0, PP::String<>, tail...>::result; };
    template <int beg, int end>
    struct PPStringSplitBeg<beg, end, PP::String<> > { using result = PP::String<>; };

    template<class PPString, char ... literal_string>
    struct PPStringAdd;
    template<char ... literal_string1, char ... literal_string2>
    struct PPStringAdd<PP::String<literal_string1...>, literal_string2...> { using result = typename PP::String< literal_string1..., literal_string2...>; };

    template <char ... literal_string>
    class String {
    public:
        static const std::string value;

        constexpr static char       get(int i) { return SubChar<literal_string..., 0>::get(i);}
        constexpr static const int  find(char c) { return SubChar<literal_string..., 0>::find(c, 0); }
        constexpr static const int  find_last_of(char c) { return SubChar<literal_string..., 0>::find_last_of(c); }
        constexpr static const bool in(char c) { return SubChar<literal_string..., 0>::in(c); }
        template <class PPStringComp>
        struct                      equal { static const bool value = PPStringEqual< String<literal_string...>, PPStringComp >::value; };
        template <int beg, int end>
        struct                      split { using result = typename PPStringSplitBeg<beg, end, PP::String<literal_string...> >::result; };
        template <class PPStringAdded>
        struct                      add { using result = typename PPStringAdd<PPStringAdded, literal_string...>::result; };
    };

    template< class string, char ... literal_string>       struct string_decay;
    template< char ... valid, char head, char ... tail >    struct string_decay< PP::String< valid... >, head, tail...>
    { using value = typename string_decay< String<valid..., head>, tail...>::value; };
    template< char ... valid, char ... tail>                struct string_decay< PP::String< valid...>, 0, tail...>
    { using value = typename PP::String<valid...>; };

    /**
     * The dynamic value
     */
    template <char ... literal_string >
    const std::string String<literal_string ... >::value({ literal_string... });
}

#endif //COMPILEDBNFWRITER_STRING_HPP
