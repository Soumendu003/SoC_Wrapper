#ifndef SOC_LEXER_H
#define SOC_LEXER_H

#include<iostream>
#include<string>
#include"Header.h"
#include"ut_Tokenizer.h"
#include<vector>
#include<stdlib.h>
#include<time.h>

using namespace std ;


typedef struct {
    uint8_t in_layer ;
    uint8_t out_layer ;
    uint64_t test_time ;
    uint16_t wrapper_chain ;        // denotes the the wrapper chain that includes it
} scanchain_t ;

class SoC_Lexer {
    private:
        ut_Tokenizer* _tok = 0;
        string  _delimeters = "\n\t\r";
        string _special_Char_Set = ":";
        uint64_t _sc_tt = (uint64_t)0 ;        // holds the tets time of the parsed scanchain
        uint64_t _avail_sc = (uint64_t)0 ;     // no of available scan chains in the buffer
    public:
        SoC_Lexer() {} ;

        explicit SoC_Lexer(const char *file)
        {
            _tok = new ut_Tokenizer(file, _delimeters.c_str(), _special_Char_Set.c_str()) ;
        }

        ~SoC_Lexer()
        {
            delete _tok ;
            cout <<"Tokenizer Killed" ;
        }

        vector<scanchain_t>* Get_SoC_scanchains() ;
        vector<scanchain_t>* Get_RandLayer_scanchains(uint8_t max_layer = 2) ;

    private:
        uint8_t Get_scanchain() ;

        


};



#endif