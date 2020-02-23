#ifndef TOKENIZER_H
#define TOKENIZER_H

#include<fstream>
#include<string>
#include <string.h>
#include<iostream>

using namespace std;

#define BUFFER_SIZE  32*1024
#define TOK_SIZE 4*1024+1

class Tokenizer{
    private:
        char buffer[BUFFER_SIZE];
        char tok[TOK_SIZE];
        char tok_buff[2];
        char end;
        ifstream ifs;
        const char* file;
        uint16_t next_ptr;
        uint16_t tok_head;
        const char* delimeters;
        bool file_finished;
        bool tok_buff_full;
    
    public:
        explicit Tokenizer() {};
        Tokenizer(const char* file, const char* delimeters, const char end_char);
        ~Tokenizer();
        char* get_token();
    
    private:
        void read_block();
        void end_block_handler();
};


#endif