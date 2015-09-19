//
// Created by collio_v on 9/19/15.
//

#ifndef COMPILEDBNFWRITER_GRAMMAR_HPP
#define COMPILEDBNFWRITER_GRAMMAR_HPP

#include <bits/stringfwd.h>

namespace cBNF {
    class Node;

    /**
     * Grammar containing all rules
     */
    template <typename ... rules>
    class Grammar {
        cBNF::Node* parse(const std::string&);
    };


    /**
     * Rule with its own definition rule ::= [grammar_node]
     */
    template <typename grammar_node>
    struct Rule {};




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
    template <typename grammar_node, int var_id>
    struct Extract{};

    /**
     * node corresponding to [ grammar_node #call_hook(context, variables)]
     */
    template <typename grammar_node, typename hook_container>
    struct Callback{};


    /**
     * node corresponding to @ignore('char', 'char', 'char', ...)
     * variations are : IgnoreNull, IgnoreBlanks(default)
     */
    template < char ... characters>
    struct Ignore{};
    struct IgnoreNUll: public Ignore<> {};
    struct IgnoreBlanks : public Ignore<' ', '\n', '\t', '\r'>{};



    /**
     * Node corresponding to [ 'char1'...'char2' ]
     */
    template <char char1, char char2>
    struct MatchRange{};

    /**
     * Node corresponding to [ 'char1' ]
     */
    struct MatchChar{};



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
};

#endif //COMPILEDBNFWRITER_GRAMMAR_HPP
