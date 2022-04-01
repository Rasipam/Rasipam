#include "Common.h"
#include "Window.h"
#include "Pattern.h"

Window::Window(const Event **mevp, Pattern *p) : mev_positions(mevp),
                                                 pat(p),
                                                 next(nullptr),
                                                 next_disjoint(nullptr),
                                                 prev_disjoint(nullptr) {
    first = mev_positions[0];
    last = mev_positions[pat->get_length() - 1];

    active = false;

    gaps = new set<int>();
    //start with all positions within the window
    for (int ts = first->id; ts <= last->id; ++ts)
        gaps->insert(ts);
    set<int>::iterator it, end = gaps->end();

    //remove all positions where the pattern is present
    for (int ts = 0; ts < pat->get_length(); ++ts)
        if ((it = gaps->find(mev_positions[ts]->id)) != end)
            gaps->erase(it);

    gap_length = gaps->size();
}

Window::~Window() {
    delete[]mev_positions;
    gaps->clear();
}

void Window::print() const {
    cout << "Window:  first=" << first->id << " last=" << last->id;
    if (next != nullptr)
        cout << " next=(" << next->first->id << "-" << next->last->id << ")";
    else
        cout << " next=0";
    if (next_disjoint != nullptr)
        cout << " next_disjoint=(" << next_disjoint->first->id << "-" << next_disjoint->last->id << ")";
    else
        cout << " next_disjoint=0";
    if (prev_disjoint != nullptr)
        cout << " prev_disjoint=(" << prev_disjoint->first->id << "-" << prev_disjoint->last->id << ")" << endl;
    else
        cout << " prev_disjoint=0" << endl;
}

bool Window::equal(Window *rhs) const {
    return (first->id == rhs->first->id && last->id == rhs->last->id);
}

