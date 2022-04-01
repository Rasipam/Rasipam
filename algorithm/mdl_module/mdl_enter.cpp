#include "mdl_enter.h"


using namespace std;
using json = nlohmann::json;

void mdl_run(MDLParameters &arg, const string& insert_patterns_filename, bool test_flag) {
    Parameters parameters;
    parameters.release = true;

    parameters.runtimes = true;
    parameters.debug = false;

    parameters.gapvariants = true;
    parameters.prune_check = false;            //mag eruit
    parameters.blacklist = false;            //mag eruit
    parameters.whitelist = false;
    parameters.prune_est_gain = true;
    parameters.prune_tree = true;
    parameters.FP_windows = arg.FP_windows;            //to print the found patterns in the data AND to translate patterns from text-data
    parameters.fill_patterns = arg.fill_patterns;        //for aligned text-data


    ///////////////////////////////////////////////////
    ///				For debug output				///
    ///////////////////////////////////////////////////
    parameters.cnt_covers = 0;
    parameters.cnt_mat_pat = 0;
    parameters.cnt_acc = 0;
    parameters.cnt_rej = 0;
    parameters.cnt_acc_var = 0;
    parameters.cnt_rej_var = 0;
    parameters.cnt_infreq_materialized = 0;
    parameters.cnt_infreq = 0;


    ///////////////////////////////////////////////////
    ///				For Alg Setting  				///
    ///////////////////////////////////////////////////
    parameters.minsup = arg.minsup;
    parameters.dummy_file = arg.dummy_filename;
    parameters.input_filename = arg.input_filename;
    parameters.pattern_window_index_min = arg.pattern_window_index_min;
    parameters.pattern_window_index_max = arg.pattern_window_index_max;
    parameters.pattern_length_min = arg.pattern_length_min;
    parameters.pattern_length_max = arg.pattern_length_max;


    ///////////////////////////////////////////////////
    ///				For result output				///
    ///////////////////////////////////////////////////
//    parameters.pattern_in_sequences_file = arg.pattern_in_sequences_file;
//    parameters.pattern_pos_in_sequences_file = arg.pattern_pos_in_sequences_file;
//    parameters.sequences_file = arg.sequences_file;
//    parameters.winner_file = arg.winner_file;
    parameters.pattern_file = arg.pattern_file;
    parameters.attr_use = arg.attr_use;
//    parameters.pattern_id_file = arg.pattern_id_file;

    FILE *f;
    if (!(f = fopen(parameters.input_filename.c_str(), "r"))) {
        cout << "ERROR opening data file: " << parameters.input_filename << "\n";
        if (!parameters.release)
            system("pause");
        return;
    }
    parameters.seq = new Sequence(f, &parameters);
    fclose(f);

    if (parameters.seq->error_flag) {
        cout << "ERROR reading sequence or patterns!\n";
        if (!parameters.release)
            system("pause");
        return;
    }

    stringstream ss;
    ss << parameters.minsup;
    int strt = parameters.input_filename.find_last_of("/\\") + 1, nd = parameters.input_filename.find_last_of('.');
    string temp = parameters.input_filename.substr(strt, nd - strt);
    parameters.output_filename = string(arg.output_dir) + "/" + "debug_fileData_" + temp;
    parameters.output_dir = arg.output_dir;

    if (parameters.fill_patterns) {    //to make sure all (sub)streams are equally long they are padded with an extra symbol (last in alphabet) which is disregarded in computing size(D, CT)
        parameters.fill_pattern = new bool[parameters.nr_of_attributes];
        for (int attr = 0; attr < parameters.nr_of_attributes; ++attr)
            parameters.fill_pattern[attr] = true;
    }

    std::ifstream insert_patterns_stream(insert_patterns_filename, ios::in);
//    string insert_patterns_str;
    json insert_patterns;
    insert_patterns_stream >> insert_patterns;
//    json insert_patterns = json::parse(insert_patterns_str);
    puts("parse insert pattern json");

    clock_t t0 = clock();
    auto *mdl = new mdl_enter(&parameters, insert_patterns);
    cout << "mdl_module time: " << (clock() - t0)*1.0/CLOCKS_PER_SEC << "s" << endl;
    if (!test_flag) {
        mdl->save_result_files(5);
    }
    if (test_flag) {
        ofstream time_stream;
        time_stream.open("test_time.txt", ios::out);
        time_stream << (clock() - t0) * 1.0 / CLOCKS_PER_SEC;
        time_stream.close();
    }
    delete mdl;
}

