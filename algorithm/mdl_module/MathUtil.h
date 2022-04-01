#ifndef MATHUTIL_H
#define MATHUTIL_H

#include "Common.h"

typedef std::vector<double> doublevector;
typedef std::vector<int> intvector;

class MathUtil {
public:
    MathUtil(int maxElements, int maxSymbol);

    double lg_choose(int n, int k) const {
        return lg_fact[n] - lg_fact[k] - lg_fact[n - k];
    } // choose(n,k) = n! / ( k! * (n-k)! )
    double lg_factorial(int n) const { return lg_fact[n]; }

    int power2(int i) const { return pow2[i]; }

    double stirling(int n, int k) const;

    double intcost(int u) const;

protected:
    doublevector lg_fact;        //precomputed factorial of which we take the lg
    intvector pow2;                //precomputed exponential with exponent = 2
};

#endif
