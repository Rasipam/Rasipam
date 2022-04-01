//
// Created by A on 2021/1/23.
//

#include "BaseEvt.h"

using namespace std;

BaseEvt::BaseEvt(vector<int> attrs) :
        attrs(move(attrs)) {}

int& BaseEvt::operator[](int i) {
    return attrs[i];
}

int BaseEvt::operator[](int i) const {
    return attrs[i];
}

int BaseEvt::size() const {
    return attrs.size();
}

ostream &operator<<(ostream &out, const BaseEvt &event) {
    out << "{";
    for (int i = 0; i < event.attrs.size(); i++) {
        if (i != 0) out << ",";
        out << i << "." << event.attrs.at(i);
    }
    out << "}";
    return out;
}
