#include <iostream>

#include "le.hpp"
#include "table.hpp"
#include "piece.hpp"

le::le(std::string file_content){
    buffers[SOURCE] = file_content;
    changes = new table(file_content);
}
le::~le(){
    delete changes;
}
std::string le::get_buffer(){
    std::string buffer;
    for(piece *p : changes->get_pieces()){
        std::cout << "[" << buffers[p->buffer].substr(p->start, p->length) << "] -- " << p->start << " -- " << p->length << std::endl;

        buffer.append(buffers[p->buffer].substr(p->start, p->length));
    }
    std::cout << std::endl;
    return buffer;
}
void le::insert(int index, std::string str){
    int start = buffers[CHANGE].length();
    int length = str.length();
    buffers[CHANGE].append(str);
    changes->insert(index, start, length);
}
void le::remove(int index, int length){
    changes->remove(index, length);
}
void le::undo(){
    changes->undo();
}
void le::redo(){
    changes->redo();
}
