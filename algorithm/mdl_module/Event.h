#ifndef EVENT_H
#define EVENT_H

#include "Common.h"
#include "Attribute.h"

class Pattern;

using namespace std;

class Event {
public:
    const int alphabetSize;        //possible number of symbols in this Event-event
    const int id;                    //time step in the data sequence
    const int seqid;                //id of the sequence it is in
    const int seqindex;             //index in the sequence

    Event(int alphabetSize, int id, int seqid, int seqindex);

    ~Event();

    void finished();

    void print() const;

    int get_size() const { return size; }

    attribute_set *get_events() const { return events; }

    void add_event(Attribute *e) {
        events->insert(e);
        size++;
    }

    void set_next(Event *mev) { nxt = mev; };

    const Event *next() const { return nxt; }                        //next event
    void reset_cover();

    bool cover(Attribute *e, Pattern *p, bool overlap);    //return true when entire Event is covered
    bool try_cover(Attribute *e);            //return true when cover is possible
    bool try_overlap();
    Pattern *test_covered(int id) { return is_covered[id]; }

    Pattern **is_covered;    //for each symbol by which pattern it is covered
    bool is_overlap;
    attribute_set *events;        //set of event pointers for this Event

private:
    Event *nxt;        //a pointer to the next Event in the data
    int events_covered;        //counts nr of covered events so we can quickly see when entire Event is covered
    int size;                //nr of events in this event

};


inline bool operator==(const Event &lhs, const Event &rhs) {
    return *lhs.get_events() == *rhs.get_events();
}


#endif