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

    delete _wc_array ;
    delete _tt_mover ;

    _wc_array = new vector<Wrapper_Chain*> ;
    _tt_mover = new TT_Movement(_wc_count) ;

    for (uint8_t i = 0; i < _wc_count; i++)
    {
        _wc_array->push_back(new Wrapper_Chain(i)) ;
    }

    Initialize_WrapperChains() ;    //Initializes the Wrapper Chain
    Initialize_TT_Mover() ;    //Initializes _tt_move_array
    Initialize_WrapperChain_TSV() ; //Initializes Wrapper Chain's TSV

    Count_TSV() ;   // Counts the current TSV
    Count_TT() ;    // Counts current Test Time

    //Simulated_Annelation() ;

}

void Wrapper_Optimizer::Initialize_WrapperChain_TSV() 
{
    for (uint64_t i = 0; i < _wc_count; i++)
    {
        _wc_array->at(i)->Initialize_TSV_Count() ;
    }
}

void Wrapper_Optimizer::Initialize_TT_Mover()
{
    uint64_t sc_id ;
    for (uint64_t i = 0; i < _wc_count; i++)
    {
        for (uint64_t j = 0; j < _wc_count; j++)
        {
            tt_move_t tem ;
            tem.destination_wc = j ;
            tem.source_wc = i ;

            if (i == j) {
                sc_id = _wc_array->at(i)->Get_Closest_TT_id(0) ; 
            } else if (_wc_array->at(i)->Get_tt() <= _wc_array->at(j)->Get_tt()) {
                sc_id = _wc_array->at(i)->Get_Closest_TT_id(0) ;
            } else {
                uint64_t tt_key = _wc_array->at(i)->Get_tt() - _wc_array->at(j)->Get_tt() ;
                sc_id = _wc_array->at(i)->Get_Closest_TT_id(tt_key) ;
            }
            tem.delta_entropy = Get_Delta_TT_Entropy(i, j, sc_id) ;
            tem.sc_id = sc_id ;
            _tt_mover->Update_TT_Move(tem) ;
        }
    }
}

void Wrapper_Optimizer::WrapperChain_TT_Update(uint64_t wc_id) 
{
    uint64_t sc_id ;
    // Updates tt movement from wc_id to others
    for (uint64_t i = 0; i < _wc_count; i++)
    {
        tt_move_t tem ;
        tem.destination_wc = i ;
        tem.source_wc = wc_id ;

        if (i == wc_id) {
            //sc_id = _wc_array->at(i)->Get_Closest_TT_id(0) ; 
            continue ;
        } else if (_wc_array->at(wc_id)->Get_tt() <= _wc_array->at(i)->Get_tt()) {
            sc_id = _wc_array->at(wc_id)->Get_Closest_TT_id(0) ;
        } else {
            uint64_t tt_key = _wc_array->at(wc_id)->Get_tt() - _wc_array->at(i)->Get_tt() ;
            sc_id = _wc_array->at(wc_id)->Get_Closest_TT_id(tt_key) ;
        }
        tem.delta_entropy = Get_Delta_TT_Entropy(wc_id, i, sc_id) ;
        tem.sc_id = sc_id ;
        _tt_mover->Update_TT_Move(tem) ;

    }

    // Updates tt movement from others to wc_id
    for (uint64_t i = 0; i < _wc_count; i++)
    {
        tt_move_t tem ;
        tem.destination_wc = wc_id ;
        tem.source_wc = i ;

        if (i == wc_id) {
            //sc_id = _wc_array->at(i)->Get_Closest_TT_id(0) ; 
            continue ;
        } else if (_wc_array->at(i)->Get_tt() <= _wc_array->at(wc_id)->Get_tt()) {
            sc_id = _wc_array->at(i)->Get_Closest_TT_id(0) ;
        } else {
            uint64_t tt_key = _wc_array->at(i)->Get_tt() - _wc_array->at(wc_id)->Get_tt() ;
            sc_id = _wc_array->at(i)->Get_Closest_TT_id(tt_key) ;
        }
        tem.delta_entropy = Get_Delta_TT_Entropy(i, wc_id, sc_id) ;
        tem.sc_id = sc_id ;
        _tt_mover->Update_TT_Move(tem) ;

    }

}

