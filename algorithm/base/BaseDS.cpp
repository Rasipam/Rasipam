//
// Created by A on 2021/1/23.
//

#include "BaseDS.h"
#include "BaseAttr.h"
#include <iostream>
#include <ctime>

using namespace std;
using json = nlohmann::json;

std::list<BaseSeq> BaseDS::sequenceList = {};

void BaseDS::load_file(const vector<string> &files,
                       FileType type,
                       const string &target_player,
                       json& processed_sequences,
                       const vector<string> &attribute_names,
                       const string &dir = "") {
    clock_t t1 = clock();
    json match_seq_count;
    json sequences;
    for (const auto &file: files) {
        std::list<BaseSeq> sequence_one_file = FileLoader::loadFile(dir + "/" + file,
                                                                    type, target_player, attribute_names,
                                                                    sequences);
        json match;
        match["match"] = file;
        match["sequence_number"] = sequence_one_file.size();
        sequenceList.splice(sequenceList.end(), sequence_one_file);
        match_seq_count.push_back(match);
    }

    processed_sequences["matches"] = match_seq_count;
    for (auto & sequence : sequences) {
        processed_sequences["sequences"].push_back(sequence);
    }

    cout << "Finish Loading! Used time: " << (clock() - t1) * 1.0 / CLOCKS_PER_SEC << "s" << endl << endl;
    cout << "Attribute mapping:" << endl << BaseAttr::get_record() << endl;
    cout << "Total processed_sequences: " << sequenceList.size() << endl;
}

void BaseDS::convert_data(const string &filename, json& processed_sequences) {
    ofstream f;
    f.open(filename, ios::out);
    auto attrs = BaseAttr::get_keys();
    int nrAttr = attrs.size();
    f << nrAttr;
    for (const auto &attr: attrs) {
        f << " " << BaseAttr::get_attrs(attr).size();
    }

    int seq_id = 0;
    for (auto sequence = sequenceList.begin(); sequence != sequenceList.end(); sequence++, seq_id++) {
        vector<vector<string> > events(sequence->size(), vector<string>(nrAttr));
        processed_sequences["sequences"][seq_id]["events"] = events;
    }

    for (int i = 0; i < nrAttr; i++) {
        f << endl;
        seq_id = 0;
        for (auto sequence = sequenceList.begin(); sequence != sequenceList.end(); sequence++, seq_id++) {
            if (sequence != sequenceList.begin()) {
                f << -1 << " ";
//                if (i == 0)
//                    winf << -1 << " ";
            }
            int event_id = 0;
            for (auto event_it = sequence->begin(); event_it != sequence->end(); ++event_it, ++event_id) {
                f << (*event_it)[i] << " ";
                processed_sequences["sequences"][seq_id]["events"][event_id][i] = BaseAttr::get_attrs(BaseAttr::get_keys()[i])[(*event_it)[i]];
            }
//            if (i == 0)
//                winf << sequence->winner << " ";
        }
        if (i != nrAttr - 1) {
            f << -2;
        }
    }
    f.close();
}
