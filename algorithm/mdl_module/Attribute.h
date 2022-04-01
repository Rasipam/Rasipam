#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "Common.h"
#include "Window.h"

using namespace std;

class Attribute {
public:
    Attribute(int s, int aid, int id, int tree_id) : symbol(s), attribute(aid), id(id), tree_id(tree_id) {}

    void print() const {
        cout << "id: " << id << " sym: " << symbol << " aid: " << attribute << endl;
    }

    const int symbol;
    const int attribute;
    const int id;                //range [0,|event|-1], to identify all events within a event/pattern, e.g. used for is_covered
    const int tree_id;            //ranges from 1 to alphabet_size

};


inline bool operator<(const Attribute &lhs, const Attribute &rhs) {
    return lhs.tree_id < rhs.tree_id;
}

inline bool operator>(const Attribute &lhs, const Attribute &rhs) { return rhs < lhs; }

inline bool operator<=(const Attribute &lhs, const Attribute &rhs) { return !(lhs > rhs); }

inline bool operator>=(const Attribute &lhs, const Attribute &rhs) { return !(lhs < rhs); }

inline bool operator==(const Attribute &lhs, const Attribute &rhs) { return (!(lhs < rhs) && !(rhs < lhs)); }

inline bool operator!=(const Attribute &lhs, const Attribute &rhs) { return !(lhs == rhs); }

struct EventPtrComp {
    bool operator()(const Attribute *lhs, const Attribute *rhs) const {
        return *lhs < *rhs;
    }
};

typedef std::set<Attribute *, EventPtrComp> attribute_set;                    //set of events



#endif