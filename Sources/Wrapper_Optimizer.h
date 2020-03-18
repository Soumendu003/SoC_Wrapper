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

typedef struct {
    uint64_t init_max_tt ;
    uint64_t final_max_tt ;
    string initial_partition_time ;
    string simulated_annelation_time ;
    uint64_t init_tsv ;
    uint64_t final_tsv ;
} two_phase_result_t ;

using namespace std ;


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
        two_phase_result_t *Two_Phase_Optimizer(uint8_t max_layer, uint64_t wrapper_chain_count) ;

    private:
        void Simulated_Annelation(two_phase_result_t *ret_val) ;
        void Initialize_WrapperChains() ;
        void Initialize_TT_Mover() ;
        void Initialize_WrapperChain_TSV() ;
        void Minimize_TSV_Phase() ;
        void Minimize_TT_Phase() ;
        void Clear_Wrapper_Assignments() ;
        //void Count_TSV() {}
        //void Count_TT() {}
        const delta_tsv_t Get_Delta_TSV(uint64_t sc_id, uint64_t destination_wc) ;
        double Get_TT_Penalty(uint64_t sc_id, uint64_t destination_wc) ;
        double Get_TSV_Penalty(const delta_tsv_t& delta_tsv) ;
        double Get_Delta_TT_Entropy(uint64_t source_wc_id, uint64_t destination_wc_id, uint64_t sc_id) ;
        void Move_SC(uint64_t sc_id, uint64_t wc_id, const delta_tsv_t& delta_tsv) ;
        void WrapperChain_TT_Update(uint64_t wc_id) ;
        double Entropy(double val, double total_val) ;

    public :
        static bool WC_tt_min_compare(Wrapper_Chain* wc1, Wrapper_Chain* wc2) 
        { 
            return wc1->Get_tt() > wc2->Get_tt() ;
        }

        static bool SC_tt_compare(scanchain_t sc1, scanchain_t sc2)
        {
            return sc1.test_time > sc2.test_time ;
        }

        static bool WC_index_compare(Wrapper_Chain* wc1, Wrapper_Chain* wc2)
        {
            return wc1->Get_id() < wc2->Get_id() ;
        }

        static void copy_result(two_phase_result_t *to_var, two_phase_result_t *from_var) ;

        static bool greater_than_result(two_phase_result_t *var1, two_phase_result_t *var2) ;

    private:
        vector<scanchain_t> *_sc_array ;
        vector<Wrapper_Chain*> *_wc_array ;
        TT_Movement *_tt_mover ;
        uint64_t _tsv_count ;
        uint64_t _test_time ;
        uint8_t _max_layer ;
        uint8_t _wc_count ;
        double _curr_T ;   // current Temperature
        uint64_t _total_tt ;
        uint64_t _tsv_over_tt ;
        uint64_t _tt_over_tsv ;
};

#endif