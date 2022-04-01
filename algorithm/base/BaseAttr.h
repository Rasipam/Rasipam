//
// Created by A on 2021/1/23.
//

#ifndef LSH_BASEATTR_HPP
#define LSH_BASEATTR_HPP

#include <string>
#include <map>
#include <vector>

class BaseAttr {
public:
    static int from_key_value(const std::string &key, const std::string &value);
    static void print_values(const std::string &filename);
    static std::string get_key_value(const std::string &key, int attr);

    static std::string get_record();
    static const std::vector<std::string> & get_keys();
    static const std::vector<std::string> & get_attrs(const std::string &);

private:
    static std::vector<std::string> keys;
    static std::map<std::string, std::map<std::string, int>> mapping;
    static std::map<std::string, std::vector<std::string>> record;
};


#endif //LSH_BASEATTR_HPP
