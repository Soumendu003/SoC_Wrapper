#include "Wrapper_Optimizer.h"

uint64_t Wrapper_Optimizer::Get_Count_TSV()
{
    return _tsv_count ;
}

uint64_t Wrapper_Optimizer::Get_Test_Time()
{
    return _test_time ;
}

void Wrapper_Optimizer::Clear_Wrapper_Assignments()
{
    for (uint64_t i = 0; i < _sc_array->size(); i++)
    {
        _sc_array->at(i).wrapper_chain = 0 ;
    }
}

void Wrapper_Optimizer::Two_Phase_Optimizer(uint8_t max_layer, uint8_t wrapper_chain_count)
{
    _wc_count = wrapper_chain_count ;
    _max_layer = max_layer ;

    Initialize_WrapperChains() ;    //Initializes the Wrapper Chain

    Count_TSV() ;   // Counts the current TSV
    Count_TT() ;    // Counts current Test Time

    Simulated_Annelation() ;

}

void Wrapper_Optimizer::Simulated_Annelation()
{
    double T = 60000 ;    // Initial Temperature
    uint16_t step = _sc_array->size() ;     //No os intermediate step per T(temperature) value

    

    while (T > 1)
    {
        for (uint16_t i = 0; i < step; i++)
        {
            Minimize_TSV_Phase() ;
        }

        for(uint16_t i = 0; i < step; i++)
        {
            Minimize_TT_Phase() ;
        }

        T = 0.9*T ;     // reduces the T value 10%
    }
}

void Wrapper_Optimizer::Minimize_TSV_Phase()
{
    uint64_t sc_id = rand() % _sc_array->size() ;

    uint8_t wc_id = rand() % _wc_count ;

    // Makes sure that 
    while (wc_id != _sc_array->at(sc_id).wrapper_chain)
    {
        wc_id = rand() % _wc_count ;
    }
    
    // Returns change in TSV count, if scanchain sc_id is
    // moved to wc_id
    int delta_tsv = Get_Delta_TSV(sc_id, wc_id) ;

    // Returns penalty in test time, if scanchain sc_id 
    // is moved to wc_id
    double tt_penalty = Get_TT_Penalty(sc_id, wc_id) ;

    double gain = (-1 * ((double)delta_tsv/_tsv_count)) + (tt_penalty) ;

    if (gain >= 0) {
        Move_SC(sc_id, wc_id, (_tsv_count + delta_tsv)) ;   // (_tsv_count + delta_tsv) is new tsv count
    } else {
        double r = rand() % RAND_MAX ;
        if(r < exp(gain/_curr_T)) {
            Move_SC(sc_id, wc_id, (_tsv_count + delta_tsv)) ;
        }
    }

}

void Wrapper_Optimizer::Move_SC(uint64_t sc_id, uint64_t wc_id, uint64_t new_tsv_count)
{
    _sc_array->at(sc_id).wrapper_chain = wc_id ;
    _tsv_count = new_tsv_count ;
}