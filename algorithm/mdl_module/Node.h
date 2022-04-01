#ifndef NODE_H
#define NODE_H

#include "Common.h"
#include "Attribute.h"
#include "Pattern.h"

class Node {
public:
    Node(int alphabet_size, int *alphabet_sizes, int timestep) : alphabet_size(alphabet_size),
                                                                 alphabet_sizes(alphabet_sizes), timestep(timestep) {
        tree_id = 0;    //ROOT
        init(-1);
    }

    Node(int alphabet_size, int *alphabet_sizes, int timestep, int id, int aid) : alphabet_size(alphabet_size),
                                                                                  alphabet_sizes(alphabet_sizes),
                                                                                  timestep(timestep), tree_id(id) {
        init(aid);
    }

    ~Node();

    void init(int aid) {
        next_timestep = nullptr;
        infrequent = false;
        pos_correction = 0;
        for (int i = 0; i <= aid; ++i)
            pos_correction += alphabet_sizes[i];                //symbols of lower or similar attributes can't become children
        nr_children = alphabet_size -
                      pos_correction;        //nr_children = alphabet_size - tree_id;
        children = new Node *[nr_children];
        for (int i = 0; i < nr_children; ++i)
            children[i] = nullptr;
    }


    bool find_pattern(Pattern *p, int start_pos, attribute_set::iterator it,
                      attribute_set::iterator end)        //True when attribute_set found and thus not frequent, False otherwise
    {
        if (infrequent)
            return true;

        if (it == end)        //check if there is a next node to go to
        {
            if (timestep == p->get_length() - 1 - start_pos)
                return false;
            else {
                if (next_timestep == nullptr)
                    return false;
                else
                    return next_timestep->find_pattern(p, start_pos,
                                                       p->get_symbols(start_pos + timestep + 1)->begin(),
                                                       p->get_symbols(start_pos + timestep + 1)->end());
            }
        }
        //evs is SORTED on tree_id, i.e. first ascending on attribute-level and then ascending alphabetically
        int pos = (*it)->tree_id - pos_correction - 1;
        if (children[pos] == nullptr)
            return false;
        else
            return children[pos]->find_pattern(p, start_pos, ++it, end);
    }

    void add_infrequent_pattern(Pattern *p, attribute_set::iterator it,
                                attribute_set::iterator end)    //evs is SORTED: first ascending on attribute-level and then ascending alphabetically
    {
        if (it == end) {
            if (timestep == p->get_length() - 1)
                infrequent = true;
            else {
                if (next_timestep == nullptr)
                    next_timestep = new Node(alphabet_size, alphabet_sizes, timestep + 1);//Root
                next_timestep->add_infrequent_pattern(p, p->get_symbols(timestep + 1)->begin(),
                                                      p->get_symbols(timestep + 1)->end());
            }
        } else {
            int tree_id = (*it)->tree_id;
            int pos = tree_id - pos_correction - 1;
            if (children[pos] == nullptr)
                children[pos] = new Node(alphabet_size, alphabet_sizes, timestep, tree_id, (*it)->attribute);
            children[pos]->add_infrequent_pattern(p, ++it, end);
        }
    }

    void print(const string& tab) {
        cout << tab << "NODE id: " << tree_id << "  infreq=" << infrequent << "  #Children: " << nr_children
             << "  nextTimestep: " << next_timestep << endl;
        for (int i = 0; i < nr_children; ++i)
            if (children[i] != nullptr)
                children[i]->print(tab + "\t");
    }

private:
    bool infrequent;    //whether the pattern represented by this node is known to be infrequent
    int alphabet_size;
    int *alphabet_sizes;
    int tree_id;        //ROOT = 0, rest ranges from 1 to alphabet_size
    int nr_children;    //alphabet_size - tree_id
    int pos_correction;

    Node **children;

    Node *next_timestep;
    int timestep;        //range 0 to ?

};

#endif