//
// Created by collio_v on 9/19/15.
//

#ifndef COMPILEDBNFWRITER_GRAMMAR_HPP
#define COMPILEDBNFWRITER_GRAMMAR_HPP

#include <memory>
#include <string>
#include <map>
#include <exception>

#include "BNF/nodes.hh"
#include "BNF/nodes.hh"
#include "PP/string.hpp"
#include "varTables.hh"

namespace cBNF {

    struct EofException : public std::exception {
        virtual const char *what() const noexcept override { return "End of File"; }
    };
    /**
     * Grammar containing all rules
     */
    template <typename subclass, typename ... rules>
    class Grammar {
    public:
        virtual ~Grammar() = default;
        Grammar(std::map<std::string, std::function<bool (subclass&, cBNF::Node&, cBNF::varTable&)>> && hooks,
                const std::string &entry);

    private:
        std::map< std::string, std::function< std::shared_ptr<cBNF::Node> (subclass&, cBNF::varTable&)> >
                                _rules;
        std::string             _entry;

    public:
        std::shared_ptr<cBNF::Node> callRule(const std::string& rule_name, cBNF::varTable& table) {
            return _rules.at(rule_name)(*this, table);
        }//need a compile time verifier

    private:
        std::shared_ptr<cBNF::Node>
                                _rule_context;
        std::map< std::string, std::function<bool (subclass&, cBNF::Node&, cBNF::varTable&)> >
                                _hooks;
    public:
        std::shared_ptr<cBNF::Node> getCurrentRuleContext() { return _rule_context; }
        void                        restoreRuleContext(std::shared_ptr<cBNF::Node>& node) { _rule_context = node; }
        void                        newRuleContext() { _rule_context.reset(new cBNF::Node); }
        bool                        callHook(const std::string& hook_name, cBNF::varTable& table) {
            _hooks[hook_name](*dynamic_cast<subclass*>(this), _rule_context, table);
        }

    private:
        std::string             _ignored;

    public:
        bool                    isIgnored(char c) { return _ignored.find(c); }
        void                    setIgnored(const std::string &_ignored) { Grammar::_ignored = _ignored; }

    private:
        std::string             stream_buffer;
        std::string::iterator   stream_cursor;

    public:
        using Context = std::string::iterator;

    public:
        bool        eof() { return stream_cursor == stream_buffer.end(); }
        char        eatChar() {
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
        Context     getContext() { return  stream_cursor; }
        void        restoreContext(Context context) { stream_cursor = context; }
    };


    /**
     * Rule with its own definition rule ::= [grammar_node]
     */
    template <class PPString, typename grammar_node>
    struct Rule {
        using name = PPString;
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
    struct PRepeat{};

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
    template <typename grammar_node, class PPString>
    struct Extract{};

    /**
     * node corresponding to [ grammar_node #call_hook(context, variables)]
     */
    template <typename grammar_node, class PPString>
    struct Callback{};


    /**
     * Node Corresponding to [ Id ]
     */
    template <class PPString >
    struct MatchRule{};


    /**
     * node corresponding to @ignore('char', 'char', 'char', ...)
     * variations are : IgnoreNull, IgnoreBlanks(default)
     */
    template < typename grammar_node, char ... characters>
    struct Ignore{};
    template< typename grammar_node >
    struct IgnoreNUll: public Ignore<grammar_node> {};
    template< typename grammar_node >
    struct IgnoreBlanks : public Ignore<grammar_node, ' ', '\n', '\t', '\r'>{};



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
    template< class PPstring>
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
    struct Id : public And< Repeat< Or< MatchRange< 'a', 'z'>, MatchRange<'A', 'Z'> > >, PRepeat< Or< MatchRange<'a', 'z'>, MatchRange<'0', '9'>, MatchRange<'A', 'Z'> > > >{};

    /**
     * Node corresponding to a number [ ['0'...'9']+ ]
     */
    struct Num : public Repeat< MatchRange<'0', '9'> >{};

    /**
     * Node corresponfing to an eof [ eof ]
     */
    struct Eof {};
};

namespace cBNF {
    namespace tree {
        template<typename Parser, class ParserNode>
        struct for_;

        template <class Parser, typename grammar_node, class PPString>
        struct for_<Parser, Callback<grammar_node, PPString> >{
            static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table) {

                return parser.call(PPString::value);
            }
        };

