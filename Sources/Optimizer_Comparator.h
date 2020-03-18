#ifndef OPTIMIZER_COMPARATOR_H
#define OPTIMIZER_COMPARATOR_H

#include "SoC_Lexer.h"
#include "Wrapper_Optimizer.h"
#include<iostream>

using namespace std;

class Optimizer_Comparator
{
    public:
        explicit Optimizer_Comparator(): _soc_lexer(0), _wrapper_optimizer(0), _curr_benchmark(0) {}

        ~Optimizer_Comparator()
        {
            delete _soc_lexer ;
            delete _wrapper_optimizer ;
        }

        void Compare(char* file) ;

    private:
        SoC_Lexer *_soc_lexer ;
        Wrapper_Optimizer *_wrapper_optimizer ;
        vector<scanchain_t> *_curr_scanchain ;
        char *_curr_benchmark ;

};


#endif