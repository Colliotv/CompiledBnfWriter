//
// Created by collio_v on 9/19/15.
//

#include <iostream>
#include "BNF/grammar.hpp"


template <typename ... childs>
class GrammarParent : public cBNF::Grammar<childs..., GrammarParent<childs...>> {
public:
    GrammarParent()
            : hooks({
            {"test", [](GrammarParent&, cBNF::Node& context, cBNF::varTable& table) -> bool {
              std::cout << std::boolalpha << "exist? " << table.has("var") << std::endl;
              std::cout << "called ? {" << table["var"]->value() << "}" << std::endl;
              return true;
            }}
    }), grammar("entry") { }

public:
    std::map<std::string, std::function<bool(GrammarParent&, cBNF::Node&, cBNF::varTable&)> >
            hooks;
    cBNF::GrammarTable<GrammarParent,
                       cBNF::Rule<makePPString("entry"), RULE(@ignore("c/c++")[[ [ :id[ _second2 ] ] !!["ti"] "ti" eof ]])>,
                       cBNF::Rule<makePPString("_second2"), RULE([ id #test[ :var[ @ignore("null")[ +'t' ] ] ] | +[ '1'->'9' ] ])>
        >   grammar;

};
REGISTER_GRAMMAR_NAME(GrammarParent)


template<typename ... childs>
class GrammarChild1 : public GrammarParent<childs..., GrammarChild1<childs...>> {
public:
    GrammarChild1() : GrammarParent<GrammarChild1>(), grammar("entry") {  }

public:
    cBNF::GrammarTable<GrammarChild1,
                       cBNF::Rule<makePPString("_second2"), RULE([ GrammarParent._second2 "ti" ->["ti"] ]) >,
                       cBNF::Rule<makePPString("newRule"), RULE(&[ "toto" | "titi" ])>
    > grammar;
};
REGISTER_GRAMMAR_NAME(GrammarChild1)

int main() {
    GrammarChild1<> grammar;
    int status;

    std::cout << std::boolalpha << (bool)grammar.parse(" tttt /**/ ttt  ti azazazti") << std::endl;
    std::cout << "done" << std::endl;
    std::cout << std::boolalpha << (bool)grammar.parse(" tttt /**/ 153  ti azazazazti") << std::endl;
    return 0;
}
