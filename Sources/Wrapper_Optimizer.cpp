#include "Wrapper_Optimizer.h"

uint64_t Wrapper_Optimizer::Get_Count_TSV()
{
    uint64_t tsv = 0 ;
    for (uint64_t i = 0; i < _wc_count; i++)
    {
        _wc_array->at(i)->Initialize_TSV_Count() ;
        tsv += _wc_array->at(i)->Get_tsv() ;
    }
    return tsv ;
}

uint64_t Wrapper_Optimizer::Get_Test_Time()
{
    uint64_t tt = 0 ;
    for (uint64_t i = 0; i < _wc_count; i++)
    {
        if (_wc_array->at(i)->Get_tt() > tt)
        {
            tt = _wc_array->at(i)->Get_tt() ;
        }
    }
    return tt ;
}

void Wrapper_Optimizer::Clear_Wrapper_Assignments()
{
    for (uint64_t i = 0; i < _sc_array->size(); i++)
    {
        _sc_array->at(i).wrapper_chain = 0 ;
    }

    _wc_count = 0 ;
    _tsv_count = 0 ;
    _test_time = 0 ;
    _curr_T = 1 ;

    delete _wc_array ;
    delete _tt_mover ;
}

two_phase_result_t * Wrapper_Optimizer::Two_Phase_Optimizer(uint8_t max_layer, uint64_t wrapper_chain_count)
{
    Clear_Wrapper_Assignments() ;

    _wc_count = wrapper_chain_count ;
    _max_layer = max_layer ;

    _tsv_over_tt = 0 ;
    _tt_over_tsv = 0 ;

    _wc_array = new vector<Wrapper_Chain*> ;
    _tt_mover = new TT_Movement(_wc_count) ;

    for (uint64_t i = 0; i < _wc_count; i++)
    {
        _wc_array->push_back(new Wrapper_Chain(i)) ;
    }

    auto start = chrono::high_resolution_clock::now();

    Initialize_WrapperChains() ;    //Initializes the Wrapper Chain

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

    //std::cout<<"Initial Solution time taken in miliseconds = "+to_string(duration.count())<<std::endl ;

    static two_phase_result_t *ret_val = new two_phase_result_t ;
    ret_val->initial_partition_time = to_string(duration.count()) ;

    Initialize_TT_Mover() ;    //Initializes _tt_move_array
    Initialize_WrapperChain_TSV() ; //Initializes Wrapper Chain's TSV

    Simulated_Annelation(ret_val) ;

    return ret_val ;
}

void Wrapper_Optimizer::Initialize_WrapperChain_TSV() 
{
    _tsv_count = 0 ;
    for (uint64_t i = 0; i < _wc_count; i++)
    {
        _wc_array->at(i)->Initialize_TSV_Count() ;
        _tsv_count += _wc_array->at(i)->Get_tsv() ;
    }
    //std::cout << "TSV count = "<<std::to_string(_tsv_count)<<std::endl ;
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
            sc_id = _wc_array->at(wc_id)->Get_Closest_TT_id((uint64_t)0) ;
        } else {
            uint64_t tt_key = _wc_array->at(wc_id)->Get_tt() - _wc_array->at(i)->Get_tt() ;
            sc_id = _wc_array->at(wc_id)->Get_Closest_TT_id(tt_key) ;
        }

        if (sc_id >= _sc_array->size()) {
            tem.delta_entropy = -100 ;
            tem.sc_id = -1 ;
        } else {
            tem.delta_entropy = Get_Delta_TT_Entropy(wc_id, i, sc_id) ;
            tem.sc_id = sc_id ;
        } 
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
        
        if (sc_id >= _sc_array->size()) {
            tem.delta_entropy = -100 ;
            tem.sc_id = -1 ;
        } else {
            tem.delta_entropy = Get_Delta_TT_Entropy(wc_id, i, sc_id) ;
            tem.sc_id = sc_id ;
        } 
        _tt_mover->Update_TT_Move(tem) ;
    }

}

