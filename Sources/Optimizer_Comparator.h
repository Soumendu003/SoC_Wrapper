#ifndef OPTIMIZER_COMPARATOR_H
#define OPTIMIZER_COMPARATOR_H

#include "SoC_Lexer.h"
#include "Wrapper_Optimizer.h"
#include "WU_Optimizer.h"
#include "csv.h"
#include<iostream>

using namespace std;

class Optimizer_Comparator
{
    public:
        explicit Optimizer_Comparator(): _soc_lexer(0), _wrapper_optimizer(0), _csv(0), _curr_benchmark(0){}

        ~Optimizer_Comparator()
        {
            delete _soc_lexer ;
            delete _wrapper_optimizer ;
            delete _csv ;
        }

        void Init_Comparator(const char* output_file) ;
        void Compare(char* file) ;
    
    private:
        void Write_CSV_result(two_phase_result_t * two_phase_result, solution_t *wu_sol, string wu_time, string bench_file, uint64_t wc_count) ;

    private:
        SoC_Lexer *_soc_lexer ;
        Wrapper_Optimizer *_wrapper_optimizer ;
        vector<scanchain_t> *_curr_scanchain ;
        csvfile *_csv ;
        char *_curr_benchmark ;

};


#endif