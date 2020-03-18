#include "Wrapper_Chain.h"

void Wrapper_Chain::Insert_SC(const scanchain_t ele)
{
    //_sc_list->push_back(ele) ;
    // std::cout<<"Size of _id_to_sc = "<<std::to_string(_id_to_sc->size())<<std::endl;
    if (_id_to_sc->find(ele.sc_id) == _id_to_sc->end()) {
        _id_to_sc->insert({ele.sc_id, ele}) ;
        _inLayer_to_id->insert({ele.in_layer, ele.sc_id}) ;
        _tt_to_id->insert({ele.test_time, ele.sc_id}) ;
        //std::cerr<<"sc id = "<<std::to_string(ele.sc_id)<<" \t inserted at wc id = "<<std::to_string(_wc_id) <<std::endl ;

        _test_time += ele.test_time ;
    } else {
        std::cerr << "Scanchain element of id = " << std::to_string(ele.sc_id) << " already in the Wrapper Chain with id = " << to_string(this->_wc_id) <<". Can't insert again." <<std::endl ;
        exit(0) ;
    }
}

uint8_t Wrapper_Chain::Delete_SC(const uint64_t sc_id)
{
    auto it = _id_to_sc->find(sc_id) ;
    if (it != _id_to_sc->end()) {
        _test_time -= it->second.test_time ;

        auto it_tt = _tt_to_id->equal_range(it->second.test_time) ;
        for (auto i = it_tt.first; i != it_tt.second; i++) 
        {
            if(i->second == sc_id) {
                _tt_to_id->erase(i) ;
                break ;
            }
        }

        auto it_inLayer = _inLayer_to_id->equal_range(it->second.in_layer) ;
        for (auto i = it_inLayer.first; i != it_inLayer.second; i++)
        {
            if (i->second == sc_id) {
                _inLayer_to_id->erase(i) ;
                break ;
            }
        }

        _id_to_sc->erase(it) ;
        //std::cerr<<"Sc_id = "<<std::to_string(sc_id)<<"\tdeleted from wc_id = "<<std::to_string(_wc_id)<<std::endl ;

        return 1 ;
    } else {
        std::cerr << "Scanchain element of id = " << std::to_string(sc_id) << "\tnot in the Wrapper Chain with id = " << to_string(this->_wc_id) <<". Can't delete." <<std::endl ;
        exit(0) ;
    }
}

uint64_t Wrapper_Chain::Get_Closest_TT_id(uint64_t tt_key) {

    if (tt_key == 0) {
        return _tt_to_id->begin()->second ;
    }

    assert (_tt_to_id->size() > 0) ;

    if (_tt_to_id->size() == 1)
    {
        return _tt_to_id->begin()->second ;
    }

    auto itlow = _tt_to_id->lower_bound(tt_key) ;

    if (itlow == _tt_to_id->end()) {
        auto pre = std::prev(itlow) ;
        return pre->second ;
        //std::cerr << "No tt element found. tt_key = " << std::to_string(tt_key) << ".\t wc_id = " << std::to_string(this->_wc_id) << std::endl ;
        //exit(0) ;
    } else if (itlow == _tt_to_id->begin()) {
        return itlow->second ;
    } else {
        auto pre = std::prev(itlow) ;
        if ((tt_key - pre->first) < (itlow->first - tt_key)) {
            return pre->second ;
        } else {
            return itlow->second ;
        }
    }
}

void Wrapper_Chain::PrintScanchains()
{
    for (auto it = _id_to_sc->begin(); it != _id_to_sc->end(); it++)
    {
        std::cout << "Scanchain id = "<<std::to_string(it->first)<<std::endl ;
    }
}

uint64_t Wrapper_Chain::Get_Nearest_InLayer(multimap<uint8_t, uint64_t> &inLayer_map, uint8_t key)
{
    auto itlow = inLayer_map.lower_bound(key) ;

    uint64_t ret_val ;

    assert (inLayer_map.size() > 0) ;

    if (itlow == inLayer_map.begin()) {
        ret_val = itlow->second ;
        inLayer_map.erase(itlow) ;
        return ret_val ;
    }

    if (itlow == inLayer_map.end()) {
        auto pre = std::prev(itlow) ;
        ret_val = pre->second ;
        inLayer_map.erase(pre) ;
        return ret_val ;
    } else {
        auto pre = std::prev(itlow) ;
        if ((key - pre->first) < (itlow->first - key)) {
            ret_val = pre->second ;
            inLayer_map.erase(pre) ;
        } else {
            ret_val = itlow->second ;
            inLayer_map.erase(itlow) ;
        }
        
    }
    return ret_val ;
}

uint64_t Wrapper_Chain::Calculate_TSV(multimap<uint8_t, uint64_t>& tem_inLayer_to_id)
{
    //if (_inLayer_to_id->size() == 1)    return 0 ;

    // Copies all elements
    //auto tem_inLayer_to_id = *_inLayer_to_id;
    uint8_t near_outLayer = 0 ;
    uint64_t tsv_count = 0 ;

    if (tem_inLayer_to_id.size() <= 1) {
        return 0 ;
    }

    uint64_t sc_id = Get_Nearest_InLayer(tem_inLayer_to_id, near_outLayer) ;
    scanchain_t new_sc_ele = _id_to_sc->find(sc_id)->second ;

    while (tem_inLayer_to_id.size() > 0)
    {
        //assert (tem_inLayer_to_id.size() > 0 ) ;
        scanchain_t sc_ele = new_sc_ele;
        sc_id = Get_Nearest_InLayer(tem_inLayer_to_id, sc_ele.out_layer) ;
        new_sc_ele = _id_to_sc->find(sc_id)->second ;
        tsv_count += (uint64_t)abs(sc_ele.out_layer - new_sc_ele.in_layer) ;
    }
    
    return tsv_count ;
}

void Wrapper_Chain::Initialize_TSV_Count()
{
    auto tem = *_inLayer_to_id ;
    _tsv_count = Calculate_TSV(tem) ;
}

uint64_t Wrapper_Chain::Get_TSV_insertion(const scanchain_t ele)
{

    uint64_t curr_tsv = _tsv_count ;

    //assert(_id_to_sc->find(ele.sc_id) == _id_to_sc->end()) ;

    //_inLayer_to_id->insert({ele.in_layer, ele.sc_id}) ;
    Insert_SC(ele) ;

    Initialize_TSV_Count() ;

    uint64_t ret_val = _tsv_count ;

    Delete_SC(ele.sc_id) ;
    
    _tsv_count = curr_tsv ;

    return ret_val ;
}

uint64_t Wrapper_Chain::Get_TSV_deletion(const scanchain_t ele)
{
    uint64_t curr_tsv = _tsv_count ;

    Delete_SC(ele.sc_id) ;

    Initialize_TSV_Count() ;

    uint64_t ret_val = _tsv_count ;

    Insert_SC(ele) ;
    //_inLayer_to_id->insert({ele.in_layer, ele.sc_id}) ;

    _tsv_count = curr_tsv ;

    return ret_val ;
}