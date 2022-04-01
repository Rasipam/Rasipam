#include "Common.h"
#include "Event.h"

using namespace std;


Event::Event(int alphabetSize, int id, int seqid, int seqindex) : alphabetSize(alphabetSize), id(id), seqid(seqid), seqindex(seqindex) {
    events = new attribute_set;
    nxt = nullptr;
    size = 0;
}

Event::~Event() {
    events->clear();            //delete the events
    delete events;                //delete the container
    delete[]is_covered;
}


//when all symbols are added to this Event
void Event::finished() {
    is_covered = new Pattern *[size];
    is_overlap = false;
}

void Event::print() const {
    std::cout << "Event: alphabet_size = " << alphabetSize << " sym-aid-id {";
    auto it = events->begin(), end = events->end();
    while (it != end) {
        std::cout << (*it)->symbol << "-" << (*it)->attribute << "-" << (*it)->id;
        if (++it != end)
            std::cout << ", ";
    }
    std::cout << "} id: " << id << " seqid: " << seqid << std::endl;
}

void Event::reset_cover() {
    for (int i = 0; i < size; ++i)
        is_covered[i] = nullptr;
    events_covered = 0;
}

//return true when entire Event is covered
bool Event::cover(Attribute *e, Pattern *p, bool overlap) {
    auto it = events->find(e);
    if (it != events->end()) {
        if (!is_covered[(*it)->id]) {
            is_covered[(*it)->id] = p;
            events_covered++;
            is_overlap = overlap;
        }
    }
    return (events_covered == size);
}

//return true when cover is possible
bool Event::try_cover(Attribute *e) {
    auto it = events->find(e);
    if (it != events->end()) {
        if (!is_covered[(*it)->id])
            return true;
    }
    return false;
}

//return true when cover is not overlap
bool Event::try_overlap() {
    return !is_overlap;
//    for (auto e : *events) {
//        if (is_covered[e->id])
//            return false;
//    }
//    return true;
}

		