void Wrapper_Optimizer::Initialize_WrapperChains()
{
    // Descending Sorting required for assignment
    std::sort(_sc_array->begin(), _sc_array->end(), Wrapper_Optimizer::SC_tt_compare) ;

    // Used to assign the max tt sc, to less tt wc
    std::make_heap(_wc_array->begin(), _wc_array->end(), Wrapper_Optimizer::WC_tt_min_compare) ;

    for (uint64_t i = 0; i < _sc_array->size(); i++) 
    {
        std::pop_heap(_wc_array->begin(), _wc_array->end(), Wrapper_Optimizer::WC_tt_min_compare) ;
        Wrapper_Chain *tem = _wc_array->at(_wc_array->size() - 1) ;

        _sc_array->at(i).wrapper_chain = tem->Get_id() ;
        tem->Insert_SC(_sc_array->at(i)) ;

        std::push_heap(_wc_array->begin(), _wc_array->end(), Wrapper_Optimizer::WC_tt_min_compare) ;
    }

    for (uint64_t i = 0 ; i < _wc_array->size(); i++)
    {
        std::cout<<"Wc_id = "<<std::to_string(_wc_array->at(i)->Get_id())<<"\t tt = "<<std::to_string(_wc_array->at(i)->Get_tt())<<std::endl ;
        std::cout<<std::endl ;
        _wc_array->at(i)->PrintScanchains() ;
    }
    std::cout<<"Initialization done"<<std::endl ;
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
        _tt_mover->Reset_Lock() ;

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
    const delta_tsv_t delta_tsv = Get_Delta_TSV(sc_id, wc_id) ;

    // Returns penalty in test time, if scanchain sc_id 
    // is moved to wc_id
    double tt_penalty = Get_TT_Penalty(sc_id, wc_id) ;

    double gain = (-1 * ((double)delta_tsv.del_tsv/_tsv_count)) + (tt_penalty) ;

    if (gain >= 0) {
        Move_SC(sc_id, wc_id, delta_tsv) ;   // (_tsv_count + delta_tsv) is new tsv count
    } else {
        double r = rand() % RAND_MAX ;
        if(r < exp(gain/_curr_T)) {
            Move_SC(sc_id, wc_id, delta_tsv) ;
        }
    }

}

const delta_tsv_t Wrapper_Optimizer::Get_Delta_TSV(uint64_t sc_id, uint64_t wc_id)
{
    uint64_t curr_tsv = _tsv_count ;
    uint64_t curr_wc = _sc_array->at(sc_id).wrapper_chain ;

    assert (curr_wc != wc_id) ;

    uint64_t curr_source_tsv = _wc_array->at(curr_wc)->Get_tsv() ;
    uint64_t curr_destination_tsv = _wc_array->at(wc_id)->Get_tsv() ;

    delta_tsv_t tem ;

    tem.source_new_tsv = _wc_array->at(curr_wc)->Get_TSV_deletion(_sc_array->at(sc_id)) ;
    tem.destination_new_tsv = _wc_array->at(wc_id)->Get_TSV_insertion(_sc_array->at(sc_id)) ;

    tem.del_tsv = (tem.destination_new_tsv + tem.source_new_tsv) - (curr_source_tsv + curr_destination_tsv) ;
    tem.total_tsv = curr_tsv + tem.del_tsv ;

    return tem ;
}

void Wrapper_Optimizer::Move_SC(uint64_t sc_id, uint64_t wc_id, const delta_tsv_t& delta_tsv)
{
    uint64_t pre_wc = _sc_array->at(sc_id).wrapper_chain ;

    assert (wc_id != pre_wc) ;

    _sc_array->at(sc_id).wrapper_chain = wc_id ;
    _tsv_count = delta_tsv.total_tsv ;

    _wc_array->at(pre_wc)->Delete_SC(sc_id) ;
    _wc_array->at(pre_wc)->Set_tsv(delta_tsv.source_new_tsv) ;

    _wc_array->at(wc_id)->Insert_SC(_sc_array->at(sc_id)) ;
    _wc_array->at(wc_id)->Set_tsv(delta_tsv.destination_new_tsv) ;

    WrapperChain_TT_Update(pre_wc) ;
    WrapperChain_TT_Update(wc_id) ;
}

void Wrapper_Optimizer::Minimize_TT_Phase() 
{
    
}