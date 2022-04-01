#ifndef PARAMS_H
#define PARAMS_H

#include <limits>
#include <cstdint>

#include "Defines.h"


struct Params {
    Params() : sparse(false), thresh(1) {};

    bool sparse;
    int thresh;
};

#endif
