//
// Created by A on 2020/12/22.
//

#include <iostream>
#include "FileLoader.h"
#include "BaseAttr.h"
#include "json.hpp"
#include <exception>
#define FLITER_SERVER
using namespace std;
using json = nlohmann::json;

list<BaseSeq> extract_tt_sequences(json j, const string & target_player, const vector<string> &attribute_names, json &sequences) {
    string our_player;
    int t_num = 0, p_num = 0;
    for (auto t : j["player"]) {
        for (auto p : t) {
            if (p["name"] == target_player) {
                our_player += (char)(t_num + '0');
                our_player += (char)(p_num + '0');
                break;
            }
            p_num++;
        }
        if (!our_player.empty()) break;
        t_num++;
    }
    try {
        list<BaseSeq> sequenceList = list<BaseSeq>();

        for (auto game: j["record"]["list"]) {
            for (auto rally: game["list"]) {
                auto events = vector<BaseEvt>();
#ifdef FLITER_SERVER
                if (rally["list"].size() < 3) {
                    continue;
                }
                if (rally["list"][0]["HitPlayer"] != our_player) {
                    continue;
                }
#endif
                json sequence;
                sequence["server"] = (int)(rally["list"][0]["HitPlayer"] != our_player);
                sequence["winner"] = (int)(rally["winSide"] != t_num);
                sequences.push_back(sequence);
                for (auto strike: rally["list"]) {
                    auto attrs = vector<int>();

                    if (strike["HitPlayer"] == our_player) {
                        for (const auto & attribute_name : attribute_names) {
                            attrs.push_back(BaseAttr::from_key_value(attribute_name, strike[attribute_name]));
                        }
                    } else {
                        auto remove_quote = [](string s) {
                            s.erase(remove_if(s.begin(), s.end(), [](unsigned char x) {return x == '"';}), s.end());
                            return s;
                        };
                        for (const auto & attribute_name : attribute_names) {
                            string attribute_name_op = remove_quote(to_string(strike[attribute_name])) + "Op";
                            attrs.push_back(BaseAttr::from_key_value(attribute_name, attribute_name_op));
                        }
                    }

                    events.emplace_back(attrs);
                }
                sequenceList.emplace_back(events, rally["winSide"]);
            }
        }

        return sequenceList;
    } catch (exception &msg) {
        throw invalid_argument("Unrecognizable file format!");
    }
}

string ball_position_int(double x) {
    if (x < 0.55) {
        return "Very Low";
    } else if (x < 1.25) {
        return "Low";
    } else if (x < 2) {
        return "Medium";
    }
//    else if (x < 2.9) {
//        return "High";
//    }
    else {
        return "High";
    }
}

list<BaseSeq> extract_badminton_sequences(json j, const string & target_player, const vector<string> &attribute_names, json &sequences) {
    string our_player = target_player;
    try {
        list<BaseSeq> sequenceList = list<BaseSeq>();
        for (auto rally: j["dataset"]) {
            auto events = vector<BaseEvt>();
#ifdef FLITER_SERVER
            if (rally["patterns"].size() < 7) {
                continue;
            }
            if (rally["server"] != our_player) {
                continue;
            }
#endif
            json sequence;
            sequence["server"] = (int)(rally["server"] != our_player);
            sequence["winner"] = (int)(rally["winner"] != our_player);
            sequences.push_back(sequence);
            for (auto strike: rally["patterns"]) {
                auto attrs = vector<int>();
                if (strike["HitPlayer"] == our_player) {
                    attrs.push_back(BaseAttr::from_key_value("BallPosition", ball_position_int(strike["BallPosition"])));
                    attrs.push_back(BaseAttr::from_key_value("StrikePosition", strike["StrikePosition"]));
                    attrs.push_back(BaseAttr::from_key_value("StrikeTech", strike["StrikeTech"]));
                } else {
                    auto remove_quote = [](string s) {
                        s.erase(remove_if(s.begin(), s.end(), [](unsigned char x) {return x == '"';}), s.end());
                        return s;
                    };
                    string BallPosition = remove_quote(to_string(strike["BallPosition"]));
                    string StrikePosition = remove_quote(to_string(strike["StrikePosition"]));
                    string StrikeTech = remove_quote(to_string(strike["StrikeTech"]));
                    attrs.push_back(BaseAttr::from_key_value("BallPosition", ball_position_int(strike["BallPosition"]) + "Op"));
                    attrs.push_back(BaseAttr::from_key_value("StrikePosition", StrikePosition + "Op"));
                    attrs.push_back(BaseAttr::from_key_value("StrikeTech", StrikeTech + "Op"));
                }

                events.emplace_back(attrs);
            }
            sequenceList.emplace_back(events, int(rally["winner"] == "Momota Kento"));
        }

        return sequenceList;
    } catch (exception &msg) {
        throw invalid_argument("Unrecognizable file format!");
    }
}

list<BaseSeq> extract_tennis_sequences(json j, const string & target_player, const vector<string> &attribute_names, json &sequences) {
    string our_player = target_player;
    try {
        list<BaseSeq> sequenceList = list<BaseSeq>();

        for (auto rally: j["dataset"]) {
            auto events = vector<BaseEvt>();
#ifdef FLITER_SERVER
//            if (rally["patterns"].size() < 7) {
//                continue;
//            }
            if (rally["server"] != our_player) {
                continue;
            }
#endif
            json sequence;
            sequence["server"] = (int)(rally["server"] != our_player);
            sequence["winner"] = (int)(rally["winner"] != our_player);
            sequences.push_back(sequence);
            for (auto strike: rally["patterns"]) {
                auto attrs = vector<int>();
                if (strike["HitPlayer"] == our_player) {
                    for (const auto & attribute_name : attribute_names) {
                        attrs.push_back(BaseAttr::from_key_value(attribute_name, strike[attribute_name]));
                    }
                } else {
                    auto remove_quote = [](string s) {
                        s.erase(remove_if(s.begin(), s.end(), [](unsigned char x) {return x == '"';}), s.end());
                        return s;
                    };
                    for (const auto & attribute_name : attribute_names) {
                        string attribute_name_op = remove_quote(to_string(strike[attribute_name])) + "Op";
                        attrs.push_back(BaseAttr::from_key_value(attribute_name, attribute_name_op));
                    }
                }

                events.emplace_back(attrs);
            }
            sequenceList.emplace_back(events, int(rally["winner"] == our_player));
        }

        return sequenceList;
    } catch (exception &msg) {
        throw invalid_argument("Unrecognizable file format!");
    }
}

list<BaseSeq> FileLoader::loadFile(const string& fileName,
                                   FileType fileType,
                                   const string& target_player,
                                   const vector<string> &attribute_names,
                                   json &sequences) {
    try {
        ifstream file(fileName, ios::in);
        if (!file.is_open())
            throw invalid_argument("Cannot open this file! File: " + fileName);


        json j;
        file >> j;

        switch (fileType) {
            case FileType::TableTennis:
                return extract_tt_sequences(j, target_player, attribute_names, sequences);
            case FileType::Badminton:
                return extract_tt_sequences(j, target_player, attribute_names, sequences);
            case FileType::Tennis:
                return extract_tt_sequences(j, target_player, attribute_names, sequences);
            default:
                throw invalid_argument("The file type is not supported!");
        }
    } catch (exception &msg) {
        cout << "Error: " << msg.what() << endl;
        return list<BaseSeq>();
    }
}
