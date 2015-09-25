//
// Created by collio_v on 9/19/15.
//

#include <iostream>
#include "BNF/grammar.hpp"


class GrammarTest : public cBNF::Grammar<GrammarTest> {
public:
    GrammarTest()
            : Grammar(), hooks({
            {"test", [](GrammarTest&, cBNF::Node& context, cBNF::varTable& table) -> bool {
              std::cout << "called ? {" << table["id"]->value() << "}" << std::endl;
              return true;
            }}
    }), grammar("entry") { }

public:
    std::map<std::string, std::function<bool(GrammarTest&, cBNF::Node&, cBNF::varTable&)> >
            hooks;
    cBNF::GrammarTable<GrammarTest,
                       cBNF::Rule<makePPString("entry"), makePPString("&[ #test[ :id[ _second2 ] ] eof ]")>,
                       cBNF::Rule<makePPString("_second2"), makePPString("?[ &[ id :var[ id ]  +[ '0'->'9' ] ] ]")>
        >   grammar;

};

int main() {
    GrammarTest grammar;

    int status = 0;
    std::cout << std::boolalpha << (bool)grammar.parse(" tttt ttt 123 ") << std::endl;
    return 0;
}