mdl_enter::mdl_enter(Parameters *par, json& insert_patterns) : par(par) {
    output_stream << par->seq->print_sequence(false);      //DEBUG

    par->start = time(nullptr);
    par->cnt_exact = 0;
    par->cnt_subset = 0;
    par->cnt_union_subset = 0;
    par->cnt_unrelated = 0;

    //build a code_table
    ct = new CodeTable(par->seq);
    black_list = new pattern_set;
    white_list = new pattern_set;
    ct_on_usg = new usagepattern_set;        //set to combine CTxCT based on usage

    //build singletons
    int **tree_ids = par->seq->get_tree_ids();
    int nr_singletons = par->alphabet_size;
    int *alphabet_sizes = par->alphabet_sizes;
    for (int aid = 0; aid < par->nr_of_attributes; ++aid) {
        for (int sym = 0; sym < alphabet_sizes[aid]; ++sym) {
            auto **event_sets = new attribute_set *[1];
            event_sets[0] = new attribute_set;
            event_sets[0]->insert(new Attribute(sym, aid, 0, tree_ids[aid][sym]));
            auto *p = new Pattern(1, event_sets, par->seq);
            ct->insert_pattern(p);
            ct_on_usg->insert(p);
        }
    }

    //build a tree that represents all patterns of length == 1
    //each timestep in a new candidate is run through this tree to see if it still can be frequent
    root = new Node(nr_singletons, par->alphabet_sizes, 0);

    cand = new candpattern_set;    //NOTE: candidates are ordered based on estimated gain

    auto it_ct_1 = ct_on_usg->begin(), it_ct_2 = ct_on_usg->begin(), begin_ct = ct_on_usg->begin(), end_ct = ct_on_usg->end();    //iterators to update candidate list

    auto *g_cover = new Cover(par->seq, ct, false);    //determine ST size

    auto *current_usgSz = new usg_sz(g_cover->get_total_usage(), g_cover->get_sz_sequence_and_ct());

    double init_sz = current_usgSz->sz;

    output_stream << "\n\nST " << *current_usgSz << endl;//DEBUG
    output_stream << ct->print_ct(false);//DEBUG

    while (true) {
        if (it_ct_2 == end_ct &&
        cand->empty())                //we stop when there are no more candidates to generate. NOTE it_ct_2 reaches end first
            break;
        //update candidates
        generate_candidates(&it_ct_1, &it_ct_2, &begin_ct, &end_ct, current_usgSz);
        if (cand->empty())
            continue;

        //Add top candidate
        Pattern *top = *cand->begin();
        cand->erase(
                top);                            //do not consider this candidate again. NOTE: it must also be erased from cand to not be compared to as best candidate

        if (par->prune_est_gain && top->get_estimated_gain() <= 0)
            continue;
        load_or_build_min_windows(top);

        //prune on minsup
        if (top->get_support() < par->minsup)
            continue;

        if (!ct->insert_pattern(
                top))                //pattern already present. NOTE this check must be after set_min_windows, because pattern equality is also based on support
            continue;                                //already present

        //Cover
        g_cover = new Cover(par->seq, ct, false);
        double new_size = g_cover->get_sz_sequence_and_ct();
        int new_total_usage = g_cover->get_total_usage();

        //Check improvement
        if (new_size < current_usgSz->sz) {
            current_usgSz = postprune(top, new_total_usage, new_size);                //post acceptance pruning
            if (par->gapvariants)                                                    //recursively try variations of top+singleton
                current_usgSz = try_variations(top, current_usgSz);

            //rebuild ct_on_usg because usages have changed
            ct_on_usg->clear();
            for (auto it_ct : *ct->get_ct())
                ct_on_usg->insert(it_ct);

            //add CTxCT to candidates
            cand->clear();
            end_ct = ct_on_usg->end();
            begin_ct = ct_on_usg->begin();

            it_ct_1 = codeTable_set::iterator(begin_ct);            //hard copy
            it_ct_2 = codeTable_set::iterator(begin_ct);            //hard copy

            par->cnt_acc++;
            output_stream << "top accepted: ";
            output_stream << top->print(false);//DEBUG
        } else {
            par->cnt_rej++;
            ct->delete_pattern(top);
            ct->rollback();            //we need to rollback because all usages must be correct before we can generate more candidates
        }

    }


    ct->delete_unused_patterns();            //Only at the very end

    this->insert_patterns(insert_patterns);

    cover = new Cover(par->seq, ct, false);
    save_debug_files(current_usgSz, init_sz);
//    save_result_files();
}


mdl_enter::mdl_enter(Parameters *par, json& code_table, vector<int>& delete_patterns_id, json& insert_patterns) : par(par) {
    par->start = time(nullptr);
    par->cnt_exact = 0;
    par->cnt_subset = 0;
    par->cnt_union_subset = 0;
    par->cnt_unrelated = 0;
    ct = new CodeTable(par->seq);
    ct_on_usg = new usagepattern_set;        //set to combine CTxCT based on usage
    cand = new candpattern_set;
    black_list = new pattern_set;
    white_list = new pattern_set;

    //build singletons
    int **tree_ids = par->seq->get_tree_ids();
    int nr_singletons = par->alphabet_size;
    int *alphabet_sizes = par->alphabet_sizes;
    for (int aid = 0; aid < par->nr_of_attributes; ++aid) {
        for (int sym = 0; sym < alphabet_sizes[aid]; ++sym) {
            auto **event_sets = new attribute_set *[1];
            event_sets[0] = new attribute_set;
            event_sets[0]->insert(new Attribute(sym, aid, 0, tree_ids[aid][sym]));
            auto *p = new Pattern(1, event_sets, par->seq);
            ct->insert_pattern(p);
            ct_on_usg->insert(p);
        }
    }

    for (auto & pattern : code_table) {
        json p = pattern["hits"];
        auto **event_sets = new attribute_set *[p.size()];
        int event_id = 0;
        for (json::iterator event_it = p.begin(); event_it != p.end(); ++event_it, ++event_id) {
            event_sets[event_id] = new attribute_set;
            int attr_id = 0;
            for (json::iterator attr_it = event_it->begin(); attr_it != event_it->end(); ++attr_it, ++attr_id) {
                if (*attr_it == "")
                    continue;
                int sym = BaseAttr::from_key_value(BaseAttr::get_keys()[attr_id], *attr_it);
                event_sets[event_id]->insert(new Attribute(sym, attr_id, event_id, tree_ids[attr_id][sym]));
            }
        }
        auto *p_ct = new Pattern(p.size(), event_sets, par->seq, (int)pattern["id"]);
        ct->insert_pattern(p_ct);
        Pattern::id_top = max(Pattern::id_top, (int)pattern["id"]);
    }

    auto *old_g_cover = new Cover(par->seq, ct, false);
    auto *old_usgSz = new usg_sz(old_g_cover->get_total_usage(), old_g_cover->get_sz_sequence_and_ct());

    for (auto &p : *(ct->get_ct())) {
        if (find(delete_patterns_id.begin(), delete_patterns_id.end(), p->get_id()) != delete_patterns_id.end()) {
            ct->delete_pattern(p);
            break;
        }
    }
    this->insert_patterns(insert_patterns);

    cover = new Cover(par->seq, ct, false);    //determine ST size
    auto *current_usgSz = new usg_sz(cover->get_total_usage(), cover->get_sz_sequence_and_ct());
    save_debug_files(current_usgSz, old_usgSz->sz);
    save_result_files();
}


