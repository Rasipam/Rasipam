#ifndef WINDOW_H
#define WINDOW_H

#include "Common.h"

class Pattern;

class Event;

using namespace std;

class Window {
public:
    Window(const Event **mevp, Pattern *p);

    ~Window();

    const Event *get_mev_position(int id) { return mev_positions[id]; }

    int get_gap_length() const { return gap_length; }

    Pattern *get_Pattern() { return pat; }

    set<int> *get_gaps() { return gaps; }

    bool equal(Window *rhs) const;

    void print() const;

    Window *next_disjoint;                //the next disjoint minimal window for this pattern
    Window *prev_disjoint;                //the previous disjoint minimal window for this pattern
    Window *next;                        //the next minimal window for this pattern
    const Event *first, *last;    //the start and end of the minimal window
    bool active;                        //true=used in cover, false=not used

private:
    Pattern *pat;                        //for which pattern this is a minimal window
    const Event **mev_positions;    //for each time step in the pattern a pointer to the corresponding event for this window
    set<int> *gaps;                        //set of timesteps (ID's of events) where there is a gap
    int gap_length;                        //total gap length when covering this window with p
};


#endif