#include "Common.h"
#include "Pattern.h"
#include "Sequence.h"

int Pattern::id_top = 0;

double log2(double d) {
    if (d == 0)
        return 0;
    else
        return lg2(d);
}

/*
Parameters:
	int length:				# of time steps the pattern is long
	attribute_set **event_sets:	for each time step a pointer to set of events
*/
Pattern::Pattern(int length, attribute_set **event_sets, Sequence *s) : length(length), event_sets(event_sets),
                                                                        g_seq(s) {
    g_x = nullptr;
    g_y = nullptr;
    id = id_top++;
    init();
    set_min_windows(s);
}
Pattern::Pattern(int length, attribute_set **event_sets, Sequence *s, int id) : length(length), event_sets(event_sets),
                                                                        g_seq(s), id(id) {
    g_x = nullptr;
    g_y = nullptr;
    init();
    set_min_windows(s);
}

Pattern::Pattern(int length, attribute_set **event_sets, Sequence *s, Pattern *x, Pattern *y)
        : length(length), event_sets(event_sets), g_seq(s) {
    id = id_top++;
    if (x != nullptr && y != nullptr) {
        //Order x and y such that x is always the one ordered lower
        if (*x > *y) {
            g_x = y;
            g_y = x;
        } else {
            g_x = x;
            g_y = y;
        }
    }
    init();
}


void Pattern::init() {
    g_info = "";
    min_windows = nullptr;
    support = 0;
    alt_min_windows = nullptr;

    codelength = DBL_MAX;
    codelength_gap = DBL_MAX;
    codelength_fill = DBL_MAX;
    r_codelength = DBL_MAX;
    r_codelength_gap = DBL_MAX;
    r_codelength_fill = DBL_MAX;

    estimatedGain = 0;

    usage = 0;
    usage_gap = 0;
    usage_fill = 0;

    r_usage = 0;
    r_usage_gap = 0;
    r_usage_fill = 0;

    usage_decreased = false;

    size = 0;                            //nr of events in the pattern
    szST = 0;                            //encoded size given ST
    total_AIDs = set<int>();                //compute the AIDs over all time steps
    double **base_code_lengths = g_seq->get_st_codelengths();
    for (int l = 0; l < length; ++l) {
        for (auto e : *event_sets[l]) {
            total_AIDs.insert(e->attribute);
            size += 1;                                            //the number of events in the pattern
            szST += base_code_lengths[e->attribute][e->symbol];    //the ST codelength for this event
        }
    }

    //set AID_rank
    AID_rank = 0;
    for (int totalAID : total_AIDs) {
        AID_rank += g_seq->get_mu()->power2(totalAID);        //aid=0 adds 1 to AID_rank, aid=1 adds 2 to AID_rank etc..
    }
}

string Pattern::print(bool console_output) const {
    stringstream result;
    result << "--- sz: " << size << "  len: " << length << "  h: " << total_AIDs.size() << "\t";
    for (int l = 0; l < length; ++l) {
        result << "{";
        auto it = event_sets[l]->begin(), end = event_sets[l]->end();
        while (it != end) {
            result << (*it)->attribute << "." << (*it)->symbol;
            if (++it != end) result << ",";
        }
        result << "}";
    }

    if (!g_seq->get_parameters()->dummy_file.empty())
        result << "\t" << g_info;

    result << "\tSTlen: " << szST;
    result << "\tsup: " << support;
    result << "\tusg: " << usage;
    if (size != 1) result << "\tusgGap: " << usage_gap;
//	result << "\tusgFill: " << usage_fill;
    result << "\tcl: " << codelength;
    if (size != 1) result << "\tgain: " /*<< std::setprecision(15)*/ << estimatedGain;
    //result << "\tusg gap: " << usage_gap;
//	result << "\tadr: " << this;


    //print minimal windows
//        if (support != 0) {
//            result << "\tminWin: ";
//            list<Window *>::iterator it = min_windows->begin(), end = min_windows->end();
//            while (it != end) {
//                result << "(" << (*it)->first->id << "," << (*it)->last->id << ") ";
//                ++it;
//            }
//        }

    //print active windows
    if (usage != 0) {
        result << "\tactiveWindows: ";
        for (auto w : *min_windows)  //loop over its minimum windows
            if (w->active)
                result << "(" << w->first->id << "," << w->last->id << ") ";
    }

    result << endl;

    if (console_output)
        cout << result.str();
    return result.str();
}

string Pattern::print_fp_windows() const {
    stringstream result;
    result << min_windows->size() /*support*/ << " " << usage << " " << usage_gap << " " << length << " ";
    for (int l = 0; l < length; ++l) {
        result << event_sets[l]->size() << " ";
        for (auto it : *event_sets[l])
            result << it->attribute << " " << it->symbol << " ";
    }
    result << endl;

    for (auto w : *min_windows) { //loop over its minimum windows
        result << w->first->id << " " << w->last->id - w->first->id + 1 << " " << w->active << " ";
        for (int it : *w->get_gaps())
            result << it << " ";
    }
    result << endl;
    return result.str();
}

