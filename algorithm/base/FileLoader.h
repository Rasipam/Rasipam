//
// Created by A on 2020/12/22.
//

#ifndef LSH_FILELOADER_HPP
#define LSH_FILELOADER_HPP

#include <list>
#include <fstream>
#include "BaseSeq.h"
#include "json.hpp"

enum FileType {
    TableTennis,
    Badminton,
    Tennis
};

class FileLoader {
public:
    static std::list<BaseSeq> loadFile(const std::string& fileName,
                                       FileType fileType,
                                       const std::string & target_player,
                                       const std::vector<std::string>& attribute_names,
                                       nlohmann::json &sequences);
};

#endif //LSH_FILELOADER_HPP
