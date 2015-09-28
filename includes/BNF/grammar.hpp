//
// Created by collio_v on 9/19/15.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"
#ifndef COMPILEDBNFWRITER_GRAMMAR_HPP
#define COMPILEDBNFWRITER_GRAMMAR_HPP

#include <memory>
#include <string>
#include <map>
#include <exception>
#include <cxxabi.h>
#include <list>

#include "../BNF/nodes.hh"
#include "../BNF/nodes.hh"
#include "../PP/string.hpp"
#include "../BNF/varTables.hh"

namespace cBNF {

    struct EofException : public std::exception {
        virtual const char *what() const noexcept override { return "End of File"; }
    };


    /**
     * BaseGrammar
     *  containing child classes
     */
    template<typename base_subclass, typename ... parent_subclasses>
    class Grammar {
    private:
        using _this_type = Grammar<base_subclass, parent_subclasses...>;
    public:
        Grammar();
        virtual ~Grammar() = default;


    private:
        std::map<std::string, std::function<std::shared_ptr<cBNF::Node> (Grammar<base_subclass, parent_subclasses...>&, cBNF::varTable&)> >  _rules;
        std::map<std::string, std::function<std::shared_ptr<cBNF::Node> (const std::string&, Grammar<base_subclass, parent_subclasses...>&, cBNF::varTable&)> > _overided_rules;

    public:
        std::shared_ptr<cBNF::Node> callRule(_this_type& grammar, const std::string &rule_name, cBNF::varTable &table) {
            return _rules.at(rule_name)(grammar, table);
        }//need a compile time verifier
        std::shared_ptr<cBNF::Node> callOveridedRule(_this_type& grammar, const std::string & class_name, const std::string & rule_name, cBNF::varTable& table) {
            return _overided_rules.at(class_name)(rule_name, grammar, table);
        }

    private:
        std::shared_ptr<cBNF::Node>
                _rule_context;

    public:
        std::shared_ptr<cBNF::Node> getCurrentRuleContext() { return _rule_context; }

        void restoreRuleContext(std::shared_ptr<cBNF::Node> &node) { _rule_context = node; }

        void newRuleContext() { _rule_context.reset(new cBNF::Node); }

    private:
        std::string _ignored;

    public:
        bool isIgnored(char c) { return _ignored.find(c) != std::string::npos; }

        void          setIgnored(const std::string &_ignored) { Grammar::_ignored = _ignored; }
        std::string   getIgnored() { return _ignored; }
    private:
        std::string stream_buffer;
        std::string::iterator stream_cursor;

    public:
        using Context = std::string::iterator;

    public:
        bool eof() { return stream_cursor == stream_buffer.end(); }

        char pickChar() {
            if (stream_cursor == stream_buffer.end())
                throw EofException();
            return *stream_cursor;
        }
        char eatChar() {
            if (stream_cursor == stream_buffer.end())
                throw EofException();
            char c = *stream_cursor;
            return (++stream_cursor, c);
        }

        std::string eatString(std::size_t string_size) {
            if (std::distance(stream_cursor, stream_buffer.end()) < string_size)
                throw EofException();

            std::string::iterator beg = stream_cursor;
            std::advance(stream_cursor, string_size);
            return std::string(beg, stream_cursor);
        }

    public:
        Context getContext() { return stream_cursor; }

        void restoreContext(Context context) { stream_cursor = context; }

    public:
        std::shared_ptr<cBNF::Node> parse(const std::string&);
    };

    /**
     * GrammarTable containing all rules
     */
    template<typename _subclass, typename ... rules>
    class GrammarTable {
    public:
        using subclass = _subclass;
        using StaticGrammar = GrammarTable<subclass, rules...>;

    public:
        virtual ~GrammarTable() = default;

        GrammarTable(const std::string &entry);

    public:
        std::map<std::string, std::function<std::shared_ptr<cBNF::Node>(subclass &, cBNF::varTable &)> >
                _rules;

    private:
        std::string _entry;

