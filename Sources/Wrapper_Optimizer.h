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
    string main_time ;
    uint64_t init_tsv ;
    uint64_t final_tsv ;
} two_phase_result_t ;

typedef struct io_container {
    uint64_t in_io_id ;
    uint64_t out_io_id ;
    uint8_t wc_scan_in ;
    uint8_t wc_scan_out ;
    uint8_t in_set_flag ;
    uint8_t out_set_flag ;
    uint8_t in_key_val ;
    uint8_t out_key_val ;

    io_container()
    {
        in_set_flag = 0 ;
        out_set_flag = 0 ;
    }

    void Insert_in(uint8_t key, uint8_t scan_in, uint64_t id) {
        if (in_set_flag) {
            if (key < in_key_val) {
                in_io_id = id ;
                in_key_val = key ;
                wc_scan_in = scan_in ;
            }
        } else{
            in_set_flag = 1 ;
            in_io_id = id ;
            in_key_val = key ;
            wc_scan_in = scan_in ;
        }
    }

    void Insert_out(uint8_t key, uint8_t scan_out, uint64_t id) {
        if (out_set_flag) {
            if (key < out_key_val) {
                out_io_id = id ;
                out_key_val = key ;
                wc_scan_out = scan_out ;
            }
        } else{
            out_set_flag = 1 ;
            out_io_id = id ;
            out_key_val = key ;
            wc_scan_out = scan_out ;
        }
    }

    void Clear() {  in_set_flag = 0 ; out_set_flag = 0 ; }
    bool Is_in_Set() { return in_set_flag == 1 ; }
    bool Is_out_Set() { return out_set_flag == 1 ; }
    uint64_t Get_in_ID() { return in_io_id ; }
    uint64_t Get_out_ID() { return out_io_id ; }
} io_container ;

using namespace std ;


class Wrapper_Optimizer
{
    public:
        explicit Wrapper_Optimizer(vector<scanchain_t> *sc_array, vector<io_cell_t> *io_cells) :
         _sc_array(sc_array), _io_cells(io_cells), _wc_array(0), _tt_mover(0), _tsv_count(0), _test_time(0) , _curr_T(1) {}

        ~Wrapper_Optimizer()
        {
            for (uint64_t i = 0; i < _wc_array->size(); i++)
            {
                delete _wc_array->at(i) ;
            }
            delete _wc_array ;
            delete _tt_mover ;
        }
        
        uint64_t Get_Count_TSV() ;
        uint64_t Get_Test_Time() ;
        two_phase_result_t *Two_Phase_Optimizer(uint8_t max_layer, uint64_t wrapper_chain_count) ;

    private:
        uint64_t IO_Cell_Assignment() ;
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
        multimap<uint8_t, uint64_t>::iterator& Get_best_WC_iterator(multimap<uint8_t, uint64_t> *layer_to_WCid, uint8_t key_layer) ;
        multimap<uint8_t, uint64_t>::iterator& Get_WC_iterator(multimap<uint8_t, uint64_t> *layer_to_WCid, uint8_t key_layer, uint64_t wc_id) ;

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
        vector<io_cell_t> *_io_cells ;
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