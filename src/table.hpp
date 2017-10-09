#ifndef TABLE_HPP
#define TABLE_HPP
#include <string>
#include <vector>

#include "undo.hpp"
struct piece;
class table: public undoable {
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
    void redo();
    std::vector<piece*> get_pieces();
};
#endif
