//
// Created by A on 2020/12/22.
//

#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include "BaseAttr.h"

using namespace std;


map<string, vector<string>> BaseAttr::record = map<string, vector<string>>();
map<string, map<string, int>> BaseAttr::mapping = map<string, map<string, int>>();
vector<string> BaseAttr::keys = vector<string>();

int BaseAttr::from_key_value(const string &key, const string &value) {
    if (record.count(key) == 0) {
        keys.push_back(key);
        record.insert(pair<string, vector<string>>(key, vector<string>()));
        mapping.insert(pair<string, map<string, int>>(key, map<string, int>()));
    }

    string new_value = value;
//    new_value.erase(remove_if(new_value.begin(), new_value.end(), [](unsigned char x) {return isspace(x);}), new_value.end());

    auto &v = record[key];
    auto &m = mapping[key];
    if (m.count(new_value) == 0) {
        int pos = v.size();
        v.push_back(new_value);
        m.insert(pair<string, int>(new_value, pos));
        return pos;
    } else {
        return m[new_value];
    }
}

void BaseAttr::print_values(const std::string &filename) {
    ofstream f;
    f.open(filename, ios::out);
    for (auto m : mapping) {
        f << m.first << endl;
        vector<string> keys(record[m.first].size());
        for (auto vp : m.second) {
            keys[vp.second] = vp.first;
        }
        for (auto k : keys) {
            f << k << " ";
        }
        f << endl;
    }
}

const vector<string> & BaseAttr::get_keys() {
    return keys;
}

const vector<string> & BaseAttr::get_attrs(const string & key) {
    return record[key];
}


string BaseAttr::get_key_value(const string &key, const int attr) {
    if (record.count(key) == 0)
        return to_string(attr);

    auto v = record[key];
    if (attr >= 0 && attr < v.size()) return v[attr];
    else return to_string(attr);
}

string BaseAttr::get_record() {
    stringstream ss;
    for (const auto &rec: record) {
        ss << rec.first << "\t\t: ";
        for (int i = 0; i < rec.second.size(); i++) {
            if (i != 0) ss << ", ";
            ss << "(" << i << "," << rec.second.at(i) << ")";
        }
        ss << endl;
    }
    return ss.str();
}