    public:
        template<typename ... subclasses>
        std::shared_ptr<cBNF::Node> callRule(Grammar<subclasses...>& grammar, const std::string &rule_name, cBNF::varTable &table) {
            return _rules.at(rule_name)(dynamic_cast<subclass&>(grammar), table);
        }//need a compile time verifier
        std::shared_ptr<cBNF::Node> callRule(subclass& grammar, const std::string &rule_name, cBNF::varTable &table) {
            return _rules.at(rule_name)(grammar, table);
        }//need a compile time verifier
        template<typename ... subclasses>
        std::shared_ptr<cBNF::Node> callEntryRule(Grammar<subclasses...>& grammar, cBNF::varTable& table) {
            return _rules.at(_entry)(dynamic_cast<_subclass&>(grammar), table);
        }
    };


    /**
     * Rule with its own definition rule ::= [grammar_node]
     */
    template<literal_string PPString>
    struct AutoGen;

    template<literal_string PPString, typename grammar_node>
    struct Rule {
        using entry = grammar_node;
        using name = PPString;
    };

    template<literal_string PPString, char ... literal_string_>
    struct Rule< PPString, PP::String<literal_string_...> > {
        using entry = typename AutoGen< PP::String<literal_string_...> >::result;
        using name = PPString;
    };


};

/**
 * Node corresponding to [grammar_node]+
 */
template <typename grammar_node>
struct Repeat{};

/**
 * Node corresponding to [grammar_node]*
 */
template <typename grammar_node>
struct PossibleRepeat {};

/**
 * Node corresponding to [grammar_node]?
 */
template <typename grammar_node>
struct PossibleMatch{};

/**
 * Node corresponding to [ grammar_node | grammar_node | .... ]
 */
template <typename ... grammar_nodes>
struct Or{};

/**
 * Node corresponding to [ grammar_node grammar_node .... ]
 */
template <typename ... grammar_nodes >
struct And{};



/**
 * node corresponding to [ grammar_node:var# ]
 */
template <typename grammar_node, literal_string PPString>
struct Extract{};

/**
 * node corresponding to [ grammar_node #call_hook(context, variables)]
 */
template <typename grammar_node, literal_string PPString>
struct Callback{};


/**
 * Node Corresponding to [ Id ]
 */
template <literal_string PPString>
struct MatchRule{};
template<literal_string, literal_string>
struct MatchOverridedRule{};

/**
 * node corresponding to @ignore('char', 'char', 'char', ...)
 * variations are : IgnoreNull, IgnoreBlanks(default)
 */
template < typename grammar_node, char ... characters>
struct Ignore{};
template< typename grammar_node >
using IgnoreNull = Ignore<grammar_node>;
template< typename grammar_node >
using IgnoreBlanks = Ignore<grammar_node, ' ', '\n', '\t', '\r'>;



/**
 * Node corresponding to [ 'char1'...'char2' ]
 */
template <char character1, char character2>
struct MatchRange{};

/**
 * Node corresponding to [ 'char1' ]
 */
template <char character>
struct MatchChar{};

/**
 * Node corresponding to [ "string" ]
 */
template< literal_string PPString>
struct MatchString{};



/**
 * Node corresponding to [ ->'char1' ]
 */
template <char char1>
struct Until{};

/**
 * Node corresponding to a C string [ '"' Until<'"'> ]
 */
struct String{};

/**
 * Node corresponding to an id [ ['a'...'z' | 'A'...'Z']+ ['a'...'z' | 'A'...'Z' | '0'...'9']* ]
 */
using Id = And< IgnoreNull< And< Repeat< Or< MatchRange< 'a', 'z'>, MatchRange<'A', 'Z'> > >, PossibleRepeat< Or< MatchRange<'a', 'z'>, MatchRange<'0', '9'>, MatchRange<'A', 'Z'> > > > > >;

/**
 * Node corresponding to a number [ ['0'...'9']+ ]
 */
using Num = Repeat< MatchRange<'0', '9'> >;

/**
 * Node corresponfing to an eof [ eof ]
 */
struct Eof {};

namespace cBNF {
    namespace tree {
        template<typename Parser, class ParserNode>
        struct for_;

