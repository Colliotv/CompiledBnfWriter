//
// Created by collio_v on 9/19/15.
//

#ifndef COMPILEDBNFWRITER_VARTABLES_H
#define COMPILEDBNFWRITER_VARTABLES_H

#include "BNF/nodes.hh"

namespace cBNF {

    class varTable {
    public:
        inline bool                     has(int variable) const  { return _context.find(variable) != _context.end() && !_context.at(variable).expired(); }
        inline std::shared_ptr<Node>    operator[](int variable) { return _context.at(variable).lock(); }

    public:
        inline void                     refresh() {
            for (auto it = _context.begin(); it != _context.end();) {
                if (it->second.expired())
                    it = _context.erase(it);
                else
                    ++it;
            }
        }
    private:
        std::map<int , std::weak_ptr<Node> > _context;
    };
}

#endif //COMPILEDBNFWRITER_VARTABLES_H
