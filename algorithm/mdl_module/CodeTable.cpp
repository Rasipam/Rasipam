#include "Common.h"
#include "CodeTable.h"

using namespace std;

double logb2(double d) {
    if (d == 0)
        return 0;
    else
        return lg2(d);
}

CodeTable::CodeTable(Sequence *s) : sequence(s) {
    code_table = new codeTable_set;
    length_ct = 0;
    sz_ct_c = 0;

    par = sequence->get_parameters();
    mu = sequence->get_mu();
    st_codelengths = sequence->get_st_codelengths();
}

// L(D | CT)
double CodeTable::compute_sz_d_ct(Sequence *sequence) {
    double size = 0;

    size += mu->intcost(par->nr_of_attributes);                                            //TERM: L_N(|A|)
    size += mu->intcost(sequence->get_nr_sequences());                                    //TERM: L_N(sequence(D))

    int *seq_sizes = sequence->get_sequence_sizes();
    for (int s = 0;
         s < sequence->get_nr_sequences(); ++s)                                //TERM: sum_{j in sequence(D)} L_N(|S_j|)
        size += mu->intcost(seq_sizes[s]);

    //sum over all Patterns in the code_table: the multiplication of their usage by their codelength
    for (auto it_ct : *code_table) {
        //skip fill-patterns
        if (par->fill_patterns && it_ct->get_size() == 1 &&
            par->fill_pattern[(*it_ct->get_symbols(0)->begin())->attribute] &&
                (*it_ct->get_symbols(0)->begin())->symbol ==
                par->alphabet_sizes[(*it_ct->get_symbols(0)->begin())->attribute] - 1)
            continue;

        if (it_ct->get_usage() > 0) {

            if (it_ct->get_length() < par->pattern_length_min || it_ct->get_length() > par->pattern_length_max) {
                size += it_ct->get_usage() * it_ct->get_codelength() * 50;
            } else {
                size += it_ct->get_usage() * it_ct->get_codelength();                    //TERM: L(C_p | CT)
            }

            if (it_ct->get_length() > 1)    // |X| > 1 -> possible gaps
            {
                size += it_ct->get_usage_gap() *
                        it_ct->get_codelength_gap();            //TERM: L(C_g | CT)		-> gap codes
                size += it_ct->get_usage_fill() *
                        it_ct->get_codelength_fill();        //TERM: L(C_g | CT)		-> no_gap codes
            }
        }
    }
    return size;
}


// L(CT | C)
double CodeTable::compute_sz_ct_c(Sequence *s) {
    double size = 0;
    int *alphabet_sizes = par->alphabet_sizes;
    int nrAttr = par->nr_of_attributes;

    for (int aid = 0; aid < nrAttr; ++aid) {
        int alph = alphabet_sizes[aid];
        if (par->fill_patterns && par->fill_pattern[aid])
            alph -= 1;
        size += mu->intcost(
                alph);                                                    //TERM: L_N(|Omega_aid|)					-> #singletons
        size += mu->lg_choose(par->nr_events,
                              alph);                            //TERM: log( ||D^aid|| OVER |Omega_aid| )	-> singleton supports
    }

    int non_singletons = length_ct;
    for (int aid = 0; aid < nrAttr; ++aid)
        non_singletons -= alphabet_sizes[aid];
    int non_singl_usg = 0;
    for (auto p : *code_table) {  //TERM: L(X in CT) for all non-singleton patters
        if (p->get_size() == 1)
            continue;                                                                    //skip singletons

        if (p->get_usage() == 0)
            continue;
        double sz_x_ct = 0;
        non_singl_usg += p->get_usage();
        sz_x_ct += mu->intcost(
                p->get_length());                                            //TERM: L_N( |X| )							-> length of the pattern

        for (int ts = 0; ts < p->get_length(); ++ts)
            sz_x_ct += logb2(
                    nrAttr);                                                        //TERM: log( |A| )							-> height of the pattern at ts

        sz_x_ct += mu->intcost(p->get_usage_gap() +
                            1);                                        //TERM: L_N( gaps(X)+1 )					-> #gaps in X
        sz_x_ct += p->get_st_size();                                                            //TERM: SUM_{x in X} L( code_p(x|ST) )		-> length of X in left column of CT

        size += sz_x_ct;
    }

    size += 10 * mu->intcost(non_singletons +
                        1);                                            //TERM: L_N( |P|+1 )						-> #non-singletons

    size += mu->intcost(non_singl_usg +
                        1);                                                //TERM: L_N( usage(P)+1 )					-> total usage of non-singletons

    if (non_singl_usg != 0) {
        if (non_singl_usg <
            non_singletons)                                                //one non-singletons with usage=0, rest with usage=1
            size += logb2(
                    non_singletons);                                                //indicate the one with usage=0
        else
            size += mu->lg_choose(non_singl_usg,
                                  non_singletons);                        //TERM: log( usage(P) OVER |P|)				-> the usage per non-singleton pattern
    }

    return size;
}


