//
// Created by collio_v on 9/20/15.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"
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
#define literal_string typename
namespace PP { // For PreProcessing
    template <char ... literal_string_>
    class String;

    template <char head, char... tail>
    struct SubChar{
        using next = SubChar<tail...>;
        template<literal_string PPStringFind>
        constexpr static const int  find_first_not_of_s(int off, int it) { return it >= off && !PPStringFind::in(head) ? it : next:: template find_first_not_of_s<PPStringFind>(off, it + 1); }
        constexpr static const char get(int i) { return i ? next::get(i-1) : head; }
        constexpr static const int  find(char c, int off, int it) { return it >= off && c == head ? it : next::find(c, off, it + 1); }
        constexpr static const int  find_first_not_of(char c, int off, int it) { return it >= off && c != head ? it : next::find_first_not_of(c, off, it + 1); }
        constexpr static const int  find_last_of(char c) { return next::find_last_of(c) != -1 ? 1 + next::find_last_of(c) : (c == head ? 0 : -1); }
        constexpr static const bool in(char c) { return c != head ? next::in(c) : true; }
    };

    template <>
    struct SubChar<0> {
        template <literal_string>
        constexpr static const int  find_first_not_of_s(int, int) { return -1; }
        constexpr static const char get(int) { return 0; }
        constexpr static const int  find(char, int, int) { return -1; }
        constexpr static const int  find_first_not_of(char, int, int) { return -1; }
        constexpr static const int  find_last_of(char) { return -1; }
        constexpr static const bool in(char) { return false; }
    };

    template <class, class>
    struct PPStringEqual;
    template <char ... str1, char ... str2>
    struct PPStringEqual< PP::String<str1...>, PP::String<str2...> >    { static const bool value = false; };
    template <char ... str>
    struct PPStringEqual< PP::String<str...>, PP::String<str...> >      { static const bool value = true; };

    template<int len, int it, typename PPString, char ... tail>
    struct PPStringSplitMid;
    template<int len, int it, char ... valid, char head, char ... tail>
    struct PPStringSplitMid<len, it, PP::String<valid...>, head, tail...> { using result = typename PPStringSplitMid<len, it + 1, PP::String<valid..., head>, tail...>::result; };
    template<int len, char ... valid, char head, char ... tail>
    struct PPStringSplitMid<len, len, PP::String<valid...>, head, tail...> { using result = PP::String<valid...>; };
    template<int len, int it, char ... valid>
    struct PPStringSplitMid<len, it, PP::String<valid...>> { using result = PP::String<valid...>; };

    constexpr const bool isItSuperior(int a, int b) { return a > b; }
    template<int beg, int len, class PPString, typename verifier = void>
    struct PPStringSplitBeg;
    template<int beg, int len, char head, char... tail>
    struct PPStringSplitBeg<beg, len, PP::String<head, tail...>, typename std::enable_if<PP::isItSuperior(beg, 0), void >::type > { using result = typename PPStringSplitBeg<beg - 1, len, PP::String<tail...> >::result; };
    template <int len, char ... tail>
    struct PPStringSplitBeg<0, len, PP::String<tail...>> { using result = typename PPStringSplitMid<len, 0, PP::String<>, tail...>::result; };
    template <int beg, int len>
    struct PPStringSplitBeg<beg, len, PP::String<> > { using result = PP::String<>; };

    template<class PPString, char ... literal_string_>
    struct PPStringAdd;
    template<char ... literal_string_1, char ... literal_string_2>
    struct PPStringAdd<PP::String<literal_string_1...>, literal_string_2...> { using result = typename PP::String< literal_string_1..., literal_string_2...>; };

    template<int , literal_string , char ...>
    struct PPStringFind;
    template<bool verif, int it, literal_string PPStringIn, char ... literal_string_>
    struct PPStringFindVerifier;
    template<int it, literal_string PPStringIn, char ... literal_string_>
    struct PPStringFindVerifier<true, it, PPStringIn, literal_string_...> {constexpr static const int value = it;};
    template<int it, literal_string PPStringIn, char ... literal_string_>
    struct PPStringFindVerifier<false, it, PPStringIn, literal_string_...> { constexpr static const int value = PPStringFind<it+1, PPStringIn, literal_string_...>::value; };
    template<int it, literal_string PPStringIn, char head, char ... tail>
    struct PPStringFind<it, PPStringIn, head, tail...> {
        constexpr static const int value =
                PPStringFindVerifier<
                    PP::String<head, tail...>:: template split<0, PPStringIn::size>::result:: template equal<PPStringIn>::value,
                    it, PPStringIn, tail...
                >::value;
    };
    template<int it, literal_string PPStringIn>
    struct PPStringFind<it, PPStringIn> { constexpr static const int value = -1; };

    template <char ... literal_string_>
    class String {
    public:
        static const                std::string value;
        constexpr static const int  size = sizeof...(literal_string_);

        constexpr static char       get(int i) { return SubChar<literal_string_..., 0>::get(i);}
        constexpr static const int  find(char c, int off = 0) { return SubChar<literal_string_..., 0>::find(c, off, 0); }
        template<literal_string PPStringIn>
        struct                      find_s { constexpr static const int value = PPStringFind<0, PPStringIn, literal_string_...>::value; };
        constexpr static const int  find_first_not_of(char c, int off = 0) { return SubChar<literal_string_..., 0>::find_first_not_of(c, off, 0); }
        template<literal_string PPStringFind>
        constexpr static const int  find_first_not_of_s(int off = 0) { return SubChar<literal_string_..., 0>:: template find_first_not_of_s<PPStringFind>(off, 0); }
        constexpr static const int  find_last_of(char c) { return SubChar<literal_string_..., 0>::find_last_of(c); }
        constexpr static const bool in(char c) { return SubChar<literal_string_..., 0>::in(c); }
        template <class PPStringComp>
        struct                      equal { static const bool value = PPStringEqual< String<literal_string_...>, PPStringComp >::value; };
        template <int beg, int len>
        struct                      split { using result = typename PPStringSplitBeg<beg, len, PP::String<literal_string_...> >::result; };
        template <class PPStringAdded>
        struct                      add { using result = typename PPStringAdd<PPStringAdded, literal_string_...>::result; };
    };

    template< class string, char ... literal_string_>       struct string_decay;
    template< char ... valid, char head, char ... tail >    struct string_decay< PP::String< valid... >, head, tail...>
    { using value = typename string_decay< String<valid..., head>, tail...>::value; };
    template< char ... valid, char ... tail>                struct string_decay< PP::String< valid...>, 0, tail...>
    { using value = typename PP::String<valid...>; };

    /**
     * The dynamic value
     */
    template <char ... literal_string_ >
    const std::string String<literal_string_ ... >::value({ literal_string_... });
}

#endif //COMPILEDBNFWRITER_STRING_HPP

#pragma clang diagnostic pop