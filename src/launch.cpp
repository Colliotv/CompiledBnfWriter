//
// Created by collio_v on 9/19/15.
//

#include <iostream>
#include "BNF/grammar.hpp"


class GrammarTest : public cBNF::Grammar<GrammarTest,
        cBNF::Rule<
                makePPString("entry"),
                     And< Callback< Extract<Id, makePPString("id")>, makePPString("test") >, Eof>// "entry = [ id:id #test Eof ]"
                        >
                > {
public:
    GrammarTest()
            : Grammar({
                              {"test", [](GrammarTest&, cBNF::Node& context, cBNF::varTable& table) -> bool {
                                  std::cout << "called ? " << table["id"]->value() << std::endl;
                                  return true;
                              }}
                      }, "entry") { }
};

int main() {
    GrammarTest grammar;
    std::cout << makePPString(" *[ id ] ")::get(cBNF::AutoGen<makePPString(" *[ id ] ")>::size) << std::endl;
    std::cout << makePPString(" [ ")::get(makePPString(" [ ")::find('[')) << std::endl;
    std::cout << makePPString(" tt i")::find_first_not_of_s<makePPString(" t")>() << std::endl;
    std::cout << std::boolalpha << (bool)grammar.parse(" test2 ") << std::endl;
}