        template<class Parser, typename grammar_node>
        struct for_<Parser, Repeat<grammar_node>> {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                bool valid(false);
                typename Parser::Context context(parser.getContext());
                try {
                    while (parser.isIgnored(parser.pickChar())) parser.eatChar();
                }
                catch (EofException) { }
                typename Parser::Context subcontext(parser.getContext());
                do {
                    if (!for_<Parser, grammar_node>::do_(parser, table)) {
                        if (!valid)
                            return (parser.restoreContext(context), nullptr);
                        else
                            return std::make_shared<cBNF::Node>(std::string(subcontext, parser.getContext()));
                    }
                    valid = true;
                }
                while (1);
            }
        };

        template<class Parser, typename grammar_node>
        struct for_<Parser, PossibleRepeat<grammar_node>> {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                typename Parser::Context context(parser.getContext());
                bool matched(false);
                try {
                    while (parser.isIgnored(parser.pickChar())) parser.eatChar();
                }
                catch (EofException) { }
                typename Parser::Context subcontext(parser.getContext());
                while (for_<Parser, grammar_node>::do_(parser, table)) matched = true;
                if (matched)
                    return std::make_shared<cBNF::Node>(std::string(subcontext, parser.getContext()));
                return (parser.restoreContext(context), std::make_shared<cBNF::Node>());
            }
        };

        template<class Parser, typename grammar_node>
        struct for_<Parser, PossibleMatch<grammar_node>> {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                typename Parser::Context context(parser.getContext());
                bool matched(false);
                try {
                    while (parser.isIgnored(parser.pickChar())) parser.eatChar();
                }
                catch (EofException) { }
                typename Parser::Context subcontext(parser.getContext());
                if (for_<Parser, grammar_node>::do_(parser, table)) matched = true;
                if (matched)
                    return std::make_shared<cBNF::Node>(std::string(subcontext, parser.getContext()));
                return (parser.restoreContext(context), std::make_shared<cBNF::Node>());
            }
        };

        template<class Parser, typename ... tail>
        struct or_;
        template<class Parser>
        struct or_<Parser> { inline static bool do_(Parser&, cBNF::varTable&) { return false; }};
        template<class Parser, typename grammar_node, typename ... tail>
        struct or_<Parser, grammar_node, tail...> {
            inline static bool do_(Parser& parser, cBNF::varTable& table)
            {
                typename Parser::Context context(parser.getContext());
                if (for_<Parser, grammar_node>::do_(parser, table))
                    return true;
                parser.restoreContext(context);
                return (parser.restoreContext(context), or_<Parser, tail...>::do_(parser, table));
            }
        };
        template<class Parser, typename ... grammar_nodes>
        struct for_<Parser, Or<grammar_nodes...>> {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                typename Parser::Context context(parser.getContext());
                try {
                    while (parser.isIgnored(parser.pickChar())) parser.eatChar();
                }
                catch (EofException) { }
                typename Parser::Context subcontext(parser.getContext());
                if (!or_<Parser, grammar_nodes...>::do_(parser, table))
                    return (parser.restoreContext(context), nullptr);
                return std::make_shared<cBNF::Node>(std::string(subcontext, parser.getContext()));
            }
        };

        template<class Parser, typename ... tail>
        struct and_;
        template<class Parser>
        struct and_<Parser> { inline static bool do_(Parser&, cBNF::varTable&) { return true; }};
        template<class Parser, typename grammar_node, typename ... tail>
        struct and_<Parser, grammar_node, tail...> {
            inline static bool do_(Parser& parser, cBNF::varTable& table)
            {
                if (for_<Parser, grammar_node>::do_(parser, table))
                    return and_<Parser, tail...>::do_(parser, table);
                return false;
            }
        };
        template<class Parser, typename ... grammar_nodes>
        struct for_<Parser, And<grammar_nodes...>> {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                typename Parser::Context context(parser.getContext());
                try {
                    while (parser.isIgnored(parser.pickChar())) parser.eatChar();
                }
                catch (EofException) { }
                typename Parser::Context subcontext(parser.getContext());
                if (!and_<Parser, grammar_nodes...>::do_(parser, table))
                    return (parser.restoreContext(context), nullptr);
                return std::make_shared<cBNF::Node>(std::string(subcontext, parser.getContext()));
            }
        };

        template<class Parser, typename grammar_node, literal_string PPString>
        struct for_<Parser, Extract<grammar_node, PPString> > {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                std::shared_ptr<cBNF::Node> node(for_<Parser, grammar_node>::do_(parser, table));

                if (!node)
                    return nullptr;
                table.insert(PPString::value, node);
                return node;
            }
        };

        template<class Parser, typename grammar_node, literal_string PPString>
        struct for_<Parser, Callback<grammar_node, PPString> > {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                std::shared_ptr<cBNF::Node> node(for_<Parser, grammar_node>::do_(parser, table));

                if (!node)
                    return nullptr;
                if (parser.hooks[PPString::value](parser, *parser.getCurrentRuleContext(), table))
                    return node;
                return nullptr;
            }
        };

        template<class Parser, literal_string PPString>
        struct for_<Parser, MatchRule<PPString> > {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                std::shared_ptr<cBNF::Node> last_context(parser.getCurrentRuleContext());
                parser.newRuleContext();
                std::shared_ptr<cBNF::Node> res(parser.callRule(parser, PPString::value, table));
                parser.restoreRuleContext(last_context);
                return res;
            }
        };

        template<class Parser, typename grammar_node, char ... characters>
        struct for_<Parser, Ignore<grammar_node, characters...> > {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                std::string last_ignored(parser.getIgnored());
                parser.setIgnored(PP::String<characters...>::value);
                std::shared_ptr<cBNF::Node> res(for_<Parser, grammar_node>::do_(parser, table));
                parser.setIgnored(last_ignored);
                return res;
            }
        };

        template<class Parser, char c1, char c2>
        struct for_<Parser, MatchRange<c1, c2> > {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                typename Parser::Context context = parser.getContext();
                try {
                    while (parser.isIgnored(parser.pickChar())) parser.eatChar();
                    char c = parser.eatChar();
                    if (c<=c2 && c>=c1)
                        return std::make_shared<cBNF::Node>(std::string(1, c));
                }
                catch (EofException) { }
                return (parser.restoreContext(context), nullptr);
            }
        };

        template<class Parser, char c>
        struct for_<Parser, MatchChar<c> > {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                typename Parser::Context context = parser.getContext();
                try {
                    while (parser.isIgnored(parser.pickChar())) parser.eatChar();
                    if (parser.eatChar()==c)
                        return std::make_shared<cBNF::Node>(std::string(1, c));
                }
                catch (EofException) { }
                return (parser.restoreContext(context), nullptr);
            }
        };

        template<class Parser, literal_string PPString>
        struct for_<Parser, MatchString<PPString> > {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                typename Parser::Context context = parser.getContext();
                try {
                    while (parser.isIgnored(parser.pickChar())) parser.eatChar();
                    std::string _match = parser.eatString(PPString::value.size());
                    if (_match!=PPString::value)
                        return (parser.restoreContext(context), nullptr);
                    return std::make_shared<cBNF::Node>(_match);
                }
                catch (EofException) {
                    return (parser.restoreContext(context), nullptr);
                }
            }
        };

        template<class Parser>
        struct for_<Parser, String> {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                typename Parser::Context context = parser.getContext();
                try {
                    while (parser.isIgnored(parser.pickChar())) parser.eatChar();
                    typename Parser::Context subcontext = parser.getContext();
                    if (parser.eatChar()!='"')
                        return (parser.restoreContext(context), nullptr);
                    while (parser.eatChar()!='"');
                    return std::make_shared<cBNF::Node>(std::string(subcontext, parser.getContext()));
                }
                catch (EofException) {
                    return (parser.restoreContext(context), nullptr);
                }
            }
        };

        template<class Parser, char c>
        struct for_<Parser, Until<c> > {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                typename Parser::Context context = parser.getContext();
                try {
                    while (parser.getChar()!=c);
                    return std::make_shared<cBNF::Node>(context, parser.getContext());
                }
                catch (EofException) {
                    return (parser.restoreContext(context), nullptr);
                }
            }
        };

        template<class Parser>
        struct for_<Parser, Eof> {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table)
            {
                try {
                    while (parser.isIgnored(parser.pickChar())) parser.eatChar();
                }
                catch (EofException) { return std::make_shared<cBNF::Node>(); }
                return nullptr;
            }
        };

        template<typename Parser, class ParserNode>
        struct _rule;
        //the rule entry
        template<typename Parser, typename name, typename inner>
        struct _rule<Parser, Rule<name, inner> > {
            inline static std::shared_ptr<cBNF::Node> _entry(Parser& parser, cBNF::varTable& table)
            {
                return for_<Parser, typename Rule<name, inner>::entry>::do_(parser, table);
            };
        };
    }
};
namespace cBNF {
    template <typename ... subclasses>
    struct _fill;

