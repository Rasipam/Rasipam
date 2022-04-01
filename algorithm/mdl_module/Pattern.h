#ifndef PATTERN_H
#define PATTERN_H

#include "Common.h"
#include "Window.h"
#include "Event.h"
#include "MathUtil.h"

using namespace std;

class Sequence;

class Pattern {

public:
    Pattern(int l, attribute_set **event_sets, Sequence *s);
    Pattern(int l, attribute_set **event_sets, Sequence *s, int id);

    Pattern(int l, attribute_set **event_sets, Sequence *s, Pattern *x, Pattern *y);

    ~Pattern();

    void init();

    string print() const { return print(true); }

    string print(bool console_output) const;

    string print_fp_windows() const;

    int get_id() const {return id;}

    int get_aid_rank() const { return AID_rank; }

    double get_st_size() const { return szST; }

    int get_support() const { return support; }

    list<Window *> *get_min_windows(Sequence *seq, bool other_data) {
        if (!other_data) return min_windows;
        else {
            if (alt_min_windows != nullptr)return alt_min_windows;
            alt_min_windows = build_min_windows(seq);
        }
        return alt_min_windows;
    }

    bool get_usage_decreased() const { return usage_decreased; }

    int get_usage() const { return usage; }

    int get_estimated_usage() { if (g_x == nullptr) return usage; else return min(g_x->get_usage(), g_y->get_usage()); }

    int get_usage_gap() const { return usage_gap; }

    int get_usage_fill() const { return usage_fill; }


    double get_codelength() const { return codelength; }

    double get_codelength_gap() const { return codelength_gap; }

    double get_codelength_fill() const { return codelength_fill; }


    double get_estimated_gain() const { return estimatedGain; }

    Pattern *get_x() { return g_x; }

    Pattern *get_y() { return g_y; }

    set<int> *get_total_aids() { return &total_AIDs; }

    int get_height_at_pos(int pos) const { return event_sets[pos]->size(); }

    int get_length() const { return length; }

    int get_size() const { return size; }

    attribute_set *get_symbols(int timestep) const { return event_sets[timestep]; }
    attribute_set **get_events() const { return event_sets; }

    bool overlap(Window *w, Window *nxt) const;

    bool overlap(attribute_set *events_a, attribute_set *events_b) const;

    void update_usages(int usg_gap, int use = 1) {
        usage += use;
        usage_gap += usg_gap;
        usage_fill += length - 1;
    }

    double compute_estimated_gain(int usg_x, int usg_y, int usg_z, int usg_s) const;

    void set_estimated_gain(double gain) { estimatedGain = gain; }

    void load_windows_and_support(Pattern *p) {
        support = p->get_support();
        min_windows = p->get_min_windows(g_seq, false);
    }                    //only used for black_list/whitelist

    void set_info(const string &s) { g_info += s; }

    void set_min_windows(Sequence *s) { min_windows = build_min_windows(s); }

    void reset_usage();

    void update_codelength(double sum); //sum = sum of all usages + laplace
    void rollback();

    static int id_top;
private:
    list<Window *> *build_min_windows(Sequence *s);
    int id;

    Sequence *g_seq;
    set<int> total_AIDs;        //over all time steps the set of attributes contained in this pattern
    int length;                //the number events it contains
    int size;                //total nr of events in the pattern
    int AID_rank;            //rank based on the attributes it specifies values for, i.e. for every attribute we have a 1-bit when the pattern has a value for it, and a 0-bit otherwise
    double szST;            //size of the pattern encoded with base code lengths

    double codelength, codelength_gap, codelength_fill;        //given a covering: the codeLength of the pattern, a gap in this pattern, and a fill in this pattern
    double estimatedGain;    //expected gain in L(CT,D) when adding this pattern to the code table
    int usage, usage_gap, usage_fill;
    int support;            //max nr of disjoint minimal windows

    string g_info;            //breakdown info for generated patterns

    attribute_set **event_sets;        //a pointer to a set of events for each timestep

    list<Window *> *min_windows;        //a list of all minimal windows for this pattern
    list<Window *> *alt_min_windows;    //alternative list of all minimal windows for this pattern in another sequence

    Pattern *g_x, *g_y;            //the two patterns from which this pattern is build. NOTE: x < y

    //for rollback
    double r_codelength, r_codelength_gap, r_codelength_fill;
    int r_usage, r_usage_gap, r_usage_fill;
    bool usage_decreased;

};


