//
// Created by A on 2021/1/23.
//

#include "BaseSeq.h"

using namespace std;

BaseSeq::BaseSeq(vector<BaseEvt> events) :
        events(move(events)) {
}
BaseSeq::BaseSeq(vector<BaseEvt> events, int winner) :
        events(move(events)), winner(winner) {
}

ostream &operator<<(ostream &out, const BaseSeq &sequence) {
    for (const auto & event : sequence.events)
        out << event;
    return out;
}
//BaseSeq & BaseSeq::operator=(const BaseSeq & a) {
//    for (int i = 0; i < cover.size(); ++i) {
//        for (int j = 0; j < cover[i].size(); ++j) {
//            cover[i][j].first = a.cover[i][j].first;
//            cover[i][j].second = a.cover[i][j].second;
//        }
//    }
//    return *this;
//}

vector<BaseEvt>::iterator BaseSeq::begin() {
    return events.begin();
}
vector<BaseEvt>::iterator BaseSeq::end() {
    return events.end();
}

int BaseSeq::size() const {
    return events.size();
}