void mdl_enter::insert_patterns(json& insert_patterns) {
    int **tree_ids = par->seq->get_tree_ids();
    for (auto & insert_pattern : insert_patterns) {
        auto **event_sets = new attribute_set *[insert_pattern.size()];
        int event_id = 0;
        for (json::iterator event_it = insert_pattern.begin(); event_it != insert_pattern.end(); ++event_it, ++event_id) {
            event_sets[event_id] = new attribute_set;
            int attr_id = 0;
            for (json::iterator attr_it = event_it->begin(); attr_it != event_it->end(); ++attr_it, ++attr_id) {
                if (*attr_it == "")
                    continue;
                int sym = BaseAttr::from_key_value(BaseAttr::get_keys()[attr_id], *attr_it);
                event_sets[event_id]->insert(new Attribute(sym, attr_id, event_id, tree_ids[attr_id][sym]));
            }
        }
        auto *p = new Pattern(insert_pattern.size(), event_sets, par->seq);
        if (!ct->find_pattern(p))
            ct->insert_pattern(p);
    }
}


void mdl_modify_and_run(const string& old_model_pattern_filename, vector<int>& delete_patterns_id, const string& insert_patterns_filename, MDLParameters &arg) {
    Parameters parameters;
    parameters.release = true;

    parameters.runtimes = true;
    parameters.debug = false;

    parameters.gapvariants = true;
    parameters.prune_check = false;            //mag eruit
    parameters.blacklist = false;            //mag eruit
    parameters.whitelist = false;
    parameters.prune_est_gain = true;
    parameters.prune_tree = true;
    parameters.FP_windows = arg.FP_windows;            //to print the found patterns in the data AND to translate patterns from text-data
    parameters.fill_patterns = arg.fill_patterns;        //for aligned text-data


    ///////////////////////////////////////////////////
    ///				For debug output				///
    ///////////////////////////////////////////////////
    parameters.cnt_covers = 0;
    parameters.cnt_mat_pat = 0;
    parameters.cnt_acc = 0;
    parameters.cnt_rej = 0;
    parameters.cnt_acc_var = 0;
    parameters.cnt_rej_var = 0;
    parameters.cnt_infreq_materialized = 0;
    parameters.cnt_infreq = 0;

    ///////////////////////////////////////////////////
    ///				For Alg Setting  				///
    ///////////////////////////////////////////////////
    parameters.minsup = arg.minsup;
    parameters.dummy_file = arg.dummy_filename;
    parameters.input_filename = arg.input_filename;
    parameters.pattern_window_index_min = arg.pattern_window_index_min;
    parameters.pattern_window_index_max = arg.pattern_window_index_max;
    parameters.pattern_length_min = arg.pattern_length_min;
    parameters.pattern_length_max = arg.pattern_length_max;

    ///////////////////////////////////////////////////
    ///				For result output				///
    ///////////////////////////////////////////////////
    parameters.pattern_file = arg.pattern_file;
    parameters.attr_use = arg.attr_use;

    FILE *f;
    if (!(f = fopen(parameters.input_filename.c_str(), "r"))) {
        cout << "ERROR opening data file: " << parameters.input_filename << "\n";
        if (!parameters.release)
            system("pause");
        return;
    }
    parameters.seq = new Sequence(f, &parameters);
    fclose(f);

    if (parameters.seq->error_flag) {
        cout << "ERROR reading sequence or patterns!\n";
        if (!parameters.release)
            system("pause");
        return;
    }

    int strt = parameters.input_filename.find_last_of("/\\") + 1, nd = parameters.input_filename.find_last_of('.');
    string temp = parameters.input_filename.substr(strt, nd - strt);
    parameters.output_filename = string(arg.output_dir) + "/" + "debug_fileData_" + temp;
    parameters.output_dir = arg.output_dir;

    if (parameters.fill_patterns) {    //to make sure all (sub)streams are equally long they are padded with an extra symbol (last in alphabet) which is disregarded in computing size(D, CT)
        parameters.fill_pattern = new bool[parameters.nr_of_attributes];
        for (int attr = 0; attr < parameters.nr_of_attributes; ++attr)
            parameters.fill_pattern[attr] = true;
    }
    std::ifstream old_model_pattern_stream(old_model_pattern_filename, ios::in);
//    string old_model_pattern_str;
    json old_model_patterns;
    old_model_pattern_stream >> old_model_patterns;
//    json old_model_patterns = json::parse(old_model_pattern_str);

    std::ifstream insert_patterns_stream(insert_patterns_filename, ios::in);
//    string insert_patterns_str;
    json insert_patterns;
    insert_patterns_stream >> insert_patterns;
    puts("parse json");


    clock_t t0 = clock();
    auto* new_mdl_model = new mdl_enter(&parameters, old_model_patterns["patterns"], delete_patterns_id, insert_patterns);
    cout << "mdl_module time: " << (clock() - t0)*1.0/CLOCKS_PER_SEC << "s" << endl;
    delete new_mdl_model;
}


