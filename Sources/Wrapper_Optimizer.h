#ifndef WRAPPER_OPTIMIZER_H
#define WRAPPER_OPTIMIZER_H

#include "SoC_Lexer.h"
#include<vector>    //vector
#include<stdlib.h>  //srand(), rand()

class Wrapper_Optimizer
{
    public:
        explicit Wrapper_Optimizer(vector<scanchain_t> *sc_array) :
         _sc_array(sc_array), _tsv_count(0), _test_time(0) , _curr_T(1) {}
        uint64_t Get_Count_TSV() ;
        uint64_t Get_Test_Time() ;
        void Two_Phase_Optimizer(uint8_t max_layer, uint8_t wrapper_chain_count) ;
        void Clear_Wrapper_Assignments() ;

    private:
        void Simulated_Annelation() ;
        void Initialize_WrapperChains() {}
        void Minimize_TSV_Phase() ;
        void Minimize_TT_Phase() {}
        void Count_TSV() {}
        void Count_TT() {}
        int Get_Delta_TSV(uint64_t sc_id, uint64_t wc_id) { return 0 ;}
        double Get_TT_Penalty(uint64_t sc_id, uint64_t wc_id) { return 0 ;}
        void Move_SC(uint64_t sc_id, uint64_t wc_id, uint64_t new_tsv_count) ;

    private:
        vector<scanchain_t> *_sc_array ;
        uint64_t _tsv_count ;
        uint64_t _test_time ;
        uint8_t _max_layer ;
        uint8_t _wc_count ;
        uint64_t _curr_T ;   // current Temperature
};

#endif