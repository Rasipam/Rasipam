#include "Common.h"
#include <iterator>
#include "Sequence.h"

Sequence::Sequence(FILE *f, Parameters *par) : par(par) {
    error_flag = false;
    if (read(f)) {
        error_flag = true;
        return;
    }
    if (!par->dummy_file.empty()) {
        if (load_dummies()) {
            error_flag = true;
            return;
        }
    } else
        par->nr_of_patterns = 0;
    init();
}

int Sequence::load_dummies() {
    string line;
    ifstream myfile(par->dummy_file.c_str());
    if (myfile.is_open()) {
        int cnt = -1;
        while (getline(myfile, line)) {
            if (cnt == -1) {
                istringstream buf(line);
                istream_iterator<string> beg(buf), end;
                vector<string> tokens(beg, end);
                auto it_vec = tokens.begin(), end_vec = tokens.end();
                it_vec++;
                par->nr_of_patterns = atoi((*it_vec).c_str());
                par->dummies = new Dummy *[par->nr_of_patterns];
                it_vec++;
                par->nr_of_attributes = atoi((*it_vec).c_str());
                it_vec++;
                par->temp_alphabet_size = atoi((*it_vec).c_str());
                par->alphabet_sizes = new int[par->nr_of_attributes];
                for (int i = 0; i < par->nr_of_attributes; ++i)
                    par->alphabet_sizes[i] = par->temp_alphabet_size;
                par->alphabet_size = par->nr_of_attributes * par->temp_alphabet_size;

                cnt++;
                continue;
            }

            int size, length, support;
            float gap_chance;
            attr_sym_set **events;
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> tokens(beg, end);
            auto it_vec = tokens.begin(), end_vec = tokens.end();
            size = atoi((*it_vec).c_str());
            it_vec++;
            length = atoi((*it_vec).c_str());
            it_vec++;
            support = atoi((*it_vec).c_str());
            it_vec++;
            gap_chance = atof((*it_vec).c_str());
            it_vec++;
            events = new attr_sym_set *[length];
            for (int l = 0; l < length; ++l) {
                int height = atoi((*it_vec).c_str());
                it_vec++;
                events[l] = new attr_sym_set;
                for (int h = 0; h < height; ++h) {
                    int aid = atoi((*it_vec).c_str());
                    it_vec++;
                    int sym = atoi((*it_vec).c_str());
                    it_vec++;
                    events[l]->insert(new attr_sym(aid, sym));
                }
            }
            par->dummies[cnt] = new Dummy(size, length, events, support, gap_chance);
            cnt++;
            if (cnt == par->nr_of_patterns)
                break;
        }
        myfile.close();
    } else {
        cout << "ERROR opening pattern file: " << par->dummy_file << endl;
        par->dummy_file = "";
        return 1;
    }
    return 0;
}


void Sequence::pre_init() {
    //compute all tree-id's once and save them based on sym and attr
    tree_ids = new int *[par->nr_of_attributes];
    int tree_id = 1;
    for (int a = 0; a < par->nr_of_attributes; ++a) {
        tree_ids[a] = new int[par->alphabet_sizes[a]];
        for (int s = 0; s < par->alphabet_sizes[a]; ++s)
            tree_ids[a][s] = tree_id++;
    }
}

void Sequence::init() {
    //count the nr of events with each possible size
    int *cnt = new int[par->nr_of_attributes + 1];
    for (int sz = 0; sz <= par->nr_of_attributes; ++sz)
        cnt[sz] = 0;

    for (int mev = 0; mev < par->nr_events; ++mev)
        cnt[mev_time[mev]->get_size()]++;

    mev_term = new float[par->nr_events];
    for (int mev = 0; mev < par->nr_events; ++mev)
        mev_term[mev] = -lg2(cnt[mev_time[mev]->get_size()] / (float) par->nr_events);

    mu = new MathUtil(nr_events, par->alphabet_size);

    compute_st_codelengths();

    is_covered = new int[par->nr_events];
    for (int i = 0; i < par->nr_events; ++i) {
        is_covered[i] = 0;                                //for each Event we keep track whether it is covered
        mev_time[i]->finished();                        //initialize the is_covered variable
        if (i < (par->nr_events - 1))                    //set NEXT pointers
            mev_time[i]->set_next(mev_time[i + 1]);
    }
}

