#include <iostream>
#include <stdio.h>

#include "le.hpp"

int main(int argc, char **argv){
    if(argc < 2){
        std::cout << "Give me a file yo" << std::endl;
        return 1;
    }
    FILE *fp = fopen(argv[1], "r");
    if(fp){
        std::string input;
        fseek(fp, 0, SEEK_END);
        int file_len = std::ftell(fp);
        fseek(fp, 0, SEEK_SET);
        input.resize(file_len);
        fread(&input[0], 1, file_len, fp);
        le *editor = new le(input);
        if(argc > 2){
            char mode;
            do{
                std::cin >> mode;
                if(mode == 'i'){
                    int index;
                    std::string text;
                    std::cin >> index;
                    std::cin >> text;
                    editor->insert(index, text);
                }
                if(mode == 'u'){
                    editor->undo();
                }
                std::cout << editor->get_buffer() << std::endl;
            } while(mode != 'q');

        } else {
            editor->insert(4, "<SOMETHING>");
            std::cout << editor->get_buffer() << std::endl;
            editor->insert(0, "<Here>");
            std::cout << editor->get_buffer() << std::endl;
            editor->insert(2, "<Hello>");
            std::cout << editor->get_buffer() << std::endl;
            editor->insert(4, "<Last one>");
            std::cout << editor->get_buffer() << std::endl;
            editor->insert(11, "<Kidding>");
            std::cout << editor->get_buffer() << std::endl;
            editor->undo();
            std::cout << editor->get_buffer() << std::endl;
            editor->undo();
            std::cout << editor->get_buffer() << std::endl;
            editor->undo();
            std::cout << editor->get_buffer() << std::endl;
        }
        delete editor;
    }
    fclose(fp);
}