    template <>
    struct _fill<> {
        template<typename p1, typename p2, typename p3>
        static void add_rule(p1, p2, p3){}
        };
    template <typename head, typename ... tail>
    struct _fill<head, tail...> {
        template<typename ... subclasses>
        static void add_rule(Grammar<subclasses...>& _class,
                std::map<std::string, std::function<std::shared_ptr<cBNF::Node> (Grammar<subclasses...>&, cBNF::varTable&)> >&  _rules,
                std::map<std::string, std::function<std::shared_ptr<cBNF::Node> (const std::string&, Grammar<subclasses...>&, cBNF::varTable&)> >& _overided_rules) {
            auto& _sub_class = dynamic_cast<head&>(_class);
            int status;

            for (auto pair : _sub_class.grammar._rules) {
                _rules.emplace(pair.first,
                        [pair](Grammar<subclasses...>& grammar, cBNF::varTable& table) -> std::shared_ptr<cBNF::Node> {
                          return pair.second(dynamic_cast<head&>(grammar), table);
                        });
            }
            _overided_rules.emplace(std::string(abi::__cxa_demangle(typeid(head).name(), 0, 0, &status)),
                    [&_sub_class](const std::string& _rule, Grammar<subclasses...>& _pclass, cBNF::varTable& table) -> std::shared_ptr<cBNF::Node> {
                        return _sub_class.grammar.callRule(_pclass, _rule, table);
                    });
            _fill<tail...>::add_rule(_class, _rules, _overided_rules);
        }
    };

