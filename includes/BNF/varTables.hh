//
// Created by collio_v on 9/19/15.
//

#ifndef COMPILEDBNFWRITER_VARTABLES_H
#define COMPILEDBNFWRITER_VARTABLES_H

#include "BNF/nodes.hh"

namespace cBNF {
    class varTable {
    public:
        inline bool                     has(const std::string& variable) const  { return _context.find(variable) != _context.end(); }
        inline std::shared_ptr<Node>    operator[](const std::string& variable) { return _context.at(variable); }

    private:
        std::map<std::string, std::shared_ptr<Node> > _context;
    };
}

#endif //COMPILEDBNFWRITER_VARTABLES_H
