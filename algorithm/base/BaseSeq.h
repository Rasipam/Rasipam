//
// Created by A on 2021/1/23.
//

#ifndef LSH_BASESEQ_HPP
#define LSH_BASESEQ_HPP


#include "BaseEvt.h"

class BaseSeq {
public:
    explicit BaseSeq(std::vector<BaseEvt> );
    explicit BaseSeq(std::vector<BaseEvt>, int);
    std::vector<BaseEvt>::iterator begin();
    std::vector<BaseEvt>::iterator end();
    friend std::ostream & operator <<(std::ostream &out, const BaseSeq &sequence);
    [[nodiscard]] int size() const;
    int winner;
private:
    std::vector<BaseEvt> events;
};


#endif //LSH_BASESEQ_HPP
