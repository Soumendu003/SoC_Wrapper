#ifndef WRAPPER_OPTIMIZER_H
#define WRAPPER_OPTIMIZER_H

#include "SoC_Lexer.h"
#include<vector>

class Wrapper_Optimizer
{
    public:
        explicit Wrapper_Optimizer(vector<scanchain_t> *sc_array) : _sc_array(sc_array) {}
        uint64_t Count_TSV() ;
        uint64_t Test_Time() ;
        void Simulated_Annelation() ;

    private:
        vector<scanchain_t> *_sc_array ;
};

#endif