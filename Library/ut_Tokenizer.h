#ifndef UT_TOKENIZER_H
#define UT_TOKENIZER_H

#include<fstream>
#include<string>
#include <string.h>
#include<iostream>
#include<stdlib.h>

using namespace std;

#define BUFFER_SIZE  32*1024
#define TOK_SIZE 4*1024

class ut_Tokenizer{
    private:
        char buffer[BUFFER_SIZE + 1];
        char tok[TOK_SIZE + 1];
        char tok_buff[2];
        ifstream ifs;
        const char* file;
        uint16_t next_ptr;
        uint16_t tok_head;
        const char* delimeters;
        const char* special_char_set;
        bool file_finished;
        bool buffer_finished;
        bool tok_buff_full;
        uint32_t file_lengh = 0;
    public:
        explicit ut_Tokenizer() {};
        ut_Tokenizer(const char* file, const char* delimeters, const char* special_char_set);
        ~ut_Tokenizer();
        char* get_token();
    private:
        void read_block();
};


#endif