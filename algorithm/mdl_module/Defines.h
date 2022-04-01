#ifndef DEFINES_H
#define DEFINES_H

#include <list>
#include <vector>
#include <set>
#include <limits>
#include <cstdint>

typedef int32_t index_t;
const index_t index_minimal = std::numeric_limits<index_t>::min();	//minimal value for int32_t


typedef std::vector<uint32_t> uintvector;
typedef std::vector<int32_t> intvector;
typedef std::list<uint32_t> uintlist;
typedef std::vector<bool> boolvector;
typedef std::vector<double> doublevector;



#endif