double Pattern::compute_estimated_gain(int usg_x, int usg_y, int usg_z, int usg_s) const {
    double gain = 0;

    bool similar = false;
    if (*g_x == *g_y)
        similar = true;

    int usgX2;
    if (similar) {
        if (usg_z % 2 == 1)
            usgX2 = 1;                                    //X and Y are similar
        else
            usgX2 = 0;
        usg_z /= 2;
    } else
        usgX2 = usg_x - usg_z;                        //usage X after adding Z

    int usgY2 = usg_y - usg_z;                        //usage Y after adding Z
    int usgS2 = usg_s - usg_z;                        //total usage after adding Z

    int nrAttr = g_seq->get_parameters()->nr_of_attributes;
    MathUtil *mu = g_seq->get_mu();

    //DELTA L(CT+Z | C)
    gain -= mu->intcost(length);                                // length of the pattern = L_N( |X| )
    for (int ts = 0; ts < length; ++ts)
        gain -= mu->intcost(nrAttr);                            // height of the pattern at ts = log( |A| )

    gain -= szST;                                                // SUM_{x in X} L( code_p(x|ST) )

    //DELTA L(D | CT+Z)
    gain += usg_s * log2(usg_s);
    gain -= usgS2 * log2(usgS2);
    gain += usg_z * log2(usg_z);
    gain -= (usg_x * log2(usg_x) - usgX2 * log2(usgX2));
    if (!similar)
        gain -= (usg_y * log2(usg_y) - usgY2 * log2(usgY2));

    return gain;
}


//NOTE: usage must be set before this method, i.e. cover must be run

void Pattern::update_codelength(double sum)    //sum includes laplace for every pattern
{
    codelength = -log2((usage + laplace) / sum);


    codelength_gap = -log2((usage_gap + laplace) / (usage_fill + usage_gap + 2 * laplace));

    codelength_fill = -log2((usage_fill + laplace) / (usage_fill + usage_gap + 2 * laplace));

    //Check if the total usage has decreased
    if (r_usage > usage)
        usage_decreased = true;
    else
        usage_decreased = false;
}