void Wrapper_Optimizer::Initialize_WrapperChains()
{
    auto tem_sc_array = *_sc_array ;
    // Descending Sorting required for assignment
    std::sort(tem_sc_array.begin(), tem_sc_array.end(), Wrapper_Optimizer::SC_tt_compare) ;

    // Used to assign the max tt sc, to less tt wc
    std::make_heap(_wc_array->begin(), _wc_array->end(), Wrapper_Optimizer::WC_tt_min_compare) ;

    _total_tt = 0 ;

    for (uint64_t i = 0; i < _sc_array->size(); i++) 
    {
        std::pop_heap(_wc_array->begin(), _wc_array->end(), Wrapper_Optimizer::WC_tt_min_compare) ;
        Wrapper_Chain *tem = _wc_array->at(_wc_array->size() - 1) ;

        tem_sc_array.at(i).wrapper_chain = tem->Get_id() ;
        _sc_array->at(tem_sc_array.at(i).sc_id).wrapper_chain = tem->Get_id() ;
        tem->Insert_SC(tem_sc_array.at(i)) ;
        
        //calculates total tt
        _total_tt += tem_sc_array.at(i).test_time ;

        std::push_heap(_wc_array->begin(), _wc_array->end(), Wrapper_Optimizer::WC_tt_min_compare) ;
    }

    std::sort(_wc_array->begin(), _wc_array->end(), Wrapper_Optimizer::WC_index_compare) ;

    
    //std::cout<<"Initialization done"<<std::endl ;
}