void mdl_enter::save_debug_files(usg_sz *current_usgSz, double init_sz) {

    double STsize = current_usgSz->sz;

    cout << "nr seq: " << par->seq->get_nr_sequences() << endl;
    double s_len_avg = 0;
    for (int i = 0; i < par->seq->get_nr_sequences(); ++i) {
        s_len_avg += par->seq->get_sequence_sizes()[i];
    }
    cout << "seq len avg: " << s_len_avg / par->seq->get_nr_sequences() << endl;
    cout << "nr Attribute: " << par->nr_of_attributes << endl;
    int event_len = 0;
    for (int i = 0; i < par->nr_of_attributes; ++i) {
        event_len += par->alphabet_sizes[i];
    }
    cout << "event len: " << event_len << endl;
    int ct_no_singleton_cnt = 0, ct_len_avg = 0;
    for (auto & p : *(ct->get_ct())) {
        ct_len_avg += p->get_size();
        if (p->get_size() > 1) {
            ++ct_no_singleton_cnt;
        }
    }
    cout << "P num: " << ct_no_singleton_cnt << " P len avg: " << (double)ct_len_avg / ct->get_ct()->size() << endl;
    double now_L = cover->get_sz_sequence_and_ct();

    cout << "percent of delta L: " << (init_sz - now_L) / init_sz * 100.0 << endl;
    //compute breakdown results for found patterns
    if (!par->dummy_file.empty()) {
        for (auto ct_it : *ct->get_ct()) {
            if (ct_it->get_size() != 1)            //only consider non-singletons
                break_down(ct_it);
        }

        //print the dummies for debug purpose
        for (int i = 0; i < par->nr_of_patterns; ++i)
            output_stream << "Dummy " << i << ": " << par->dummies[i]->to_string() << endl;
    }

    output_stream << ct->print_ct(false);    //NOTE: after search for Dummy patterns

    par->length_CT = ct->get_ct_length();
    par->nr_non_singletons = ct->get_ct_length() - par->alphabet_size;
    par->nr_non_singletons_per_size = ct->get_nr_non_singletons_per_size();

    par->STsize = STsize;
    par->CTsize = cover->get_sz_sequence_and_ct();
    par->perc = par->CTsize / par->STsize;
    par->eind = time(nullptr);

    FILE *p_file = nullptr;
    if (par->FP_windows) {
        //print FP_windows to file
        cout << "output" << endl;
        FILE *fp_file = nullptr;
        int cnt_fp = 0;
        fp_file = fopen((par->output_filename + "_FP_windows.txt").c_str(), "w");
        if (fp_file != nullptr) {
            stringstream result, result2;
            for (auto p : *ct->get_ct()) {
                if (p->get_usage() > 0 && p->get_size() > 1)        //all used non-singletons
                {
                    result << p->print_fp_windows();
                    cnt_fp++;
                }
            }
            result2 << cnt_fp << "\n" << result.str();
            fprintf(fp_file, "%s", result2.str().c_str());
            fclose(fp_file);
        }
    }
    if (par->release) {
        //print output data to file
        p_file = fopen((par->output_filename + ".txt").c_str(), "w");
        if (p_file != nullptr) {
            if (par->runtimes)
                output_stream << par->print();
            fprintf(p_file, "%s", output_stream.str().c_str());
            fclose(p_file);
        }
    } else {
        cout << output_stream.str();
        if (par->runtimes)
            cout << par->print();
    }

    if (par->runtimes) {
        bool exist = false;
        if (FILE *testFile = fopen((par->output_filename + "_runtimes.csv").c_str(), "r")) {
            fclose(testFile);
            exist = true;
        }

        FILE *runFile = fopen((par->output_filename + "_runtimes.csv").c_str(), "a");
        if (runFile != nullptr) {
            if (!exist)
                fprintf(runFile, "%s", par->header().c_str());

            fprintf(runFile, "%s", par->to_string().c_str());
            fclose(runFile);
        }
    }
}


void mdl_enter::save_result_files(int min_sup) {
    string pattern_file = par->output_dir + "/" + par->pattern_file;
    ofstream pattern_stream;
    pattern_stream.open(pattern_file, ios::out);

    json patterns = vector<int>(0);
    json patterns_in_sequences;
    json code_table;


    map<Pattern *, int> pat_id;
    int id_top = 0;
    json sequence = vector<int>(0), sequences;
    for (int i = 0, seq_i = 0; i < par->nr_events; ++i, ++seq_i) {
        auto events = par->seq->mev_time[i];
        for (int j = 0; j < events->get_size(); ++j) {
            if (events->is_covered[j]->get_length() <= 1 || events->is_covered[j]->get_usage() < min_sup) {
                continue;
            }
            if (pat_id.find(events->is_covered[j]) == pat_id.end()) {
                json hits;
                for (int ts = 0; ts < events->is_covered[j]->get_length(); ++ts) {
                    auto e = events->is_covered[j]->get_symbols(ts);
                    json hit(vector<string>(par->nr_of_attributes));
                    for (auto a : *e) {
                        hit[a->attribute] = BaseAttr::get_attrs(BaseAttr::get_keys()[a->attribute])[a->symbol];
                    }
                    hits.push_back(hit);
                }
                json pattern;
                pattern["hits"] = hits;
                pattern["id"] = events->is_covered[j]->get_id();
                patterns.push_back(pattern);
                pat_id[events->is_covered[j]] = id_top++;
            }
            json pattern_occ;
            pattern_occ["pattern_index"] = pat_id[events->is_covered[j]];
            pattern_occ["pattern_position"] = seq_i;
            sequence.push_back(pattern_occ);
            i += events->is_covered[j]->get_length() - 1;
            seq_i += events->is_covered[j]->get_length() - 1;
            break;
        }
        if (i + 1 == par->nr_events || par->seq->mev_time[i + 1]->seqid != par->seq->mev_time[i]->seqid) {
            seq_i = -1;
            sequences.push_back(sequence);
            sequence.clear();
        }
    }
    for (auto & p : *(ct->get_ct())) {
        json hits;
        for (int ts = 0; ts < p->get_length(); ++ts) {
            auto e = p->get_symbols(ts);
            json hit(vector<string>(par->nr_of_attributes));
            for (auto a : *e) {
                hit[a->attribute] = BaseAttr::get_attrs(BaseAttr::get_keys()[a->attribute])[a->symbol];
            }
            hits.push_back(hit);
        }
        json pattern;
        pattern["hits"] = hits;
        pattern["id"] = p->get_id();
        code_table.push_back(pattern);
    }
    json output_json;
    output_json["desc_len"] = cover->get_sz_sequence_and_ct();
    output_json["patterns"] = patterns;
    output_json["patterns_in_sequences"] = sequences;
    output_json["code_table"] = code_table;
    pattern_stream << output_json.dump();

    pattern_stream.close();
}


