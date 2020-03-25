#ifndef SOC_LEXER_H
#define SOC_LEXER_H

#include<iostream>
#include<string>
#include<assert.h>
#include"Header.h"
#include"ut_Tokenizer.h"
#include<vector>
#include<stdlib.h>


using namespace std ;

typedef struct {
    uint64_t sc_id ;
    uint8_t in_layer ;
    uint8_t out_layer ;
    uint64_t test_time ;
    uint64_t wrapper_chain ;        // denotes the the wrapper chain that includes it
} scanchain_t ;

typedef enum {
    IN_CELL ,
    OUT_CELL ,
    SCAN_CHAIN 
} wrapper_element_type ;

typedef struct {
    uint64_t io_id ;
    wrapper_element_type type ;
    uint8_t layer ;
    uint64_t wrapper_chain ;
} io_cell_t ;

class SoC_Lexer 
{
    
    public:
        SoC_Lexer() {} ;

        explicit SoC_Lexer(const char *file)
        {
            _tok = new ut_Tokenizer(file, _delimeters.c_str(), _special_Char_Set.c_str()) ;
            _scanchains = 0 ;
            _sc_tt = (uint64_t)0 ;
            _avail_sc = (uint64_t)0 ;
        }

        ~SoC_Lexer()
        {
            delete _scanchains ;
            delete _io_cells ;
            delete _tok ;
            cout <<"Tokenizer Killed" ;
        }

        void Init_SoC_Lexer() ;
        vector<scanchain_t>* Get_RandLayer_scanchains(uint8_t max_layer = 2) ;
        vector<io_cell_t>* Get_RandLayer_iocells(uint8_t max_layer = 2) ;

    private:
        void Get_SoC_Wrapper_Elements() ;
        void Get_scanchains() ;
        void Get_input_cell() ;
        void Get_output_cell() ;

        
    private:
        ut_Tokenizer* _tok ;
        vector<scanchain_t>* _scanchains ;
        vector<io_cell_t>* _io_cells ;
        string  _delimeters = "\n\t\r";
        string _special_Char_Set = ":";
        uint64_t _sc_tt ;        // holds the tets time of the parsed scanchain
        uint64_t _avail_sc ;     // no of available scan chains in the buffer

};



#endif