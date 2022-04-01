#ifndef MDL_FIRST_MINING_H
#define MDL_FIRST_MINING_H

#include "Common.h"
#include "Sequence.h"
#include "Pattern.h"
#include "CodeTable.h"
#include "Cover.h"
#include "Node.h"
#include "GetOpt.h"
#include "../base/BaseDS.h"
#include "../base/BaseAttr.h"


struct MDLParameters {
    bool FP_windows = false;
    bool fill_patterns = false;

    char * input_filename;
    char * dummy_filename;
    char * output_dir;

    int minsup = 10;
    int pattern_window_index_min = 0;
    int pattern_window_index_max = -1;
    int pattern_length_min = 0;
    int pattern_length_max = INF;

    vector<int> attr_use;

    char * pattern_in_sequences_file;
    char * pattern_pos_in_sequences_file;
    char * sequences_file;
    char * winner_file;
    char * pattern_file;
    char * pattern_id_file;
};

class mdl_enter {
public:
    explicit mdl_enter(Parameters *parameters, nlohmann::json& insert_patterns);
    explicit mdl_enter(Parameters *parameters, nlohmann::json& code_table, vector<int>& delete_patterns_id, nlohmann::json& insert_patterns);

    ~mdl_enter();

    Pattern *build_pattern(Pattern *a, Pattern *b, int offset);

    Pattern *build_interleaved_pattern(Pattern *singleton, Pattern *p, int pos_singleton);

    bool check_pattern_attribute_overlap(Pattern *a, Pattern *b, int offset);

    bool check_eventset_attribute_overlap(attribute_set *a, attribute_set *b);

    bool check_subset(Pattern *a, Pattern *b);

    bool check_subset(attribute_set *esa, attribute_set *esb);

    bool check_if_dummy(Pattern *p);

    bool combine_patterns(Pattern *a, Pattern *b, int total_usage, pattern_set *result);

    attribute_set *join_eventsets(attribute_set *a, attribute_set *b);

    void insert_candidates(pattern_set *list);

    usg_sz *postprune(Pattern *accepted, int total_usg, double new_size);

    pattern_set::iterator find_pattern_in_set(pattern_set *pset, Pattern *p);

    candpattern_set::iterator find_pattern_in_set(candpattern_set *pset, Pattern *p);

    prunepattern_set::iterator find_pattern_in_set(prunepattern_set *pset, Pattern *p);

    void break_down(Pattern *p);
    void generate_candidates(usagepattern_set::iterator *pt_ct_1, usagepattern_set::iterator *pt_ct_2,
                             usagepattern_set::iterator *pt_begin_ct, usagepattern_set::iterator *pt_end_ct,
                             usg_sz *current_usgSz);
    usg_sz *try_variations(Pattern *accepted, usg_sz *current_usgSz);

    void load_or_build_min_windows(Pattern *p);

    CodeTable *get_codeTable() { return ct; }

    Cover *get_cover() { return cover; }

    void save_debug_files(usg_sz *current_usgSz, double init_sz);
    void save_result_files();

    void insert_patterns(nlohmann::json& insert_patterns);

private:
    CodeTable *ct;

    candpattern_set *cand;            //ordered on estimated gain
    usagepattern_set *ct_on_usg;        //ordered on usage

    Cover *cover{};

    pattern_set *white_list;            //contains all considered candidates so we don't need to compute their support and min_windows again
    pattern_set *black_list;            //contains all considered candidates with sup = 0

    ostringstream output_stream;

    Node *root;

    Parameters *par;
};

void mdl_run(MDLParameters &arg, const string&, bool test_flag = false);

void mdl_modify_and_run(const string& old_model_pattern_filename, vector<int>& delete_patterns_id, const string& insert_patterns_filename, MDLParameters&);

#endif