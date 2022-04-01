#include "Common.h"
#include "MathUtil.h"

//const double logbase = log(2);

#define pw2(x) exp((x) * logbase)

double inline
plus(double a, double b) {
    double c = a > b ? a : b;
    return lg2(pw2(a - c) + pw2(b - c)) + c;
}

double inline
diff(double a, double b) {
    double c = a > b ? a : b;
    return lg2(pw2(a - c) - pw2(b - c)) + c;
}


MathUtil::MathUtil(int maxElements, int maxSymbol) : lg_fact(maxElements + 1), pow2(maxSymbol + 1) {
    for (int i = 2; i <= maxElements; i++)
        lg_fact[i] = lg_fact[i - 1] + lg2(i);

    pow2[0] = 1;
    pow2[1] = 2;
    for (int i = 2; i <= maxSymbol; i++)
        pow2[i] = pow2[i - 1] * 2;
}

double MathUtil::stirling(int n, int k) const {
    double s, ns;

    ns = s = std::numeric_limits<double>::min();

    for (int i = 1; i <= k; i++) {
        double term = lg_choose(k, k - i) + n * lg2(i);

        if ((k - i) % 2 == 1)
            ns = plus(ns, term);
        else
            s = plus(s, term);

        if (s > ns && ns >= 0) {
            s = diff(s, ns);
            ns = std::numeric_limits<double>::min();
        }
    }

    return s - lg_factorial(k);
}

double MathUtil::intcost(int u) const {
    double c = lg2(2.865064);
    double z = lg2(u);

    while (z > 0) {
        c += z;
        z = lg2(z);
    }

    return c;
}