/* Return:		L(D, CT) = L(D|CT) + L(CT)
   Parameters:	dataStream: either where this code_table is build on or another arbitrary stream
*/
double CodeTable::compute_sz(Sequence *sequence) {
    double size = 0;
    size += compute_sz_d_ct(sequence);                //L(D | CT)

    double temp = compute_sz_ct_c(sequence);
    if (sz_ct_c ==
        0)                                    //when the final CT has been computed we can reuse this size because it does not change when encoding other data
        size += temp;            //L(CT | C)
    else
        size += sz_ct_c;

    return size;
}

// We want to have the highest ordered pattern at the start of the list
//Returns true when the pattern was added, i.e. not yet present
bool CodeTable::insert_pattern(Pattern *p) {
    std::pair<codeTable_set::iterator, bool> ret;
    ret = code_table->insert(p);
    if (ret.second)            //not yet present
        length_ct++;

    return ret.second;
}

bool CodeTable::find_pattern(Pattern *p) {
    for (auto & p_c : *code_table) {
        if (*p_c == *p) {
            return true;
        }
    }
    return false;
}

void CodeTable::delete_pattern(Pattern *p) {
    if (p->get_size() == 1) {
        cout << "Trying to delete a singleton pattern!\n";
        return;
    }

    auto fnd = code_table->find(p), end = code_table->end();
    if (fnd != end) {
        code_table->erase(fnd);
        length_ct--;
    }
}


//delete all Patterns for which the usage dropped to zero in the final code table
void CodeTable::delete_unused_patterns() {
    auto it_ct = code_table->begin(), it_end = code_table->end();
    while (it_ct != it_end) {
        if ((*it_ct)->get_size() == 1) {
            ++it_ct;
            continue;                                    //EXCEPT singletons!
        }

        if ((*it_ct)->get_usage() == 0) {
            it_ct = code_table->erase(it_ct);
            length_ct--;
            it_end = code_table->end();
            if (it_ct == it_end)
                break;
        } else
            ++it_ct;                                    //only if no block is erased we have to shift the iterator
    }
}

//rollback all changed patterns
void CodeTable::rollback() {
    auto it_ct = code_table->begin(), it_end = code_table->end();
    while (it_ct != it_end) {
        (*it_ct)->rollback();
        ++it_ct;
    }
}


string CodeTable::printcandidate_set(bool console_output, candpattern_set *plist, const string &title) const {
    stringstream result;
    result << endl << title << endl;
    candpattern_set::iterator it_patternSet, it_end = plist->end();
    for (it_patternSet = plist->begin(); it_patternSet != it_end; ++it_patternSet) {
        result << "\t\t";
        result << (*it_patternSet)->print(false);
    }
    result << endl;

    if (console_output)
        cout << result.str();
    return result.str();
}

string CodeTable::printpattern_set(bool console_output, codeTable_set *plist, const string &title) const {

    stringstream result;
    result << endl << title << endl;
    codeTable_set::iterator it_pattern_set, it_end = plist->end();
    for (it_pattern_set = plist->begin(); it_pattern_set != it_end; ++it_pattern_set) {
        result << "\t\t";
        result << (*it_pattern_set)->print(false);
    }
    result << endl;

    if (console_output)
        cout << result.str();
    return result.str();
}


string CodeTable::print_ct(bool console_output) const {
    return printpattern_set(console_output, code_table, "Code Table");
}

list<usg_sz> *CodeTable::get_nr_non_singletons_per_size() {
    auto *result = new list<usg_sz>;

    int cnt = 0;
    int sz_old = (*code_table->begin())->get_size();
    for (auto it : *code_table) {
        int sz_new = it->get_size();
        if (sz_old == sz_new)
            cnt++;
        else {
            usg_sz us(cnt, sz_old);
            result->push_back(us);
            cnt = 1;
            sz_old = sz_new;
        }
    }
    return result;
}

CodeTable::~CodeTable() {
    //code_table->clear();
    delete code_table;
}

