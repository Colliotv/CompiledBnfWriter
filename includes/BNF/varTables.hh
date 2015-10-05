//
// Created by collio_v on 9/19/15.
//

#ifndef COMPILEDBNFWRITER_VARTABLES_H
#define COMPILEDBNFWRITER_VARTABLES_H

#include <algorithm>
#include "../BNF/nodes.hh"

namespace cBNF {

    class varTable {
    public:
        inline bool                     has(const std::string& variable) const  { return _context.find(variable) != _context.end(); }
        inline void                     insert(const std::string& variable, std::shared_ptr<cBNF::Node>& node) { _context.emplace(variable, node); }
        inline std::shared_ptr<Node>&   operator[](const std::string& variable) { return _context.at(variable); }

    public:
        using LifeTimeToken = std::list<std::string>;
        LifeTimeToken   getLifeTimeToken() {
            LifeTimeToken tmp;
            for (decltype(*_context.begin()) p : _context) {
                tmp.push_back(p.first);
            }
            return tmp;
        }
        void            substractLifeTime(LifeTimeToken t) {
            for (std::map<std::string, std::shared_ptr<Node> >::iterator it = _context.begin(); it !=_context.end(); ) {
                if (std::find(std::begin(t), std::end(t), it->first) != t.end()) {
                    ++it;
                } else {
                    it = _context.erase(it);
                }
            }
        }
    private:
        std::map<std::string , std::shared_ptr<Node> > _context;
    };
}

#endif //COMPILEDBNFWRITER_VARTABLES_H