/*loop over all occurences for the last Event in this pattern and do the following:
	-step back until we have a window that contains all events of the pattern -> not always minimal
	-from first Event in the pattern step forward to find minimal window within the recently found window
	-note: when a Event can be located at multiple timesteps in the minimal window, in this approach we always choose the first possibility in time. E.g. we choose the first b when covering the minimal window abbc with pattern abc.
*/
list<Window *> *Pattern::build_min_windows(Sequence *s) //NOTE: 's' might be different from 'g_seq'
{
    support = 0;
    auto *result = new list<Window *>();
    const auto **occ_per_timestep = new const list<Event *> *[length];
    int *sup_per_timestep = new int[length];
    Parameters *par = s->get_parameters();
    for (int l = 0; l < length; ++l) {
        occ_per_timestep[l] = s->find_occurrences(event_sets[l]);
        sup_per_timestep[l] = occ_per_timestep[l]->size();
        if (sup_per_timestep[l] == 0)
            return result;    //There can be no minimal windows for this pattern
    }

    if (length == 1) {
        for (auto it : *occ_per_timestep[0]) {
            if (size > 1) {
                int window_index_min = par->pattern_window_index_min >= 0 ?
                                       par->pattern_window_index_min :
                                       s->get_sequence_sizes()[it->seqid] + par->pattern_window_index_min;
                int window_index_max = par->pattern_window_index_max >= 0 ?
                                       par->pattern_window_index_max :
                                       s->get_sequence_sizes()[it->seqid] + par->pattern_window_index_max;
                if (window_index_min > it->seqindex || window_index_max < it->seqindex) {
                    continue;
                }
            }
            ++support;
            const auto **mev_positions = new const Event *[1];
            mev_positions[0] = it;
            auto *w = new Window(mev_positions, this);
            if (!result->empty())
                result->back()->next = w;
            result->push_back(w);
        }
        return result;
    }

    const Event **mev_positions;
    const Event *front, *back;
    auto it_last = occ_per_timestep[length - 1]->rbegin(),
            end_last = occ_per_timestep[length - 1]->rend();
    while (it_last != end_last) {  //loop over all occurences of the last Event in the pattern
        mev_positions = new const Event *[length];
        front = nullptr;
        back = *it_last;
        int id = back->id;

        bool stop = false;
        //find a window that ends at back->id
        mev_positions[length - 1] = back;
        for (int pos = length - 2; pos >= 0; --pos) {   //skip the last
            auto it = occ_per_timestep[pos]->rbegin(), end = occ_per_timestep[pos]->rend();    //reverse iterator
            while ((*it)->id >= id) {
                ++it;
                if (it == end) {
                    stop = true;
                    break;
                }
            }

            if (!stop && (back->id - (*it)->id) + 1 >
                         length)  // max gapsize is patternlength-1 -> total length = 2*patternlength-1
                stop = true;

            if (stop)
                break;

            id = (*it)->id;
            mev_positions[pos] = *it;
            if (pos == 0)
                front = *it;
        }

        //make a minimal window out of the found window
        if (!stop && front != nullptr) {   //if a window was found
            int cur_id = front->id;
            for (int pos = 1; pos < length; ++pos) {   //first Event is already in right place
                auto it = occ_per_timestep[pos]->begin(), end = occ_per_timestep[pos]->end();    //iterator
                while ((*it)->id <= cur_id) ++it;
                mev_positions[pos] = *it;
                cur_id = (*it)->id;
            }
        }

        if (!stop && front != nullptr && front->seqid == back->seqid) {
            int window_index_min = par->pattern_window_index_min >= 0 ?
                                   par->pattern_window_index_min :
                                   s->get_sequence_sizes()[mev_positions[0]->seqid] + par->pattern_window_index_min;
            int window_index_max = par->pattern_window_index_max >= 0 ?
                                   par->pattern_window_index_max :
                                   s->get_sequence_sizes()[mev_positions[length - 1]->seqid] + par->pattern_window_index_max;
            if (window_index_min <= mev_positions[0]->seqindex && window_index_max >= mev_positions[length - 1]->seqindex) {
                auto *w = new Window(mev_positions, this);
                bool present = false;
                if (!result->empty()) {
                    if (w->equal(result->back()))
                        present = true;
                    else
                        w->next = result->back();
                }
                if (present)
                    delete w;
                else
                    result->push_back(w);            //the windows are added with the last starting window first
            }
        } else
            delete[]mev_positions;

        ++it_last;
    }

    //set prev/next_disjoint windows, NOTE: min_windows are ordered with the first starting window at the end of the list
    if (result->size() > 1) {
        Window *w, *nxt = result->back()->next;
        auto it = result->rbegin(), end = result->rend();
        while (it != end) {
            w = *it;

            //for w its next disjoint windows can not be before nxt, thus we can continue with the nxt variable as long as it comes after w
            if (nxt->first->id <= w->first->id)
                nxt = w->next;

            //check if there are events that overlap
            while (overlap(w, nxt))
                nxt = nxt->next;

            if (nxt == nullptr) //all further windows will also have no next_disjoint
                break;

            w->next_disjoint = nxt;
            nxt->prev_disjoint = w;
            ++it;
        }
    }

    //set support (max nr of disjoint minimal windows): we greedily add the minimal window that starts/ends first (because all windows are minimal it is also the window that ends first)
    if (!result->empty()) {
        //NOTE: min_windows are ordered with the first starting window at the back of the list
        Window *greedy = result->back();
        while (greedy != nullptr) {
            ++support;
            greedy = greedy->next_disjoint;
        }
    }

    return result;
}


//Return true when the two event sets overlap on an event
bool Pattern::overlap(attribute_set *events_a, attribute_set *events_b) const {
    auto endB = events_b->end();
    for (auto it : *events_a)
        if (events_b->find(it) != endB)
            return true;
    return false;
}

//Return true when the two windows overlap on an event
bool Pattern::overlap(Window *w, Window *nxt) const {
    if (nxt == nullptr)
        return false;

    //determine offset for overlap
    int offset = nxt->first->id - w->first->id;

    //for the overlapping positions check if the two corresponding event-sets of this pattern overlap
    for (int posA = length - 1; posA >=
                                offset; --posA)    //NOTE: offset >= patternlength means no overlap of events -> is automatically skipped by this loop
    {
        if (overlap(event_sets[posA], event_sets[posA - offset]))
            return true;
    }
    return false;
}

void Pattern::reset_usage() {
    //backup usage
    r_usage = usage;
    r_usage_gap = usage_gap;
    r_usage_fill = usage_fill;
    usage = 0;
    usage_gap = 0;
    usage_fill = 0;

    r_codelength = codelength;
    r_codelength_gap = codelength_gap;
    r_codelength_fill = codelength_fill;
    //leave the codelength itself untouched!

    //set al windows to active = false
    for (auto &minWindow : *min_windows) {
        minWindow->active = false;
    }

}

void Pattern::rollback() {
    usage = r_usage;
    usage_gap = r_usage_gap;
    usage_fill = r_usage_fill;
    codelength = r_codelength;
    codelength_gap = r_codelength_gap;
    codelength_fill = r_codelength_fill;
}

Pattern::~Pattern() {
    total_AIDs.clear();

    for (int l = 0; l < length; ++l) {
        //events themselves get deleted when the events are deleted, when the sequence is deleted
        delete event_sets[l];        //delete the containers, not the events
    }
    delete[]event_sets;

    if (min_windows != nullptr) {
        min_windows->clear();
        delete min_windows;
    }

    if (alt_min_windows != nullptr) {
        alt_min_windows->clear();
        delete alt_min_windows;
    }
}

