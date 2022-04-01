#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "Common.h"
#include "Event.h"
#include "MathUtil.h"
#include "Pattern.h"

//#define LSH

using namespace std;

class Sequence {
public:
    Sequence(FILE *f, Parameters *par);    //constructor
    ~Sequence();

    void pre_init();

    void init();

    int read(FILE *f);

    Event **get_mev_time() { return mev_time; }

    float get_mev_size_term(int id) { return mev_term[mev_time[id]->get_size()]; }

    MathUtil *get_mu() const { return mu; }

    Parameters *get_parameters() { return par; }

    int get_nr_events() const { return nr_events; }

    int get_nr_sequences() const { return nr_sequences; }

    int *get_sequence_sizes() const { return sequence_sizes; }

    double **get_st_codelengths() { return ST_codelengths; }

    int get_sup(int aid, int s) { return occ[aid][s].size(); }

    int **get_tree_ids() { return tree_ids; }

    void compute_st_codelengths();

    string print_sequence(bool all_values);

    const list<Event *> *find_occurrences(attribute_set *events) const;

    int load_dummies();

    void reset_cover() {
        for (int i = 0; i < par->nr_events; ++i) {
            is_covered[i] = 0;
            mev_time[i]->reset_cover();
        }
        mev_covered = 0;
    }

    bool cover(Pattern *p, Window *w); //pos = id of event in the sequence
    bool cover(Attribute *e, int pos, Pattern *p);
    bool try_cover(attribute_set *events, int pos); //true = cover is possible
    bool try_cover(Attribute *e, int pos);
    bool try_overlap(int pos);

    void cover_singletons(Pattern ***singletons);

    bool debug_flag;//DEBUG
    bool error_flag;
    Event **mev_time;                        //a multi-event array, one event pointer per time step
    int nr_events;                                    //total number of events


protected:
    Parameters *par;

    MathUtil *mu;

    list<Event *> **occ;                        //for each attribute for each symbol an array of Event* where it occurs,
    // occ[aid][s] -> list of Event* for symbol s on attribute aid

    int nr_sequences;                                //number of sequences in the data
    int *sequence_sizes;                            //holds the length of each sequence (sequences are separated with '-1' in the input)
    double **ST_codelengths;                        //for each attribute for each symbol its base codelengths

    int **tree_ids;                                    //all tree-id's based on attr and sym

    int *is_covered;                                    //for every event whether it is completely covered. E.g. is_covered[timestep] = 1 means it is completely covered
    int mev_covered;                                    //the number of completely covered events

    float *mev_term;                                //for each event on position ' id'  with 'sz' events: -lg(probMev[id]) = -lg(P(sz | data)) = -log of the probability that a event in the data has 'sz' events

    ostringstream output_stream;
};


#endif
