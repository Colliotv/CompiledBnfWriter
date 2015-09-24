//
// Created by collio_v on 9/19/15.
//

#include <iostream>
#include "BNF/grammar.hpp"


class GrammarTest : public cBNF::Grammar<GrammarTest,
        cBNF::Rule<makePPString("entry"), makePPString("&[ #test[ :id[ second ] ] eof ]")>,
        cBNF::Rule<makePPString("second"), makePPString("?[ &[ id :var[ id ]  ?[ num ] ] ]")>
                > {
public:
    GrammarTest()
            : Grammar({
            {"test", [](GrammarTest&, cBNF::Node& context, cBNF::varTable& table) -> bool {
              std::cout << "called ? {" << table["id"]->value() << "}" << std::endl;
              return true;
            }}
    }, "entry") { }
};

int main() {
    GrammarTest grammar;

    std::cout << std::boolalpha << (bool)grammar.parse(" tttt ttt 123 ") << std::endl;
    return 0;
}
