#ifndef COVER_H
#define COVER_H

#include "Common.h"
#include "Pattern.h"
#include "Sequence.h"
#include "CodeTable.h"
#include "PatternTable.h"
//#ifndef LSH
//#define LSH
//#endif
using namespace std;

class Cover {
private:
    CodeTable *code_table;
    Sequence *sequence;
    int total_usage;
    double sz_sequence_and_ct;            //total compressed size of Data and Code Table: L(D, CT) = L(D|CT) + L(CT)
    bool other_data;                    //false = we cover the data on which the Code Table is build, true = we cover other data

    Parameters *par;

public:
    Cover(Sequence *s, CodeTable *ct, bool other_data);

    ~Cover();

    void compute_total_usage(codeTable_set *ct);


    bool cover_with_pattern_min_windows(Pattern *p);

    bool cover_window_with_pattern(Window *w, Pattern *p);

    void update_pattern_codelengths(
            codeTable_set *ct);    //after every Cover we need to update the codelenghts, except when we use a code set to cover other/new data

    double get_sz_sequence_and_ct() const { return sz_sequence_and_ct; }

    int get_total_usage() const { return total_usage; }
};

#endif
