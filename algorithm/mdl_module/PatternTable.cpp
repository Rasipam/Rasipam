#include "PatternTable.h"

std::vector<std::vector<int>> PatternTable::pre_table;
std::vector<std::vector<int>> PatternTable::last_table;
std::vector<std::vector<int>> *PatternTable::table = &pre_table;
std::vector<Pattern*> PatternTable::pattern_id_table;
int PatternTable::table_size = 1000;
PatternTable::MyHashMap PatternTable::pattern_id_map;

void PatternTable::check_table() {
    table = table == &pre_table ? &last_table : &pre_table;
}

void PatternTable::clear_table(int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            (*table)[i][j] = 0;
        }
    }
}

void PatternTable::rollback_table() {
    table = table == &pre_table ? &last_table : &pre_table;
}