void mdl_enter::break_down(Pattern *p) {
    //FOR EXACT MATCH - loop through all dummies
    bool exact = false;
    for (int i = 0; i < par->nr_of_patterns; ++i) {
        Dummy *d = par->dummies[i];
        exact = true;

        if (d->length != p->get_length() || d->size != p->get_size())
            continue;

        for (int l = 0; l < d->length; ++l) {
            if (p->get_height_at_pos(l) != d->events[l]->size())        //different height at timestep l
                exact = false;
            else {
                for (auto it = p->get_symbols(l)->begin(), end = p->get_symbols(l)->end(); it != end; ++it) {
                    if (!d->contains_attr_sym(l, (*it)->attribute, (*it)->symbol)) {
                        exact = false;
                        break;
                    }
                }
            }
            if (!exact)
                break;
        }
        if (exact) {
            stringstream ss;
            ss << i;
            p->set_info("Exact " + ss.str());
            par->cnt_exact++;
            return;
        }
    }

    //FOR SUBSET MATCH -> alle multi-events van het patroon komen in dezelfde volgorde voor in een Dummy-patroon
    for (int i = 0; i < par->nr_of_patterns; ++i) {
        Dummy *d = par->dummies[i];
        int dummy_ts = 0;
        bool subset;
        //every timestep must be found completely in the same order
        //loop through all timesteps in the pattern
        for (int l = 0; l < p->get_length(); ++l) {
            subset = false;
            while (dummy_ts < d->length) {
                bool complete_timestep = true;
                attribute_set *evs = p->get_symbols(l);
                for (auto ev : *evs) {
                    bool complete_symbol = false;
                    attr_sym_set *as = d->events[dummy_ts];
                    for (auto a : *as)
                        if (ev->attribute == a->aid && ev->symbol == a->sym) {
                            complete_symbol = true;
                            break;
                        }
                    if (!complete_symbol) {
                        complete_timestep = false;
                        break;        //try next timestep in Dummy
                    }
                }
                dummy_ts++;
                if (complete_timestep) {
                    subset = true;
                    break;
                }
            }
            if (!subset)
                break;
        }

        if (subset) {
            stringstream ss;
            ss << i;
            p->set_info("Subset of " + ss.str());
            par->cnt_subset++;
            return;
        }
    }

    //FOR UNION_SUBSET MATCH -> alle events komen in ��n Dummy in verkeerde volgorde voor OF alle events komen in een (verschillende) Dummy voor
    bool union_subset = true;
    for (int l = 0; l < p->get_length(); ++l) {
        attribute_set *evs = p->get_symbols(l);
        for (auto ev : *evs) {
            //check for every event in the pattern if it occurs in one of the dummies
            bool found_event = false;
            for (int i = 0; i < par->nr_of_patterns; ++i) {
                //loop over all dummies to find this event
                Dummy *d = par->dummies[i];
                for (int dummy_ts = 0; dummy_ts < d->length; ++dummy_ts) {
                    attr_sym_set *as = d->events[dummy_ts];
                    for (auto a : *as)
                        if (ev->attribute == a->aid && ev->symbol == a->sym) {
                            found_event = true;
                            break;
                        }
                    if (found_event)
                        break;
                }
                if (found_event)
                    break;
            }
            if (!found_event) {
                //this event is not found in all dummies
                union_subset = false;
                break;
            }
        }
        if (!union_subset)
            break;
    }
    if (union_subset) {
        p->set_info("Union Subset");
        par->cnt_union_subset++;
        return;
    }

    //if not exact or subset or union_subset it is unrelated
    p->set_info("Unrelated");
    par->cnt_unrelated++;

}

//Return true when p is one of the inserted synthetic patterns
bool mdl_enter::check_if_dummy(Pattern *p) {
    bool match;
    for (int i = 0; i < par->nr_of_patterns; ++i) {
        Dummy *d = par->dummies[i];
        match = true;

        if (d->length != p->get_length() || d->size != p->get_size())
            continue;

        for (int l = 0; l < d->length; ++l) {
            if (p->get_height_at_pos(l) != d->events[l]->size())        //different height at timestep l
                match = false;
            else
                for (auto it = p->get_symbols(l)->begin(), end = p->get_symbols(l)->end(); it != end; ++it) {
                    if (!d->contains_attr_sym(l, (*it)->attribute, (*it)->symbol)) {
                        match = false;
                        break;
                    }
                }
            if (!match) break;
        }
        if (match) return true;
    }
    return false;
}

