#ifndef CODETABLE_H
#define CODETABLE_H

#include "Common.h"
#include "Pattern.h"
#include "Sequence.h"
#include "MathUtil.h"

using namespace std;

class CodeTable {
public:
    explicit CodeTable(Sequence *s);

    ~CodeTable();

    double compute_sz_d_ct(Sequence *sequence);

    double compute_sz_ct_c(Sequence *s);

    double
    compute_sz(Sequence *s);                            //used when we use the CodeTable to compress other data

    void update_ratio(double rt) { ratio = rt; };

    void delete_unused_patterns();                                //NOTE: only used at the very end
    void delete_pattern(Pattern *p);

    bool insert_pattern(Pattern *p);

    void rollback();

    string print_ct() const { return print_ct(true); };

    string print_ct(bool console_output) const;

    string printpattern_set(bool console_output, codeTable_set *plist, const string &title) const;

    string printcandidate_set(bool console_output, candpattern_set *plist, const string &title) const;

    double **get_st_codelengths() const { return st_codelengths; };

    int get_ct_length() const { return length_ct; };

    codeTable_set *get_ct() const { return code_table; };

    void set_sz_ct_c(Sequence *s) {
        sz_ct_c = compute_sz_ct_c(s);
    }; //only called when the final CT for this data is computed

    list<usg_sz> *get_nr_non_singletons_per_size();

    bool find_pattern(Pattern *p);

private:
    Parameters *par;
    MathUtil *mu;
    codeTable_set *code_table;            //COVER ORDER: descending on cardinality, descending on support, descending on L(X|ST), ascending on AID-rank, lexicographically ascending

    Sequence *sequence;            //the data

    int length_ct;                //total number of patterns in the code_table (incl singletons)
    double ratio;                    //between current compression rate of current codeSet versus standard code_table

    double **st_codelengths;        //FOR EACH idLevel: the code length for each singleton item based on its relative frequency in the data (indexed by its own item-value, i.e. the length of pattern 2 is stored at position 2)
    double sz_ct_c;                    //only computed once when the CT is used to cover the data it is build on
};

#endif
