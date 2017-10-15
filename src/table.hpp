#ifndef TABLE_HPP
#define TABLE_HPP
#include <string>
#include <vector>

#include "history.hpp"
struct piece;
class table: public history {
    piece *begin;
    piece *end;
    std::string original;
    std::string addition;
    std::string notes;
    public:
    table(std::string input);
    ~table();
    void insert(int index, int start, int length);
    void remove(int from, int to);
    std::vector<piece*> get_pieces();
};
#endif
