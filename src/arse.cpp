#include <iostream>
#include <stdio.h>

#include "le.hpp"

#include "testing.hpp"

void exec_command(le *editor, char mode);
int main(int argc, char **argv){
    if(argc < 2){
        le *editor = new le("Testing");
        std::cout << "Insertion test" << std::endl;
        editor->insert(4, "<SOMETHING>");
        assert(editor->get_buffer(), "Test<SOMETHING>ing");
        editor->insert(0, "<Here>");
        assert(editor->get_buffer(), "<Here>Test<SOMETHING>ing");
        editor->insert(2, "<Hello>");
        assert(editor->get_buffer(), "<H<Hello>ere>Test<SOMETHING>ing");
        editor->insert(4, "<Last one>");
        assert(editor->get_buffer(), "<H<H<Last one>ello>ere>Test<SOMETHING>ing");
        editor->insert(11, "<Kidding>");
        assert(editor->get_buffer(), "<H<H<Last o<Kidding>ne>ello>ere>Test<SOMETHING>ing");
        /* Undo/redo and removal causes memory leaks */
        std::cout << "Undo/redo test" << std::endl;
        editor->undo();
        assert(editor->get_buffer(), "<H<H<Last one>ello>ere>Test<SOMETHING>ing");
        editor->undo();
        assert(editor->get_buffer(), "<H<Hello>ere>Test<SOMETHING>ing");
        editor->redo();
        assert(editor->get_buffer(), "<H<H<Last one>ello>ere>Test<SOMETHING>ing");
        editor->undo();
        assert(editor->get_buffer(), "<H<Hello>ere>Test<SOMETHING>ing");
        std::cout << "Removal test" << std::endl;
        /* Delete across pieces */
        editor->remove(4, 10);
        assert(editor->get_buffer(), "<H<Hest<SOMETHING>ing");
        /* Delete whole piece */
        editor->remove(0, 2);
        assert(editor->get_buffer(), "<Hest<SOMETHING>ing");
        /* Delete in same piece */
        editor->remove(6, 0);
        assert(editor->get_buffer(), "<Hest<SOMETHING>ing");
        delete editor;
    } else {
        FILE *fp = fopen(argv[1], "r");
        if(fp){
            std::string input;
            fseek(fp, 0, SEEK_END);
            int file_len = std::ftell(fp);
            fseek(fp, 0, SEEK_SET);
            input.resize(file_len);
            fread(&input[0], 1, file_len, fp);
            le *editor = new le(input);
            char mode;
            do{
                std::cin >> mode;
                exec_command(editor, mode);
                std::cout << editor->get_buffer() << std::endl;
            } while(mode != 'q');

            delete editor;
    }
    fclose(fp);
    }
}
void exec_command(le *editor, char mode){
    int index;
    std::string text;
    switch(mode){
        case 'i':
            std::cin >> index;
            std::cin >> text;
            editor->insert(index, text);
            break;
        case 'u':
            editor->undo();
            break;
        default:
            std::cout << "Not a command" << std::endl;
    }
}
