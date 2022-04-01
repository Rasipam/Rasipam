#ifndef LSH_P_P_TABLE_H
#define LSH_P_P_TABLE_H

#include <map>
#include "Pattern.h"

class PatternTable {
    class MyHashMap {
        const int MOD = 1000007;
        int a[1000007]{};
    public:
        MyHashMap() {memset(a, 0, sizeof a);}
        int operator[](Pattern* x) const {
            return a[(unsigned long long)x % MOD];
        }
        int& operator[](Pattern* x) {
            return a[(unsigned long long)x % MOD];
        }
        void clear() {
            memset(a, 0, sizeof a);
        }
    }; // a hash map for mapping from Pattern* to pattern-id
public:
    static std::vector<std::vector<int>> *table; // A table point which points to the current using table.
                                                 // (*table)[i][j] presents the number of sequences that pattern i and pattern j both emerge
    static std::vector<std::vector<int>> pre_table; // used for rollback
    static std::vector<std::vector<int>> last_table; // used for rollback
    static std::vector<Pattern*> pattern_id_table; // mapping from pattern-id to Pattern*
    static void check_table(); // switch *table to pre_table if it points to last_table, and switch *table to last_table if it points to pre_table
    static void clear_table(int size); // clear *table to all zeros
    static void rollback_table(); // switch *table back
    static int table_size; // current size of pre_table and last_table (pre_table and last_table have same size)

    static MyHashMap pattern_id_map; // mapping from Pattern* to pattern-id
};


#endif //LSH_P_P_TABLE_H
