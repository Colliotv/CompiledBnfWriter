//
// Created by collio_v on 9/19/15.
//

#include <iostream>
#include "BNF/grammar.hpp"

int main() {
    using toto = makePPString("toto");
    std::cout << toto::split<toto::find('o'), toto::find_last_of('t')>::result::value << std::endl;
    std::cout << toto::add<toto>::result::value << std::endl;
}
