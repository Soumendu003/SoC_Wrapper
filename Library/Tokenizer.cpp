#include "Tokenizer.h"

Tokenizer::Tokenizer (const char* file, const char* delimeters, const char end_char) : file(file), delimeters(delimeters) {
    this->ifs.open(file);
    if(!ifs.is_open()){
        cout<<"Unable to open file = "<<string(file)<<endl;
        throw "Unable to open file = "+string(file);
    }
    this->file_finished = false;
    this->tok_buff_full = false;
    strncpy(tok_buff, ";", 2);
    if(end_char == ';'){
        this->end = 0;
    }
    else if(end_char == '\n'){
        this->end = 1;
    }
    read_block();
}

Tokenizer::~Tokenizer(){
    if(ifs.is_open()){
        ifs.close();
    }
}

void Tokenizer::read_block(){
    if(end == 0){
        if(ifs.getline(buffer, BUFFER_SIZE, ';')){
            next_ptr = 0;
        }
        else{
            file_finished = true;
        }
        return;
    }
    else if(end == 1){
        if(ifs.getline(buffer, BUFFER_SIZE, '\n')){
            next_ptr = 0;
        }
        else{
            file_finished = true;
        }
        return;
    }
}

void Tokenizer::end_block_handler(){
    tok_buff_full = true;
}

char* Tokenizer::get_token(){
    tok_head = 0;
    if(tok_buff_full){
        tok_buff_full = false;
        return tok_buff;
    }
    if(file_finished){
        return 0;
    }
    uint16_t buf_s = strlen(buffer);
    while (tok_head < TOK_SIZE && next_ptr < buf_s)
    {
        char ch = buffer[next_ptr];
        if(strchr(delimeters, ch) != 0){
            tok[tok_head] = '\0';
            if(tok[0] != '\0')
            {
                next_ptr += 1;
                return tok;
            }
            
        }
        else if(ch == ' '){
            tok[tok_head] = '\0';
            if(tok[0] != '\0')
            {
                next_ptr += 1;
                return tok;
            }
        }
        else{
            tok[tok_head] = ch;
            tok_head += 1;
        }
        next_ptr += 1;
    }
    tok[tok_head] = '\0';
    if(next_ptr >= buf_s){
        end_block_handler();
        read_block();
    }
    if(tok[0] != '\0'){
        return tok;
    }
    else{
        return get_token();
    }
    
}
