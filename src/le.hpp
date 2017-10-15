#ifndef LE_HPP
#define LE_HPP
#include <string>
#include <vector>
class table;
class le{
    private:
        std::string buffers[3];
        table *changes;
    public:
        le(std::string file_content);
        ~le();
        std::string get_buffer();
        void insert(int index, std::string str);
        void remove(int index, int length);
        void undo();
        void redo();
};
#endif