    template<typename head, typename ... tail>
    cBNF::Grammar<head, tail...>::
    Grammar(){}

    template <typename subclass, typename ... rules>
    cBNF::GrammarTable<subclass, rules...>::
    GrammarTable(const std::string &entry) : _entry(entry),
                                        _rules({
                                                std::make_pair( rules::name::value,
                                                        std::function<std::shared_ptr<cBNF::Node>(subclass &, cBNF::varTable &)>(cBNF::tree::_rule<subclass, rules>::_entry) )...
                                        }) {}

    template<typename base_class, typename ... parent_classes>
    std::shared_ptr<cBNF::Node> cBNF::Grammar<base_class, parent_classes...>::parse(const std::string& string) {
        varTable    table;
        if (_rules.empty()) {
            _fill<base_class, parent_classes...>::add_rule(dynamic_cast<base_class&>(*this), _rules, _overided_rules);
            this->_ignored = {' ', '\n', '\t', '\v', '\f', '\r'};
        }
        this->stream_buffer = string;
        this->stream_cursor = this->stream_buffer.begin();
        std::shared_ptr<cBNF::Node> res(dynamic_cast<base_class&>(*this).grammar.callEntryRule(*this, table));

        if (!res || this->stream_cursor != this->stream_buffer.end())
            return nullptr;
        return res;
    }
}

namespace cBNF {
    template<literal_string name>
    struct PredefinedRule{ using result = MatchRule<name>; };
    template<>
    struct PredefinedRule<makePPString("id")>{ using result = Id; };
    template<>
    struct PredefinedRule<makePPString("num")>{ using result = Num; };
    template<>
    struct PredefinedRule<makePPString("eof")>{ using result = Eof; };
    template<>
    struct PredefinedRule<makePPString("string")>{ using result = String; };