//Return 1 in case of error
int Sequence::read(FILE *f) {
    int ecnt = 0;        //event count
    int mcnt = 0;        //event count
    int scnt = 1;        //sequence count		NOTE: no -1 after last sequence
    int acnt = 1;        //attribute count		NOTE: no -2 after last attribute
    int a;                //symbol

    /*
    Every line contains one attribute and ends with -2, all sequences are separated by -1
    For each attribute i its values range from 0 to max_i
    */

    //read header info
    int cnt = -1;
    par->alphabet_size = 0;
    while (fscanf(f, "%d", &a) == 1) {
        if (cnt == -1) {
            par->nr_of_attributes = a;
            par->alphabet_sizes = new int[a];
            cnt++;
            continue;
        }
        if (cnt == par->nr_of_attributes)
            break;
        par->alphabet_sizes[cnt++] = a;
        par->alphabet_size += a;
    }

    //count sequences and events
    rewind(f);
    cnt = -1;
    while (fscanf(f, "%d", &a) == 1) {
        if (cnt++ < par->nr_of_attributes)    //skip header line
            continue;
        if (a == -2) acnt++;            //end of attribute
        if (acnt < 2) {
            //all attributes have same number of sequences and events, so we only count the events and sequences in the first attribute
            if (a >= 0) {
                mcnt++;    //new event
            }
            else {
                scnt++;       //new sequence
            }
        }
    }
    ecnt = mcnt * acnt;
    rewind(f);
    par->nr_events = mcnt;
    nr_sequences = scnt;
    nr_events = ecnt;

    if (acnt != par->nr_of_attributes) {
        cout
                << "WARNING: header line differs from data! Must contain number of attributes followed by alphabetsize per attribute.\n";
        return 0;    //Does not have to be an error
    }

    occ = new list<Event *> *[par->nr_of_attributes];                    //for each aid
    for (int aid = 0; aid < par->nr_of_attributes; ++aid) {
        occ[aid] = new list<Event *>[par->alphabet_sizes[aid]];        //for each symbol
        for (int s = 0; s < par->alphabet_sizes[aid]; ++s)
            occ[aid][s] = list<Event *>();                        //a list of Event* where it occurs

    }

    mev_time = new Event *[par->nr_events];                    //number of multi-events
    for (int i = 0; i < par->nr_events; ++i)
        mev_time[i] = nullptr;
    sequence_sizes = new int[nr_sequences];                            //number of sequences
    for (int i = 0; i < nr_sequences; ++i)
        sequence_sizes[i] = 0;

    pre_init();

    cnt = -1;
    int sym;
    int i = 0;                //event id
    int sid = 0;            //sequence id
    int aid = 0;            //attribute id
    int sindex = 0;         //index in sequence
    while (fscanf(f, "%d", &sym) == 1) {
        if (cnt++ < par->nr_of_attributes)    //skip header line
            continue;
        if (sym == -2) {
            aid++;
            i = 0;
            sid = 0;
            sindex = 0;
            continue;
        }
        if (sym == -1) {
            sid++;
            sindex = 0;
            continue;
        }
        if (aid == 0) {
            mev_time[i] = new Event(par->nr_of_attributes, i, sid, sindex);
            sequence_sizes[sid]++;
        }
        Event *me = mev_time[i];
        occ[aid][sym].push_back(me);
        me->add_event(new Attribute(sym, aid, me->get_size(), tree_ids[aid][sym]));
        i++;
        sindex++;
    }
    return 0;
}

void Sequence::compute_st_codelengths() {
    ST_codelengths = new double *[par->nr_of_attributes];
    for (int aid = 0; aid < par->nr_of_attributes; ++aid) {
        ST_codelengths[aid] = new double[par->alphabet_sizes[aid]];
        for (int s = 0; s < par->alphabet_sizes[aid]; ++s)
            ST_codelengths[aid][s] = -lg2(
                    (occ[aid][s].size() + laplace) / (nr_events + par->alphabet_size * laplace));
    }

}

//Find all events that contain this set of events
const list<Event *> *Sequence::find_occurrences(attribute_set *events) const {
    auto *occ = new list<Event *>();
    int nr_events = events->size();

    auto *occ_per_event = new list<Event *>::iterator[nr_events];        //for each event an iterator to the list of Event where it occurs
    auto *end_per_event = new list<Event *>::iterator[nr_events];        //for each event an iterator to the end of the list of Event where it occurs
    int i = 0;
    for (auto event : *events) {
        occ_per_event[i] = this->occ[event->attribute][event->symbol].begin();
        end_per_event[i] = this->occ[event->attribute][event->symbol].end();
        ++i;
    }

    int smallest_id, smallest;
    bool aligned;
    while (true) {
        if (occ_per_event[0] == end_per_event[0])
            break;

        smallest_id = (*occ_per_event[0])->id, smallest = 0;
        aligned = true;
        for (int j = 1; j < nr_events; ++j) {
            if (occ_per_event[j] == end_per_event[j])
                break;

            if ((*occ_per_event[j])->id <
                smallest_id)                    //keep track of the event with the earliest occurence
            {
                smallest_id = (*occ_per_event[j])->id;
                smallest = j;
            }

            if ((*occ_per_event[j])->id != (*occ_per_event[j - 1])->id)    //check if all point at same Event
                aligned = false;
        }
        if (aligned)                                                //add new occurence
            occ->push_back((*occ_per_event[0]));

        occ_per_event[smallest]++;                                    //go to next occurence for the event with the earliest occurence
        if (occ_per_event[smallest] == end_per_event[smallest])            //if there is no next occurence we stop
            break;
    }

    return occ;
}