void mdl_enter::load_or_build_min_windows(Pattern *p) {
    bool compute_min_windows = true;
    if (par->blacklist) {
        auto fnd = black_list->find(p), end_black = black_list->end();
        if (fnd != end_black) {
            p->load_windows_and_support((*fnd));
            compute_min_windows = false;
        }
    }
    if (compute_min_windows && par->whitelist) {
        auto fnd = white_list->find(p), end_white = white_list->end();
        if (fnd != end_white) {
            p->load_windows_and_support((*fnd));
            compute_min_windows = false;
        }
    }
    if (compute_min_windows) {
        par->cnt_mat_pat++;

        p->set_min_windows(
                par->seq);                    //only compute the min_windows when we actually consider the pattern

        if (p->get_support() < par->minsup)
            par->cnt_infreq_materialized++;

        if (par->prune_tree && p->get_support() < par->minsup)
            root->add_infrequent_pattern(p, p->get_symbols(0)->begin(), p->get_symbols(0)->end());

        if (par->whitelist)
            white_list->insert(p);                    //so we never have to compute its support and min_windows again
        if (par->blacklist && p->get_support() == 0)
            black_list->insert(p);                    //so we never have to compute its support and min_windows again
    }
}


usg_sz *mdl_enter::try_variations(Pattern *accepted, usg_sz *current_usgSz) {
    auto *temp_cand = new pattern_set;                    //so we don't try the same variation more often for the same pattern
    auto temp_end = temp_cand->end();

    Event **mev_time = par->seq->get_mev_time();

    int new_length = accepted->get_length() + 1;

    list<Window *> *minWin = accepted->get_min_windows(par->seq, false);

    for (auto w : *minWin) { //loop over its minimum windows
        if (w->active && w->get_gap_length() > 0) { //minwindows hebben max patternlength-1 gaps
            int start_id = w->get_mev_position(0)->id;
            set<int> *gaps = w->get_gaps();

            for (int gap : *gaps) { //loop over all gap-positions
                attribute_set *gap_events = mev_time[gap]->get_events();
                for (auto ev : *gap_events) { //loop over all events at the gap-position               {
                    int gap_cnt = 0;
                    int gap_ID = mev_time[gap]->id;
                    int gap_position = 0;
                    for (int l = 0; l < accepted->get_length(); ++l) {
                        if (w->get_mev_position(l)->id < gap_ID)
                            gap_position++;
                        else
                            break;
                    }
                    auto **event_sets = new attribute_set *[new_length];
                    for (int l = 0; l < new_length; ++l) {
                        if (l == gap_position) {
                            gap_cnt = 1;
                            event_sets[l] = new attribute_set;
                            event_sets[l]->insert(ev);
                        } else
                            event_sets[l] = new attribute_set(*accepted->get_symbols(l - gap_cnt)); //hard copy
                    }
                    auto *newp = new Pattern(new_length, event_sets, par->seq, accepted,
                                             nullptr);        //we call this constructor because it doesn't build the min_windows yet
                    //prune on minsup
                    bool delete_pattern = false;
                    if (par->prune_tree) {
                        //find_pattern returns true when the pattern or any prefix of it is known to be infrequent, we call this method with the pattern p starting from all its timesteps to consider all subpatterns
                        for (int startPos = 0; startPos < newp->get_length(); ++startPos) {
                            if (root->find_pattern(newp, startPos, newp->get_symbols(0)->begin(),
                                                   newp->get_symbols(0)->end())) {
                                par->cnt_infreq++;
                                delete_pattern = true;
                                break;        //if the pattern from one of the startPositions is found then we know enough
                            }
                        }
                    }

                    if (!delete_pattern && find_pattern_in_set(temp_cand, newp) ==
                                           temp_end)                                //only when not already tried
                    {
                        temp_cand->insert(newp);
                        load_or_build_min_windows(newp);

                        if (newp->get_support() < par->minsup) {
                            if (par->prune_tree)//add to prune tree
                                root->add_infrequent_pattern(newp, newp->get_symbols(0)->begin(),
                                                             newp->get_symbols(0)->end());
                        } else {
                            if (ct->insert_pattern(
                                    newp))                    //NOTE this check must be after set_min_windows, because pattern equality is also based on support
                            {
                                cover = new Cover(par->seq, ct, false);
                                double new_size = cover->get_sz_sequence_and_ct();
                                int new_total_usage = cover->get_total_usage();
                                if (new_size < current_usgSz->sz)                                    //Check improvement
                                {
                                    par->cnt_acc_var++;
                                    current_usgSz = postprune(newp, new_total_usage,
                                                              new_size);        //post acceptance pruning
                                    current_usgSz = try_variations(newp,
                                                                   current_usgSz);                //recursively try variations of newp+singleton
                                } else {
                                    par->cnt_rej_var++;
                                    ct->delete_pattern(newp);
                                }
                            }
                        }
                    } else
                        delete newp;
                }
            }
        }
    }

    delete temp_cand;
    return current_usgSz;
}

void mdl_enter::generate_candidates(usagepattern_set::iterator *pt_ct_1, usagepattern_set::iterator *pt_ct_2,
                                    usagepattern_set::iterator *pt_begin_ct, usagepattern_set::iterator *pt_end_ct,
                                    usg_sz *current_usgSz) {
    //order: 1x1, 1x2, 2x2, 1x3, 2x3, 3x3, 1x4, 2x4, 3x4, 4x4, 1x5, ..
    bool stop = false;
    while (*pt_ct_2 != *pt_end_ct)        //it_ct_2 is the first to reach the end
    {
        //if one of the patterns is a fill-pattern we do not combine it
        if (par->fill_patterns) {
            if ((**pt_ct_1)->get_size() == 1 &&
                par->fill_pattern[(*(**pt_ct_1)->get_symbols(0)->begin())->attribute] &&
                (*(**pt_ct_1)->get_symbols(0)->begin())->symbol ==
                par->alphabet_sizes[(*(**pt_ct_1)->get_symbols(0)->begin())->attribute] - 1) {
                ++*pt_ct_1;
                continue;
            }
            if ((**pt_ct_2)->get_size() == 1 &&
                par->fill_pattern[(*(**pt_ct_2)->get_symbols(0)->begin())->attribute] &&
                (*(**pt_ct_2)->get_symbols(0)->begin())->symbol ==
                par->alphabet_sizes[(*(**pt_ct_2)->get_symbols(0)->begin())->attribute] - 1) {
                ++*pt_ct_2;
                continue;
            }
        }

        //if one of the patterns support is below minsup we do not combine
        if ((**pt_ct_1)->get_support() < par->minsup) {
            ++*pt_ct_1;
            continue;
        }
        if ((**pt_ct_2)->get_support() < par->minsup) {
            ++*pt_ct_2;
            continue;
        }

        auto *result = new pattern_set;
        stop = combine_patterns(**pt_ct_1, **pt_ct_2, current_usgSz->usg, result);
        if (stop)
            break;

        insert_candidates(result);

        if (*pt_ct_1 == *pt_ct_2) {
            *pt_ct_1 = usagepattern_set::iterator(*pt_begin_ct);    //hard copy
            ++*pt_ct_2;
        } else
            ++*pt_ct_1;
    }

}

