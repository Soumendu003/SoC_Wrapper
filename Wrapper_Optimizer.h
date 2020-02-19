#ifndef WRAPPER_OPTIMIZER_H
#define WRAPPER_OPTIMIZER_H

#include "SoC_Lexer.h"
#include<vector>    //vector
#include<stdlib.h>  //srand(), rand()

class Wrapper_Optimizer
{
    public:
        explicit Wrapper_Optimizer(vector<scanchain_t> *sc_array) : _sc_array(sc_array) {}
        uint64_t Count_TSV() {return 0 ;}
        uint64_t Test_Time() {return 0 ;}
        void Simulated_Annelation() ;

    private:
        void Initialize_WrapperChains() {}
        void Minimize_TSV_Phase() ;
        void Minimize_TT_Phase() {}

    private:
        vector<scanchain_t> *_sc_array ;
};

#endif