//RETURN true when whole cover is complete
bool Sequence::cover(Pattern *p, Window *w) {
    bool result = false;

    attribute_set *events;
    for (int ts = 0; ts < p->get_length(); ++ts) {
        events = p->get_symbols(ts);
        for (auto event : *events) {
            if (result) {
                output_stream << "\n\n\n\n\n";
                output_stream
                        << "ERROR: we checked that pattern p could cover window w, but everything is covered before this method is finished.\n";
                output_stream << "The next event is: ";
                event->print();
                output_stream << "The pattern p: ";
                p->print();
                output_stream << "The window w: ";
                w->print();
                output_stream << "\n\n\n\n\n";
            }
            if (cover(event, w->get_mev_position(ts)->id, p))
                result = true;
        }
    }
    return result;
}

//RETURN true when whole cover is complete
bool Sequence::cover(Attribute *e, int pos, Pattern *p) {
    if (!is_covered[pos]) {
        if (mev_time[pos]->cover(e, p, p->get_length() > 1)) {
            is_covered[pos] = 1;
            mev_covered++;
        }
    }
    return (mev_covered == par->nr_events);
}

//RETURN true when cover is possible
bool Sequence::try_cover(attribute_set *events, int pos) {
    for (auto it: *events) {
        if (!try_cover(it, pos))
            return false;

        if (!try_overlap(pos))
            return false;
    }
    return true;
}

//RETURN true when cover is possible
bool Sequence::try_cover(Attribute *e, int pos) {
    if (is_covered[pos])
        return false;

    return mev_time[pos]->try_cover(e);
}

//RETURN true when cover is not overlap
bool Sequence::try_overlap(int pos) {
    if (is_covered[pos])
        return false;

    return mev_time[pos]->try_overlap();
}

//cover the rest of the data with singletons. Loop over all data
void Sequence::cover_singletons(Pattern ***singletons) {
    for (int i = 0; i < par->nr_events; ++i) {
        if (is_covered[i])    //this multi-event is already covered
            continue;
        for (auto it = mev_time[i]->get_events()->begin(); it != mev_time[i]->get_events()->end(); ++it) {
            if (!mev_time[i]->test_covered((*it)->id)) {   //if not yet covered
                //fill the is_covered array in Event
                mev_time[i]->cover((*it), singletons[(*it)->attribute][(*it)->symbol], false);
                singletons[(*it)->attribute][(*it)->symbol]->update_usages(0, par->attr_use[(*it)->attribute]);
            }
        }
    }
}

string Sequence::print_sequence(bool all_values) {
    stringstream g_output_stream;
    g_output_stream << "SEQUENCE. Alphabet size: " << par->alphabet_size << " #Attributes: " << par->nr_of_attributes
                   << " alphabet sizes: {";
    for (int i = 0; i < par->nr_of_attributes; ++i) {
        g_output_stream << par->alphabet_sizes[i];
        if (i + 1 < par->nr_of_attributes)
            g_output_stream << ", ";
    }
    g_output_stream << "}" << endl;
    g_output_stream << "#events: " << par->nr_events << " total #events: " << nr_events << endl;
    g_output_stream << "#Sequences: " << nr_sequences << endl;

    if (all_values) //print the entire sequence
    {
        for (int i = 0; i < par->nr_events; ++i) {
            for (auto it: *(mev_time[i]->get_events())) {
                g_output_stream << i << ":  ";
                it->print();
            }
            g_output_stream << endl;
        }
    }

    return g_output_stream.str();
}

Sequence::~Sequence() {
    for (int i = 0; i < par->nr_events; ++i)
        delete mev_time[i];
    delete[]mev_time;

    for (int aid = 0; aid < par->nr_of_attributes; ++aid) {
        delete[]ST_codelengths[aid];
        delete[]occ[aid];
    }
    delete[]ST_codelengths;
    delete[]occ;

    delete[]sequence_sizes;
    delete[]par->alphabet_sizes;

    delete[]is_covered;

}