//Only adds the patterns if they are not already present
void mdl_enter::insert_candidates(pattern_set *list) {
    auto it = list->begin(), end = list->end();
    while (it != end) {
        Pattern *p = *it;
        bool delete_pattern = false;

        if (par->prune_tree) {
            //find_pattern returns true when the pattern or any prefix of it is known to be infrequent, we call this method with the pattern p starting from all its timesteps to consider all subpatterns
            for (int startPos = 0; startPos < p->get_length(); ++startPos) {
                if (root->find_pattern(p, startPos, p->get_symbols(startPos)->begin(),
                                       p->get_symbols(startPos)->end())) {
                    par->cnt_infreq++;
                    delete_pattern = true;
                    break;        //if the pattern from one of the startPositions is found then we know enough
                }
            }
        }
        if (!delete_pattern) {
            auto candidate_end = cand->end();
            if (find_pattern_in_set(cand, p) == candidate_end)
                cand->insert(p);
            else
                delete_pattern = true;
        }
        if (delete_pattern) {

            delete p;
        }
        ++it;
    }
}


//Returns true when we do not combine x and y because of their usage compared to bestCand
//Returns a list of patterns constructed from a and b via the pattern_set* result
bool mdl_enter::combine_patterns(Pattern *a, Pattern *b, int total_usage, pattern_set *result) {
    if (!cand->empty() && (a->get_usage() < (*cand->begin())->get_estimated_usage() ||
            b->get_usage() < (*cand->begin())->get_estimated_usage()))
        return true;

    int usg_z, usg_x, usg_y;

    //check whether they specify values for a similar attribute
    bool sim_attr = false;
    set<int> *set_x = a->get_total_aids(), *set_y = b->get_total_aids();
    auto endY = set_y->end();
    for (int it : *set_x) {
        if (set_y->find(it) != endY) {
            sim_attr = true;
            break;
        }
    }

    for (int offset = 0; offset < a->get_length() + b->get_length() + 1; ++offset) {
        if (!sim_attr || !check_pattern_attribute_overlap(a, b, offset)) {
            Pattern *newp = build_pattern(a, b, offset);
            usg_x = newp->get_x()->get_usage();
            usg_y = newp->get_y()->get_usage();
            usg_z = min(usg_x, usg_y);                        //estimated usage
            newp->set_estimated_gain(newp->compute_estimated_gain(usg_x, usg_y, usg_z, total_usage));
            result->insert(newp);
        }
    }

    return false;
}

//RETURN true when eventsets a and b contain a similar attribute
bool mdl_enter::check_eventset_attribute_overlap(attribute_set *a, attribute_set *b) {
    bool result = false;
    bool *present_attributes = new bool[par->nr_of_attributes];
    for (int i = 0; i < par->nr_of_attributes; ++i)
        present_attributes[i] = false;

    for (auto ita : *a)  //one eventset can not contain duplicate attributes
        present_attributes[ita->attribute] = true;
    for (auto itb : *b) {
        if (present_attributes[itb->attribute]) {  //check if the attribute was already seen in 'a'
            result = true;
            break;
        }
        present_attributes[itb->attribute] = true;
    }
    delete[]present_attributes;
    return result;
}

//RETURN true when Patterns a and b overlap when combined with the given offset
bool mdl_enter::check_pattern_attribute_overlap(Pattern *a, Pattern *b,
                                                int offset) {    //	  Offset:	0		1		2		3		4		5
    // Pattern A:   xxx		xxx		xxx		xxx		xxx		xxx
    // Pattern B: xx	   xx		xx		 xx		  xx	   xx
    //	  length:	5		4		3		3		4		5
    // start_pos_a:	2		1		0		0		0		0
    // start_pos_b:	0		0		0		1		2		3

    //	  Offset:	0		1		2		3
    // Pattern A:   x		x		x		x
    // Pattern B: xx	   xx		xx		 xx
    //	  length:	3		2		2		3
    // start_pos_a:	2		1		0		0
    // start_pos_b:	0		0		0		1

    //	  Offset:	0		1		2		3
    // Pattern A:   xx		xx		xx 		xx
    // Pattern B:  x	    x		 x		  x
    //	  length:	3		2		2		3
    // start_pos_a:	1		0		0		0
    // start_pos_b:	0		0		1		2


    if (offset == 0 || offset == a->get_length() + b->get_length())    //one pattern completely before the other
        return false;

    int start_pos_a = max(0, b->get_length() - offset), start_pos_b = max(0, offset - b->get_length());
    int new_length;
    if (start_pos_b == 0)
        new_length = max(start_pos_a + a->get_length(), b->get_length());
    else
        new_length = max(start_pos_b + b->get_length(), a->get_length());
    for (int pos = 0; pos < new_length; ++pos) {
        if (pos >= start_pos_a && pos >= start_pos_b && pos - start_pos_a < a->get_length() &&
            pos - start_pos_b < b->get_length())
            if (check_eventset_attribute_overlap(a->get_symbols(pos - start_pos_a), b->get_symbols(pos - start_pos_b)))
                return true;
    }
    return false;
}

