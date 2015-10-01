//
// Created by collio_v on 9/19/15.
//

#ifndef COMPILEDBNFWRITER_NODES_HPP
#define COMPILEDBNFWRITER_NODES_HPP

#include <string>
#include <map>
#include <memory>

namespace cBNF {

    class Node {
    public:
        virtual ~Node() = default;
        Node() = default;
        Node(const std::string &value) : _value(value) { }
        Node(const Node& oth) : _value(oth._value), _context(oth._context) {}

    public:
        void                        setValue(const std::string& value) { _value = value; }
        inline const std::string    &value() const { return _value; }

    public:
        inline bool                     has(const std::string& variable) const  { return _context.find(variable) != _context.end(); }
        inline std::shared_ptr<Node>    operator[](const std::string& variable) { return _context.at(variable); }

    private:
        std::string                                     _value;
        std::map<std::string, std::shared_ptr<Node> >   _context;
    };
}

#endif //COMPILEDBNFWRITER_NODES_HPP
