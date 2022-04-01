#include <iostream>
#include <thread>
#include <atomic>
#include "mdl_module/mdl_enter.h"

using namespace std;

int main(int argc, char** argv) {
    char *input_dir = "../data/tennis",
            *output_filename = "../iii.dat",
            *processed_sequences_filename = "../out_sequences.json",
            *target_player = "A";
    vector<string> files = {"4.json",
                            "7.json",
                            "9.json"};
    vector<string> attribute_names = {"BallPosition",
                                      "HittingPose",
                                      "HitTechnique"};
    auto file_type = FileType::TableTennis;
    nlohmann::json sequences;
    BaseDS::sequenceList = {};
    BaseDS::load_file(files, file_type, target_player, sequences, attribute_names, input_dir);
    BaseDS::convert_data(output_filename, sequences);

    ofstream processed_sequences_stream;
    processed_sequences_stream.open(processed_sequences_filename);
    processed_sequences_stream << sequences.dump();
    processed_sequences_stream.close();




    MDLParameters arg;
    arg.FP_windows = false;
    arg.fill_patterns = false;
    arg.input_filename = output_filename;
    arg.dummy_filename = "";
    arg.output_dir = "..";
    arg.minsup = 10;
    arg.pattern_window_index_min = 0;
    arg.pattern_window_index_max = -1;
    arg.pattern_length_min = 0;
    arg.pattern_length_max = 1000;
    arg.pattern_file = "out_patterns.json";
    string old_pattern_filename = "../pattern.json";
    string insert_patterns_filename = "../insert_tactics.json";
    string attr_use_str = "1 1 1";
    string delete_patterns_id_str = "";

    //region quantitative test
//    arg.input_filename = argv[1];
//    attr_use_str = argv[2];
//    auto replace = [](std::string& str, const std::string& from, const std::string& to) {
//        size_t start_pos = str.find(from);
//        if(start_pos == std::string::npos)
//            return false;
//        str.replace(start_pos, from.length(), to);
//        return true;
//    };
//    replace(attr_use_str, ",", " ");
//
//    arg.pattern_window_index_min = stoi(argv[3]);
//    arg.pattern_window_index_max = stoi(argv[4]);
//    arg.pattern_length_min = stoi(argv[5]);
//    arg.pattern_length_max = stoi(argv[6]);
    //endregion
    vector<int> attr_use;
    stringstream ss(attr_use_str);
    string use;
    while (ss >> use) {
        attr_use.push_back(stoi(use));
    }
    arg.attr_use = attr_use;

    vector<int> delete_patterns_id;
    stringstream ss_p(delete_patterns_id_str);
    string pattern_id;
    while (ss_p >> pattern_id) {
        delete_patterns_id.push_back(stoi(pattern_id));
    }
    string old_pattern_filename_str = old_pattern_filename;
    mdl_modify_and_run(old_pattern_filename_str, delete_patterns_id, insert_patterns_filename, arg);
//    mdl_run(arg, insert_patterns_filename, true);
    return 0;
}