attribute_set *mdl_enter::join_eventsets(attribute_set *a, attribute_set *b) {
    auto *result = new attribute_set;
    for (auto ita : *a)
        result->insert(ita);
    for (auto itb : *b)
        result->insert(itb);
    return result;
}

//construct a new pattern from a singleton and another pattern, with the singleton at the specified position
Pattern *mdl_enter::build_interleaved_pattern(Pattern *singleton, Pattern *p, int pos_singleton) {
    int new_length = p->get_length() + 1;
    int pos_p = 0;
    auto **event_sets = new attribute_set *[new_length];
    for (int pos = 0; pos < new_length; ++pos) {
        if (pos == pos_singleton)
            event_sets[pos] = new attribute_set(*singleton->get_symbols(0)); //hard copy
        else
            event_sets[pos] = new attribute_set(*p->get_symbols(pos_p++)); //hard copy
    }
    return new Pattern(new_length, event_sets, par->seq, singleton, p);
}

//construct a new pattern from a and b with given offset (already checked that there is no overlap)
Pattern *mdl_enter::build_pattern(Pattern *a, Pattern *b, int offset) {
    Pattern *newp;

    int start_pos_a = max(0, b->get_length() - offset), start_pos_b = max(0, offset - b->get_length());
    int new_length;
    if (start_pos_b == 0)
        new_length = max(start_pos_a + a->get_length(), b->get_length());
    else
        new_length = max(start_pos_b + b->get_length(), a->get_length());

    auto **event_sets = new attribute_set *[new_length];
    for (int pos = 0; pos < new_length; ++pos) {
        if (pos >= start_pos_a && pos >= start_pos_b && pos - start_pos_a < a->get_length() &&
            pos - start_pos_b < b->get_length())
            event_sets[pos] = join_eventsets(a->get_symbols(pos - start_pos_a), b->get_symbols(pos - start_pos_b));
        else if (pos >= start_pos_a && pos - start_pos_a < a->get_length())
            event_sets[pos] = new attribute_set(*a->get_symbols(pos - start_pos_a)); //hard copy
        else if (pos >= start_pos_b && pos - start_pos_b < b->get_length())
            event_sets[pos] = new attribute_set(*b->get_symbols(pos - start_pos_b)); //hard copy
    }
    newp = new Pattern(new_length, event_sets, par->seq, a, b);
    return newp;
}


usg_sz *mdl_enter::postprune(Pattern *accepted, int total_usg, double current_size) {
    auto *pruneset = new prunepattern_set;
    for (auto p : *ct->get_ct()) {
        if (p->get_size() > 1)//singletons can't be pruned
            if (p->get_usage_decreased())
                pruneset->insert(p);
    }
    while (!pruneset->empty()) {
        //prune the top
        auto it_top = pruneset->begin();
        Pattern *top = *it_top;

        pruneset->erase(it_top);        //erase on iterator, because pruneset compares on usage
        ct->delete_pattern(top);

        cover = new Cover(par->seq, ct, false);

        double new_size = cover->get_sz_sequence_and_ct();
        if (new_size < current_size)            //Check improvement
        {
            //check if pruned pattern was subset of last accepted pattern
            if (par->prune_check) {
                if (!check_subset(top, accepted)) {
                    output_stream << "\nPruned a pattern that was not a subset of the accepted pattern.\n"
                                   << "\tAccepted: " << accepted->print(false) << "\tPruned: " << top->print(false)
                                   << endl;
                }
            }
            total_usg = cover->get_total_usage();
            current_size = new_size;
            //add more prune candidates
            for (auto p : *ct->get_ct()) {
                if (p->get_size() > 1 && p->get_usage_decreased())                //singletons can't be pruned
                    if (find_pattern_in_set(pruneset, p) != pruneset->end())    //if not already present
                        pruneset->insert(p);
            }
        } else {
            ct->rollback();
            ct->insert_pattern(top);        //put pattern back
        }
    }
    delete pruneset;
    return new usg_sz(total_usg, current_size);
}

//checks whether esa is a subset of esb
bool mdl_enter::check_subset(attribute_set *esa, attribute_set *esb) {
    auto endb = esb->end();
    for (auto it : *esa)
        if (esb->find(it) == endb)
            return false;
    return true;
}

//checks whether a is a subset of b
bool mdl_enter::check_subset(Pattern *a, Pattern *b) {
    int la = 0;
    for (int lb = 0; lb < b->get_length(); ++lb) {
        if (check_subset(a->get_symbols(la), b->get_symbols(lb)))
            la++;

        if (la == a->get_length())
            return true;
    }
    return false;
}


candpattern_set::iterator mdl_enter::find_pattern_in_set(pattern_set *pset, Pattern *p) {
    auto end = pset->end();
    for (auto it = pset->begin(); it != end; ++it)
        if (**it == *p)
            return it;
    return end;
}

candpattern_set::iterator mdl_enter::find_pattern_in_set(candpattern_set *pset, Pattern *p) {
    auto end = pset->end();
    for (auto it = pset->begin(); it != end; ++it)
        if (**it == *p)
            return it;
    return end;
}

prunepattern_set::iterator mdl_enter::find_pattern_in_set(prunepattern_set *pset, Pattern *p) {
    auto end = pset->end();
    for (auto it = pset->begin(); it != end; ++it)
        if (**it == *p)
            return it;
    return end;
}

mdl_enter::~mdl_enter() {
    delete par->seq;
    delete ct;
    if (cand != NULL) {
        delete cand;
    }
    delete ct_on_usg;
    delete cover;
    if (white_list)
        white_list->clear();
    delete white_list;
    delete black_list;
}
