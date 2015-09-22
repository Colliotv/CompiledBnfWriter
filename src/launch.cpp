//
// Created by collio_v on 9/19/15.
//

#include <iostream>
#include "BNF/grammar.hpp"


class GrammarTest : public cBNF::Grammar<GrammarTest,
        cBNF::Rule<makePPString("entry"), makePPString("#test[ :id[ second ] ]")>,
        cBNF::Rule<makePPString("second"), makePPString("*[ |[ \"test\" \"test2\" ] ]")>
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
    std::cout << makePPString(":toto[")::split<1, makePPString(":toto[")::find('[') - 1>::result::value << std::endl;
    std::cout << makePPString(" *[ id ] ")::get(cBNF::AutoGen<makePPString(" *[ id ] ")>::size) << std::endl;
    std::cout << makePPString(" [ ")::get(makePPString(" [ ")::find('[')) << std::endl;
    std::cout << makePPString(" tt i")::find_first_not_of_s<makePPString(" t")>() << std::endl;
    std::cout << std::boolalpha << (bool)grammar.parse(" tttt ttt ") << std::endl;
}