//Lowest ordered pattern at start of the list
//RETURN true if lhs < rhs 
inline bool operator<(const Pattern &lhs, const Pattern &rhs) {
    //descending on cardinality ||X||
    if (lhs.get_size() < rhs.get_size())
        return false;
    if (lhs.get_size() > rhs.get_size())
        return true;

    //descending on L(X|ST)
    if (lhs.get_st_size() < rhs.get_st_size())
        return false;
    if (lhs.get_st_size() > rhs.get_st_size())
        return true;

/*	//ascending on AID-rank
	if (lhs.getAIDrank() > rhs.getAIDrank())
		return false;
	if (lhs.getAIDrank() < rhs.get_aid_rank())
		return true;
*/
    //ascending lexicographically
    //loop over all events from top to bottom and left to right
    for (int ts = 0; ts < max(lhs.get_length(), rhs.get_length()); ++ts) {
        //ascending on length
        if (ts > lhs.get_length() -
                 1)                //up to here everything equal, but rhs is longer than lhs, thus lhs has higher order
            return true;
        if (ts > rhs.get_length() - 1)
            return false;

        attribute_set *avts = lhs.get_symbols(ts), *bvts = rhs.get_symbols(ts);
        auto ita = avts->begin(), enda = avts->end(), itb = bvts->begin(), endb = bvts->end();
        while (ita != enda && itb != endb) {
            Attribute *eva = (*ita);
            Attribute *evb = (*itb);

            if (*eva < *evb)        //NOTE: we compare events NOT integers
                return true;
            if (*evb < *eva)
                return false;

            ++ita;
            ++itb;
            if (ita == enda &&
                itb != endb)        //lhs is smaller on this time step but all its events are similar to rhs
                return true;
            if (itb == endb && ita != enda)
                return false;
        }
    }

    return false;
}

inline bool operator>(const Pattern &lhs, const Pattern &rhs) { return rhs < lhs; }

inline bool operator<=(const Pattern &lhs, const Pattern &rhs) { return !(lhs > rhs); }

inline bool operator>=(const Pattern &lhs, const Pattern &rhs) { return !(lhs < rhs); }

inline bool operator==(const Pattern &lhs, const Pattern &rhs) { return (!(lhs < rhs) && !(rhs < lhs)); }

inline bool operator!=(const Pattern &lhs, const Pattern &rhs) { return !(lhs == rhs); }


struct PatternPtrComp    //for code table
{
    bool operator()(const Pattern *lhs, const Pattern *rhs) const {
        return *lhs < *rhs;
    }
};


//COVER ORDER: descending on cardinality, descending on support, descending on L(X|ST), ascending on AID-rank, lexicographically ascending
struct CodeTablePatternPtrComp    //for code table elements
{
    bool operator()(const Pattern *lhs, const Pattern *rhs) const {
        //descending on cardinality
        if (lhs->get_size() < rhs->get_size())
            return false;
        if (lhs->get_size() > rhs->get_size())
            return true;

        //descending on support
        if (lhs->get_support() < rhs->get_support())
            return false;
        if (lhs->get_support() > rhs->get_support())
            return true;

        return *lhs < *rhs;
    }
};

//CANDIDATE ORDER: descending on gain, descending on support, descending on cardinality, descending on L(X|ST), ascending on AID-rank, lexicographically ascending
struct CandidatePatternPtrComp    //NOTE: top candidate is positioned at begin of the set
{
    bool operator()(const Pattern *lhs, const Pattern *rhs) const {
        //descending on estimated gain
        if (lhs->get_estimated_gain() < rhs->get_estimated_gain())
            return false;
        if (lhs->get_estimated_gain() > rhs->get_estimated_gain())
            return true;

        //descending on support
        if (lhs->get_support() < rhs->get_support())
            return false;
        if (lhs->get_support() > rhs->get_support())
            return true;

        return *lhs < *rhs;
    }
};

//USAGE ORDER: descending on usage, descending on support, descending on cardinality, descending on L(X|ST), ascending on AID-rank, lexicographically ascending
struct UsagePatternPtrComp {
    bool operator()(const Pattern *lhs, const Pattern *rhs) const {
        //descending on usage
        if (lhs->get_usage() < rhs->get_usage())
            return false;
        if (lhs->get_usage() > rhs->get_usage())
            return true;

        //descending on support
        if (lhs->get_support() < rhs->get_support())
            return false;
        if (lhs->get_support() > rhs->get_support())
            return true;

        return *lhs < *rhs;
    }
};