        template <class Parser, class PPString>
        struct for_<Parser, MatchRule<PPString> >{
            static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table) {
                std::shared_ptr<cBNF::Node> last_context = parser.getCurrentRuleContext();
                parser.newRuleContext();
                std::shared_ptr res = parser.callRule(PPString::value, table);
                parser.restoreRuleContext(last_context);
                return res;
            }
        };

        template <class Parser, typename grammar_node, char ... characters>
        struct for_<Parser, Ignore<grammar_node, characters...> >{
            static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table) {
                parser.setIgnored(std::string({characters..., 0}));
                return for_<Parser, grammar_node>::do_(parser, table);
            }
        };

        template <class Parser, char c1, char c2>
        struct for_<Parser, MatchRange<c1, c2> >{
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table) {
                typename  Parser::Context context = parser.getContext();
                try {
                    char c = parser.eatChar();
                    if (c < c2 && c > c1)
                        return std::make_shared<cBNF::Node>(std::string(1, c));
                }catch (EofException) {}
                return (parser.restoreContext(context), nullptr);
            }
        };

        template <class Parser, char c>
        struct for_<Parser, MatchChar<c> >{
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table) {
                typename  Parser::Context context = parser.getContext();
                try {
                    while (parser.isIgnored(parser.eatChar()));
                    if (parser.eatChar() == c)
                        return std::make_shared<cBNF::Node>(std::string(1, c));
                } catch (EofException) {}
                return (parser.restoreContext(context), nullptr);
            }
        };

        template <class Parser, class PPString>
        struct for_<Parser, MatchString<PPString> >{
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table) {
                typename Parser::Context context = parser.getContext();
                try {
                    std::string _match = parser.eatString(PPString::value.size());
                    if (_match != PPString::value)
                        return (parser.restoreContext(context), nullptr);
                    return std::make_shared<cBNF::Node>(_match);
                } catch (EofException) {
                    return (parser.restoreContext(context), nullptr);
                }
            }
        };

        template<class Parser>
        struct for_<Parser, String>{
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table) {
                typename Parser::Context context = parser.getContext();
                try {
                    while (parser.isIgnored(parser.eatChar()));
                    typename Parser::Context subcontext = parser.getContext();
                    if (parser.eatChar() != '"')
                        return (parser.restoreContext(context), nullptr);
                    while (parser.eatChar() != '"');
                    return std::make_shared<cBNF::Node>(std::string(subcontext, parser.getContext()));
                } catch (EofException) {
                    return (parser.restoreContext(context), nullptr);
                }
            }
        };

        template <class Parser, char c>
        struct for_<Parser, Until<c> > {
            inline static std::shared_ptr< cBNF::Node > do_(Parser& parser, cBNF::varTable& table) {
                typename Parser::Context context = parser.getContext();
                try {
                    while (parser.getChar() != c);
                    return std::make_shared<cBNF::Node>(context, parser.getContext());
                } catch (EofException) {
                    return (parser.restoreContext(context), nullptr);
                }
            }
        };

        template <class Parser>
        struct for_<Parser, Eof> {
            inline static std::shared_ptr<cBNF::Node> do_(Parser& parser, cBNF::varTable& table) {
                while (parser.isIgnored(parser.getChar()));
                if (not parser.eof()) return nullptr;
                return std::make_shared<cBNF::Node>();
            }
        };

        template<typename Parser, class ParserNode>
        struct _rule;//the rule entry
        template<typename Parser, typename name, typename inner>
        struct _rule< Parser, Rule<name, inner> > {
            inline static std::shared_ptr<cBNF::Node> _entry(Parser &parser, cBNF::varTable & table) {
                return for_<Parser, inner>::do_(parser, table);
            };
        };
    }

    template <typename subclass, typename ... rules>
    cBNF::Grammar<subclass, rules...>::
    Grammar(std::map<std::string, std::function<bool(subclass &, cBNF::Node&, cBNF::varTable&)>> && hooks,
           const std::string &entry) : _entry(entry), _hooks(hooks),
               _rules({
                    std::make_pair( rules::name::value, cBNF::tree::_rule<subclass, rules>::_entry )...
                }) {}

}

#endif //COMPILEDBNFWRITER_GRAMMAR_HPP