    template<char head, literal_string PPString>
    struct Match {
        using name = typename PPString:: template split< 0, PPString:: template find_first_not_of_s<makePPString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789")>(PPString:: template find_first_not_of_s<makePPString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")>()) >:: result;
        using result = typename PredefinedRule<name>::result;
        constexpr static const int size = name::size;
    };

    template<literal_string PPString>
    struct Match<'*', PPString> {
        using result = PossibleRepeat< typename Match<
                PPString::get( PPString::find_first_not_of(' ', PPString::find('[') + 1) ),
                typename PPString:: template split< PPString::find_first_not_of(' ', PPString::find('[') + 1), PPString::size >::result
        >::result >;
        constexpr static const int _subsize =
                Match<
                        PPString::get( PPString::find_first_not_of(' ', PPString::find('[') + 1) ),
                        typename PPString:: template split< PPString::find_first_not_of(' ', PPString::find('[') + 1), PPString::size >::result
                >::size;
        constexpr static const int size = PPString::find(']', PPString::find_first_not_of(' ', PPString::find('[') + 1) + _subsize) + 1;
    };

    template<literal_string PPString>
    struct Match<'?', PPString> {
        using result = PossibleRepeat< typename Match<
                PPString::get( PPString::find_first_not_of(' ', PPString::find('[') + 1) ),
                typename PPString:: template split< PPString::find_first_not_of(' ', PPString::find('[') + 1), PPString::size >::result
        >::result >;
        constexpr static const int _subsize =
                Match<
                        PPString::get( PPString::find_first_not_of(' ', PPString::find('[') + 1) ),
                        typename PPString:: template split< PPString::find_first_not_of(' ', PPString::find('[') + 1), PPString::size >::result
                >::size;
        constexpr static const int size = PPString::find(']', PPString::find_first_not_of(' ', PPString::find('[') + 1) + _subsize) + 1;
    };

    template<literal_string PPString>
    struct Match<'+', PPString> {
        using result = Repeat< typename Match<
                PPString::get( PPString::find_first_not_of(' ', PPString::find('[') + 1) ),
                typename PPString:: template split< PPString::find_first_not_of(' ', PPString::find('[') + 1), PPString::size >::result
        >::result >;
        constexpr static const int _subsize =
                Match<
                        PPString::get( PPString::find_first_not_of(' ', PPString::find('[') + 1) ),
                        typename PPString:: template split< PPString::find_first_not_of(' ', PPString::find('[') + 1), PPString::size >::result
                >::size;
        constexpr static const int size = PPString::find(']', PPString::find_first_not_of(' ', PPString::find('[') + 1) + _subsize) + 1;
    };

    template<literal_string PPString>
    struct Match<'"', PPString>{
        using name = typename PPString:: template split<1, PPString::find('"', 1) - 1>::result;
        using result = MatchString< name >;
        constexpr static const int size = PPString::find('"', 1) + 1;
    };

    template<literal_string then, literal_string PPString>
    struct SubCharMatch {
        using result = MatchChar< PPString::get(1) >;
        constexpr static const int size = 3;
    };
    template<literal_string PPString>
    struct SubCharMatch<makePPString("->"), PPString> {
        using result = MatchRange< PPString::get(1), PPString::get(6)>;
        constexpr static const int size = 3 + 2 + 3;
    };

    template<literal_string PPString>
    struct Match<'\'', PPString> {
        using result = typename SubCharMatch< typename PPString:: template split<3, 2>::result, PPString>::result;
        constexpr static const int size = SubCharMatch< typename PPString:: template split<3, 2>::result, PPString>::size;
    };

    template<class filling, literal_string PPString, char head>
    struct Fill;
    template<template <typename ...> class filling, typename ... head, literal_string PPString, char character>
    struct Fill< filling<head...> , PPString, character> {
        using sub_type = Match<
                PPString::get(PPString::find_first_not_of(' ')),
                typename PPString:: template split< PPString::find_first_not_of(' '), PPString::size >::result
        >;
        using next_string = typename PPString::
        template split< PPString::find_first_not_of(' ', PPString::find_first_not_of(' ') + sub_type::size), PPString::size>::result;
        using next = Fill< filling<head..., typename sub_type::result>,  next_string, next_string::get(0)>;
        using result = typename next::result;
        constexpr static const int size = next::size + (PPString::size - next_string::size);
    };
    template< template <typename ...>class filling, typename ...  head, literal_string PPString>
    struct Fill< filling<head...>, PPString, ']' >{
        constexpr static const int size = 1;
        using result = filling<head...>;
    };

    template<literal_string PPString>
    struct Match<'&', PPString> {
        using and_result = Fill< And<>, typename PPString:: template split<PPString::find_first_not_of(' ', 2), PPString::size>::result, PPString::get(PPString::find_first_not_of(' ', 2)) >;
        using result = typename and_result::result;
        constexpr static const int size = and_result::size + PPString::find_first_not_of(' ', 2);
    };

    template<literal_string PPString>
    struct Match<'|', PPString> {
        using or_result = Fill< Or<>, typename PPString:: template split<PPString::find_first_not_of(' ', 2), PPString::size>::result, PPString::get(PPString::find_first_not_of(' ', 2)) >;
        using result = typename or_result::result;
        constexpr static const int size = or_result::size + PPString::find_first_not_of(' ', 2);
    };

    template<literal_string PPString>
    struct Match<'@', PPString>;

    template<literal_string PPString>
    struct Match<'#', PPString> {
        using name = typename PPString:: template split<1, PPString::find('[') - 1>::result;
        using result = Callback< typename Match<
                PPString::get( PPString::find_first_not_of(' ', PPString::find('[') + 1) ),
                typename PPString:: template split< PPString::find_first_not_of(' ', PPString::find('[') + 1), PPString::size >::result
        >::result, name >;
        constexpr static const int _subsize =
                Match<
                        PPString::get( PPString::find_first_not_of(' ', PPString::find('[') + 1) ),
                        typename PPString:: template split< PPString::find_first_not_of(' ', PPString::find('[') + 1), PPString::size >::result
                >::size;
        constexpr static const int size = PPString::find(']', PPString::find_first_not_of(' ', PPString::find('[') + 1) + _subsize) + 1;
    };

    template<literal_string PPString>
    struct Match<':', PPString>{
        using name = typename PPString:: template split<1, PPString::find('[') - 1>::result;
        using result = Extract< typename Match<
                PPString::get( PPString::find_first_not_of(' ', PPString::find('[') + 1) ),
                typename PPString:: template split< PPString::find_first_not_of(' ', PPString::find('[') + 1), PPString::size >::result
        >::result, name >;
        constexpr static const int _subsize =
                Match<
                        PPString::get( PPString::find_first_not_of(' ', PPString::find('[') + 1) ),
                        typename PPString:: template split< PPString::find_first_not_of(' ', PPString::find('[') + 1), PPString::size >::result
                >::size;
        constexpr static const int size = PPString::find(']', PPString::find_first_not_of(' ', PPString::find('[') + 1) + _subsize) + 1;
    };

    template<char ... literal_string_>
    struct AutoGen< PP::String< literal_string_... > > {
        using result =
        typename Match<   PP::String<literal_string_...>::get(PP::String<literal_string_...>::find_first_not_of(' ')),
                          typename PP::String<literal_string_...>:: template split<PP::String<literal_string_...>::find_first_not_of(' '), sizeof...(literal_string_)>::result
        >::result;
        constexpr static const int _subsize =
                Match<   PP::String<literal_string_...>::get(PP::String<literal_string_...>::find_first_not_of(' ')),
                         typename PP::String<literal_string_...>:: template split<PP::String<literal_string_...>::find_first_not_of(' '), sizeof...(literal_string_)>::result
                >::size;
        constexpr static const int size = _subsize + PP::String< literal_string_... >::find_first_not_of(' ');
    };
};

#endif //COMPILEDBNFWRITER_GRAMMAR_HPP

#pragma clang diagnostic pop