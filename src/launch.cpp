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
              std::cout << "called ? {" << table["id"]->value() << "}" << std::endl;
              return true;
            }}
    }), grammar("entry") { }

public:
    std::map<std::string, std::function<bool(GrammarParent&, cBNF::Node&, cBNF::varTable&)> >
            hooks;
    cBNF::GrammarTable<GrammarParent,
                       cBNF::Rule<makePPString("entry"), makePPString("@ignore(\"null\")[ &[ #test[ :id[ @ignore(' ')[ _second2 ] ] ] eof ] ]")>,
                       cBNF::Rule<makePPString("_second2"), makePPString("?[ &[ id :var[ id ]  +[ '0'->'9' ] ] ]")>
        >   grammar;

};
REGISTER_GRAMMAR_NAME(GrammarParent)


template<typename ... childs>
class GrammarChild1 : public GrammarParent<childs..., GrammarChild1<childs...>> {
public:
    GrammarChild1() : GrammarParent<GrammarChild1>(), grammar("entry") {  }

public:
    cBNF::GrammarTable<GrammarChild1,
                       cBNF::Rule<makePPString("_second2"), makePPString("&[ GrammarParent._second2 \"toto\\\"toto\" ]") >,
                       cBNF::Rule<makePPString("newRule"), makePPString("&[ ]")>
    > grammar;
};
REGISTER_GRAMMAR_NAME(GrammarChild1)

int main() {
    GrammarChild1<> grammar;

    std::cout << cBNF::Match<'@', makePPString("@ignore(\"blanks\")[ id ]")>::_then::value << std::endl;
    std::cout << makePPString("@ignore(\"blanks\")[ id ]")::size << std::endl;
    std::cout << cBNF::Match<'@', makePPString("@ignore(\"blanks\")[ id ]")>::size << std::endl;
    std::cout << std::boolalpha << (bool)grammar.parse(" tttt ttt 123 toto\"toto ") << std::endl;
    return 0;
}