//PRUNE ORDER: ascending on usage, descending on support, descending on cardinality, descending on L(X|ST), ascending on AID-rank, lexicographically ascending
struct PrunePatternPtrComp    //for prune candidates -> prune-candidate with smallest usage is considered first and is positioned at the end of the set
{
    bool operator()(const Pattern *lhs, const Pattern *rhs) const {
        //ascending on usage
        if (lhs->get_usage() < rhs->get_usage())
            return true;
        if (lhs->get_usage() > rhs->get_usage())
            return false;

        //descending on support
        if (lhs->get_support() < rhs->get_support())
            return false;
        if (lhs->get_support() > rhs->get_support())
            return true;

        return *lhs < *rhs;
    }
};

typedef std::set<Pattern *, PatternPtrComp> pattern_set;                    //set of patterns

typedef std::set<Pattern *, CodeTablePatternPtrComp> codeTable_set;        //set of patterns

typedef std::set<Pattern *, CandidatePatternPtrComp> candpattern_set;        //set of patterns

typedef std::set<Pattern *, UsagePatternPtrComp> usagepattern_set;        //set of patterns

typedef std::set<Pattern *, PrunePatternPtrComp> prunepattern_set;        //set of patterns


struct usg_sz {
    usg_sz(int usg, double sz) : sz(sz), usg(usg) {}

    //public String to_string() { cout << "Size: " << sz << "  Total usage: " << usg << endl; }

    double sz;
    int usg;
};

inline std::ostream &operator<<(std::ostream &Str, usg_sz const &v) {
    Str << "Size: " << v.sz << "  Total usage: " << v.usg << endl;
    return Str;
}

struct attr_sym {
    attr_sym(int aid, int sym) : aid(aid), sym(sym) {}

    string print() const {
        std::stringstream ss;
        ss << "[" << aid << ", " << sym << "] ";
        return ss.str();
    }

    int aid, sym;
};

typedef set<attr_sym *> attr_sym_set;

struct Dummy {
    Dummy(int size, int length, attr_sym_set **events, int support, float gap_chance) : size(size), length(length),
                                                                                        events(events), support(support),
                                                                                        gapChance(gap_chance) {
    }

    bool contains_attr_sym(int ts, int aid, int sym) const {
        for (auto it : *events[ts])
            if (it->aid == aid && it->sym == sym)
                return true;
        return false;
    }

    string to_string() const {
        stringstream ss;
        ss << "sz: " << size << "\tlen: " << length;
        for (int l = 0; l < length; ++l) {
            ss << "{";
            auto it = events[l]->begin(), end = events[l]->end();
            while (it != end) {
                ss << (*it)->aid << "." << (*it)->sym;
                ++it;
                if (it != end) ss << ",";
            }
            ss << "}";
        }
        ss << "\tsup: " << support << "\tgapChance: " << gapChance;
        return ss.str();
    }

    int size, length, support;
    float gapChance;
    attr_sym_set **events;    //per timestep a pointer to a set of attr_sym
};

struct Parameters {
    string header() {
        return "date; runtime; ST size; CT size; Perc. of orignal; |CT|; |non_singletons|; #exact; #subset; #union_subset; #unrelated; minsup; #events; #attributes; alphabet_size; alphabet_size_per_attribute; #patterns; #patternfile; cnt_mat_pat; cnt_covers; cnt_acc; cnt_rej; cnt_acc_var; cnt_rej_var; cnt_infreq_materialized; cnt_infreq; input; output; gapvariants; blacklist; whitelist; prune_est_gain; prune_tree\n";
    }

    string to_string() {
        stringstream ss;
        string date = ctime(&start);
        date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

        ss << date << "; " << eind - start << "; " << STsize << "; " << CTsize << "; " << perc << "; " << length_CT
           << "; " << nr_non_singletons << "(";

        for (auto &it : *nr_non_singletons_per_size)
            ss << it.sz << "-" << it.usg << " ";
        ss << "); ";
        if (!dummy_file.empty())
            ss << cnt_exact << "; " << cnt_subset << "; " << cnt_union_subset << "; " << cnt_unrelated << "; ";
        else
            ss << "-; -; -; -;";
        ss << minsup << "; ";
        ss << nr_events << "; " << nr_of_attributes << "; " << alphabet_size << "; ";
        for (int a = 0; a < nr_of_attributes; ++a)
            ss << " " << alphabet_sizes[a];
        ss << "; " << nr_of_patterns << "; " << dummy_file << "; ";
        ss << cnt_mat_pat << "; " << cnt_covers << "; " << cnt_acc << "; " << cnt_rej << "; " << cnt_acc_var << "; "
           << cnt_rej_var << "; " << cnt_infreq_materialized << "; " << cnt_infreq << "; " << input_filename << "; "
           << output_filename << "; " << gapvariants << "; " << blacklist << "; " << whitelist << "; " << prune_est_gain
           << "; " << prune_tree << "; " << "\n";
        return ss.str();
    }

