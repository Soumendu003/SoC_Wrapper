#include"ut_Tokenizer.h"

ut_Tokenizer::ut_Tokenizer(const char* file, const char* delimeters, const char* special_char_set)
:file(file), delimeters(delimeters), special_char_set(special_char_set){
    this->ifs.open(file);
    if(!ifs.is_open()){
        throw "Unable to open file = "+string(file);
    }
    this->file_finished = false;
    this->tok_buff_full = false;
    this->buffer_finished = false;
    if(ifs){
        ifs.seekg(0, ifs.end);
        file_lengh = ifs.tellg();
        ifs.seekg(0, ifs.beg);
    }
    read_block();
}

ut_Tokenizer::~ut_Tokenizer(){
    if(ifs.is_open()){
        ifs.close();
    }
}

void ut_Tokenizer::read_block(){
    ifs.read(buffer, BUFFER_SIZE);
    if(file_finished){
        buffer_finished = true;
        buffer[0] = '\0';
        //cout<<"Reading finished"<<endl;
        return;
    }
    if(ifs.eof()){
        file_finished = true;
    }

    if(!file_finished){
        buffer[BUFFER_SIZE] = '\0';
        file_lengh -= BUFFER_SIZE;
    }
    else{
        buffer[file_lengh] = '\0';
        file_lengh = 0;
    }
    next_ptr = 0;
}

char* ut_Tokenizer::get_token(){
    tok_head = 0;
    if(tok_buff_full){
        tok_buff_full = false;
        return tok_buff;
    }
    if(buffer_finished){
        return 0;
    }
    while (strlen(buffer) > 0){
        uint16_t buf_s = strlen(buffer);
        while (tok_head < TOK_SIZE && next_ptr < buf_s){
            char ch = buffer[next_ptr];
            if(strchr(delimeters, ch) != 0){
                tok[tok_head] = '\0';
                if(tok[0] != '\0'){
                    next_ptr += 1;
                    return tok;
                }   
            }
            else if(ch == ' '){
                tok[tok_head] = '\0';
                if(tok[0] != '\0'){
                    next_ptr += 1;
                    return tok;
                }
            }
            else if(strchr(special_char_set, ch) != 0){
                tok[tok_head] = '\0';
                if(tok[0] != '\0'){
                    next_ptr += 1;
                    tok_buff[0] = ch;
                    tok_buff[1] = '\0';
                    tok_buff_full = true;
                    return tok;
                }
                else{
                    next_ptr += 1;
                    tok[0] = ch;
                    tok[1] = '\0';
                    return tok;
                } 
            }
            
            else{
                tok[tok_head] = ch;
                tok_head += 1;
            }
            next_ptr += 1;
        }
        if(next_ptr >= buf_s){
            read_block();
        }
        else if(tok_head >= TOK_SIZE){
            cout<<"Tok size greater than "<<to_string(TOK_SIZE)<<endl;
            exit(0);
        }
        else if(tok[0] != '\0'){
            return tok;
        }
        else{
            return get_token();
        }
    }
    return 0 ;
}
