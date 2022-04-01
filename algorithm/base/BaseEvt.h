//
// Created by A on 2021/1/23.
//

#ifndef LSH_BASEEVT_HPP
#define LSH_BASEEVT_HPP

#include <iostream>
#include <vector>
#include <list>

class BaseEvt {
public:
    explicit BaseEvt(std::vector<int>);
    friend std::ostream & operator <<(std::ostream &out, const BaseEvt &event);

    int& operator [](int i);
    int operator [](int i) const;
    [[nodiscard]] int size() const;

private:
    std::vector<int> attrs;
};


#endif //LSH_BASEEVT_HPP