    string print() {
        stringstream ss;
        string date = ctime(&start);
        date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

        ss << "date: " << date << "\nruntime: " << eind - start << "\ninp: " << input_filename << "\noutp: "
           << output_filename << endl;
        ss << "STsize: " << STsize << "\nCTsize: " << CTsize << "\n% of orig: " << perc << "\n|CT|: " << length_CT
           << "\n |non_singletons|: " << nr_non_singletons << "(";

        for (auto it: *nr_non_singletons_per_size)
            ss << it.sz << "-" << it.usg << " ";

        ss << ")\n" << "Breakdown of found patterns, hidden: " << nr_of_patterns << " total found: "
           << nr_non_singletons << " Exact: " << cnt_exact << " Subset: " << cnt_subset << " Union_subset: "
           << cnt_union_subset << " unrelated: " << cnt_unrelated << endl;
        ss << "cnt_mat_pat: " << cnt_mat_pat << "  cnt_covers: " << cnt_covers << "  cnt_acc: " << cnt_acc << "  cnt_rej: "
           << cnt_rej << "  cnt_acc_var: " << cnt_acc_var << "  cnt_rej_var: " << cnt_rej_var << "  cntInfreqMat: "
           << cnt_infreq_materialized << "  cnt_infreq: " << cnt_infreq << "\n";
        return ss.str();
    }

    bool release,                //no system("pause");
    runtimes,                //print runtime info to file
    gapvariants,            //if a pattern is accepted we recursively try this candidate with all its gap events
    blacklist,                //contains all materialized patterns with sup=0
    whitelist,                //contains all materialized patterns
    prune_est_gain,            //do not consider candidates with estimated gain <= 0
    prune_tree,                //quickly discard new candidates based on minimum support
    FP_windows,                //to print the windows of the top of the code table to file to plot the occurences with python
    fill_patterns,            //to turn fill_pattern on or off
    *fill_pattern;            //for each attribute whether fillPatters are used. To make sure all (sub)streams are equally long they are padded with an extra symbol (last in alphabet) which is disregarded in computing size(D, CT)

    int minsup,                    //a minimum support threshold for candidate patterns
    nr_events,
            nr_of_attributes,
            temp_alphabet_size,        //for generated data we only specify the alphabet_size for a single attribute (similar for all attributes)
    alphabet_size,            //total over all attributes
    *alphabet_sizes,            //per attribute
    nr_of_patterns;            //synthetic patterns

    Sequence *seq;                    //the data


    // RESULTS
    int length_CT, nr_non_singletons, cnt_exact, cnt_subset, cnt_union_subset, cnt_unrelated;
    list<usg_sz> *nr_non_singletons_per_size;        //'usg' is the number of times a non-singleton of 'sz' occurs in the CT
    float STsize, CTsize, perc;

    ///////////////////////////////////////////////////
    ///				For debug output				///
    ///////////////////////////////////////////////////
    bool debug,                 //print debug info
    prune_check;                //reports when a pattern is pruned that was not part of the recently added pattern

    int cnt_mat_pat, cnt_acc, cnt_rej, cnt_acc_var, cnt_rej_var, cnt_infreq_materialized, cnt_infreq, cnt_covers; //DEBUG 22-10

    string output_filename,
            input_filename,
            dummy_file,
            output_dir;


    string pattern_in_sequences_file;
    string pattern_pos_in_sequences_file;
    string sequences_file;
    string winner_file;
    string pattern_file;
    string pattern_id_file;


    time_t start, eind;

    Dummy **dummies;                //a list of the inserted Dummy-patterns

    int pattern_window_index_min, pattern_window_index_max;
    int pattern_length_min, pattern_length_max;

    vector<int> attr_use;
};


#endif