void Wrapper_Optimizer::Simulated_Annelation(two_phase_result_t *ret_val)
{
    ret_val->init_tsv = _tsv_count ;
    ret_val->init_max_tt = Get_Test_Time() ;

    assert (_tsv_count == Get_Count_TSV()) ;
    double T = 60000 ;    // Initial Temperature
    uint16_t step = _sc_array->size() ;     //No os intermediate step per T(temperature) value

    auto start = chrono::high_resolution_clock::now();
    while (T > 1)
    {
        _curr_T = T ;
        //std::cerr<<"Current temp = "<<std::to_string(_curr_T)<<std::endl ;
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
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

    //std::cout<<"Simulated Annelation time taken in miliseconds = "+to_string(duration.count())<<std::endl ;
    ret_val->simulated_annelation_time = to_string(duration.count()) ;

    uint64_t final_tsv_calculated = Get_Count_TSV() ;
    if (final_tsv_calculated < _tsv_count){
        ret_val->final_tsv = final_tsv_calculated ;
    } else {
        ret_val->final_tsv = _tsv_count ;
    }
    ret_val->final_max_tt = Get_Test_Time() ;
}

void Wrapper_Optimizer::Minimize_TSV_Phase()
{
    uint64_t sc_id = rand() % _sc_array->size() ;

    uint8_t wc_id = rand() % _wc_count ;

    // Makes sure that 
    while (wc_id == _sc_array->at(sc_id).wrapper_chain)
    {
        wc_id = rand() % _wc_count ;
    }
    
    // Returns change in TSV count, if scanchain sc_id is
    // moved to wc_id
    assert(_sc_array->at(sc_id).wrapper_chain != wc_id) ;
    const delta_tsv_t delta_tsv = Get_Delta_TSV(sc_id, wc_id) ;

    // Returns test time penalty, if scanchain sc_id 
    // is moved to wc_id
    double tt_penalty = Get_TT_Penalty(sc_id, wc_id) ;

    double gain = (-1 * ((double)delta_tsv.del_tsv/delta_tsv.total_tsv)) - (tt_penalty) ;

    if (gain >= 0) {
        Move_SC(sc_id, wc_id, delta_tsv) ;   // (_tsv_count + delta_tsv) is new tsv count
        _tsv_over_tt += 1 ;
    } else {
        double r = rand() % RAND_MAX ;
        if(r < exp(gain/_curr_T)) {
            Move_SC(sc_id, wc_id, delta_tsv) ;
        }
    }

}

const delta_tsv_t Wrapper_Optimizer::Get_Delta_TSV(uint64_t sc_id, uint64_t destination_wc)
{
    uint64_t curr_tsv = _tsv_count ;
    uint64_t curr_wc = _sc_array->at(sc_id).wrapper_chain ;

    assert (curr_wc != destination_wc) ;

    uint64_t curr_source_tsv = _wc_array->at(curr_wc)->Get_tsv() ;
    uint64_t curr_destination_tsv = _wc_array->at(destination_wc)->Get_tsv() ;
    //std::cout << "\n\nInitial total tsv = " << std::to_string(curr_source_tsv + curr_destination_tsv) << std::endl ;

    delta_tsv_t tem ;

    assert(_sc_array->at(sc_id).sc_id == sc_id) ;

    tem.source_new_tsv = _wc_array->at(curr_wc)->Get_TSV_deletion(_sc_array->at(sc_id)) ;
    tem.destination_new_tsv = _wc_array->at(destination_wc)->Get_TSV_insertion(_sc_array->at(sc_id)) ;
    //std::cout << "\nNext total tsv = " << std::to_string(tem.destination_new_tsv + tem.source_new_tsv) << std::endl ;


    tem.del_tsv = (int)(tem.destination_new_tsv + tem.source_new_tsv) - (int)(curr_source_tsv + curr_destination_tsv) ;
    //std::cout << "\n\nDelta TSV = " << std::to_string(tem.del_tsv) << std::endl ;
    tem.total_tsv = (curr_tsv + tem.del_tsv) ;
    assert(tem.del_tsv == (int)(tem.total_tsv - curr_tsv)) ;

    return tem ;
}

void Wrapper_Optimizer::Move_SC(uint64_t sc_id, uint64_t wc_id, const delta_tsv_t& delta_tsv)
{
    uint64_t pre_wc = _sc_array->at(sc_id).wrapper_chain ;

    assert (wc_id != pre_wc) ;

    _sc_array->at(sc_id).wrapper_chain = wc_id ;
    _tsv_count = delta_tsv.total_tsv ;
    //_tsv_count -= (_wc_array->at(pre_wc)->Get_tsv() + _wc_array->at(wc_id)->Get_tsv()) ;

    _wc_array->at(pre_wc)->Delete_SC(sc_id) ;
    _wc_array->at(pre_wc)->Set_tsv(delta_tsv.source_new_tsv) ;
    //_wc_array->at(pre_wc)->Initialize_TSV_Count();

    _wc_array->at(wc_id)->Insert_SC(_sc_array->at(sc_id)) ;
    _wc_array->at(wc_id)->Set_tsv(delta_tsv.destination_new_tsv) ;
    //_wc_array->at(wc_id)->Initialize_TSV_Count();

    WrapperChain_TT_Update(pre_wc) ;
    WrapperChain_TT_Update(wc_id) ;
}

void Wrapper_Optimizer::Minimize_TT_Phase() 
{
    const tt_move_t move_ele = _tt_mover->Get_TT_Move() ;

    if (move_ele.destination_wc == move_ele.source_wc)
    {
        std::cerr<<"source_wc = "<<std::to_string(move_ele.source_wc)<<"\t destination_wc = "<<std::to_string(move_ele.destination_wc)<<std::endl ;
        std::cerr<<"eleemnt sc_id = "<<std::to_string(move_ele.sc_id)<<std::endl;
        std::cerr<<"move key = "<<std::to_string(move_ele.delta_entropy)<<std::endl ;
        std::cerr<<"sc source_wc = "<<std::to_string(_sc_array->at(move_ele.sc_id).wrapper_chain)<<std::endl ;
    }
    assert (move_ele.destination_wc != _sc_array->at(move_ele.sc_id).wrapper_chain) ;
    //assert (move_ele.destination_wc != move_ele.source_wc) ;

    const delta_tsv_t delta_tsv = Get_Delta_TSV(move_ele.sc_id, move_ele.destination_wc) ;

    double del_tt_entropy = Get_Delta_TT_Entropy(move_ele.source_wc, move_ele.destination_wc, move_ele.sc_id) ;

    //std::cout<<"\ndel_tt_entropy val = "<<std::to_string(del_tt_entropy)<<std::endl ;
    //std::cout<<"tsv penalty val = "<<std::to_string(Get_TSV_Penalty(delta_tsv))<<"\n"<<std::endl ;

    double gain =  del_tt_entropy - Get_TSV_Penalty(delta_tsv) ;

    if (gain >= 0) {
        _tt_over_tsv += 1 ;
        Move_SC(move_ele.sc_id, move_ele.destination_wc, delta_tsv) ;   // (_tsv_count + delta_tsv) is new tsv count
    } else {
        double r = rand() % RAND_MAX ;
        if(r < exp(gain/_curr_T)) {
            Move_SC(move_ele.sc_id, move_ele.destination_wc, delta_tsv) ;
        }
    }

}

double Wrapper_Optimizer::Get_TT_Penalty(uint64_t sc_id, uint64_t destination_wc)
{
    uint64_t source_wc = _sc_array->at(sc_id).wrapper_chain ;

    assert (source_wc != destination_wc) ;

    double del_entropy = Get_Delta_TT_Entropy(source_wc, destination_wc, sc_id) ;

    if (del_entropy >= 0) {
        return 0 ;
    } else {
        return 1 - exp(del_entropy/_curr_T) ;
    }

}

double Wrapper_Optimizer::Entropy(double val, double total_val)
{
    if (val == 0) {
        return 0 ;
    } else if (total_val == 0)
    {
        return 0 ;
    }

    double p = (val/total_val) ;

    return (-1) * p * (log(p)/log(_wc_count)) ;
}

double Wrapper_Optimizer::Get_TSV_Penalty(const delta_tsv_t& delta_tsv)
{
    if (delta_tsv.del_tsv <= 0) {
        return 0 ;
    } else {
        return 1 - exp((-1 * delta_tsv.del_tsv)/(_curr_T * delta_tsv.total_tsv)) ;
    }
}

double Wrapper_Optimizer::Get_Delta_TT_Entropy(uint64_t source_wc_id, uint64_t destination_wc_id, uint64_t sc_id)
{
    uint64_t tt = _sc_array->at(sc_id).test_time ;
    double init_entropy = Entropy(_wc_array->at(source_wc_id)->Get_tt(), _total_tt) + Entropy(_wc_array->at(destination_wc_id)->Get_tt(), _total_tt) ; 
    double final_entropy = Entropy(_wc_array->at(source_wc_id)->Get_tt() - tt, _total_tt) + Entropy(_wc_array->at(destination_wc_id)->Get_tt() + tt, _total_tt) ;

    return final_entropy - init_entropy ;
}

void Wrapper_Optimizer::copy_result(two_phase_result_t *to_var, two_phase_result_t *from_var) 
{
    to_var->final_max_tt = from_var->final_max_tt ;
    to_var->final_tsv = from_var->final_tsv ;
    to_var->init_max_tt = from_var->init_max_tt ;
    to_var->init_tsv = from_var->init_tsv ;
    to_var->initial_partition_time = from_var->initial_partition_time ;
    to_var->simulated_annelation_time = from_var->simulated_annelation_time ;   
}

bool Wrapper_Optimizer::greater_than_result(two_phase_result_t *var1, two_phase_result_t *var2)
{
    if (var1->final_tsv > var2->final_tsv) {
        return true ;
    }

    if (var1->final_max_tt > var2->final_max_tt) {
        return true ;
    }

    return false ;
}