#ifndef WRAPPER_OPTIMIZER_H
#define WRAPPER_OPTIMIZER_H

#include "SoC_Lexer.h"
#include "Wrapper_Chain.h"
#include "TT_Movement.h"
#include<vector>    //vector
#include<stdlib.h>  //srand(), rand()
#include<algorithm>
#include<assert.h>

typedef struct {
    uint64_t total_tsv ;
    uint64_t source_new_tsv ;
    uint64_t destination_new_tsv ;
    int del_tsv ;
} delta_tsv_t ;



class Wrapper_Optimizer
{
    public:
        explicit Wrapper_Optimizer(vector<scanchain_t> *sc_array) :
         _sc_array(sc_array), _wc_array(0), _tt_mover(0), _tsv_count(0), _test_time(0) , _curr_T(1) {}

        ~Wrapper_Optimizer()
        {
            delete _wc_array ;
            delete _tt_mover ;
        }
        
        uint64_t Get_Count_TSV() ;
        uint64_t Get_Test_Time() ;
        void Two_Phase_Optimizer(uint8_t max_layer, uint8_t wrapper_chain_count) ;
        void Clear_Wrapper_Assignments() ;

    private:
        void Simulated_Annelation() ;
        void Initialize_WrapperChains() ;
        void Initialize_TT_Mover() ;
        void Initialize_WrapperChain_TSV() ;
        void Minimize_TSV_Phase() ;
        void Minimize_TT_Phase() ;
        void Count_TSV() {}
        void Count_TT() {}
        const delta_tsv_t Get_Delta_TSV(uint64_t sc_id, uint64_t wc_id) ;
        double Get_TT_Penalty(uint64_t sc_id, uint64_t wc_id) { return 0 ;}
        double Get_Delta_TT_Entropy(uint64_t source_wc_id, uint64_t destination_wc_id, uint64_t sc_id) { return 0 ;}
        void Move_SC(uint64_t sc_id, uint64_t wc_id, const delta_tsv_t& delta_tsv) ;
        void WrapperChain_TT_Update(uint64_t wc_id) ;

    public :
        static bool WC_tt_min_compare(Wrapper_Chain* wc1, Wrapper_Chain* wc2) 
        { 
            return wc1->Get_tt() > wc2->Get_tt() ;
        }

        static bool SC_tt_compare(scanchain_t sc1, scanchain_t sc2)
        {
            return sc1.test_time > sc2.test_time ;
        }

    private:
        vector<scanchain_t> *_sc_array ;
        vector<Wrapper_Chain*> *_wc_array ;
        TT_Movement *_tt_mover ;
        uint64_t _tsv_count ;
        uint64_t _test_time ;
        uint8_t _max_layer ;
        uint8_t _wc_count ;
        uint64_t _curr_T ;   // current Temperature
};

#endif