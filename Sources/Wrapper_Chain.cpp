#include "Wrapper_Chain.h"

void Wrapper_Chain::Insert_SC(const scanchain_t ele)
{
    //_sc_list->push_back(ele) ;
    if (_id_to_sc->find(ele.sc_id) != _id_to_sc->end()) {
        _id_to_sc->insert({ele.sc_id, ele}) ;
        _tt_to_id->insert({ele.test_time, ele.sc_id}) ;

        _test_time += ele.test_time ;
    } else {
        std::cerr << "Scanchain element of id = " << std::to_string(ele.sc_id) << "already in the Wrapper Chain with id = " << to_string(this->_wc_id) <<". Can't insert again." <<std::endl ;
        exit(0) ;
    }
}

uint8_t Wrapper_Chain::Delete_SC(const uint64_t sc_id)
{
    auto it = _id_to_sc->find(sc_id) ;
    if (it != _id_to_sc->end()) {
        _test_time = it->second.test_time ;

        auto it_tt = _tt_to_id->equal_range(it->second.test_time) ;
        for (auto i = it_tt.first; i != it_tt.second; i++) 
        {
            if(i->second == sc_id) {
                _tt_to_id->erase(i) ;
                break ;
            }
        }
        _id_to_sc->erase(it) ;
    } else {
        std::cerr << "Scanchain element of id = " << std::to_string(sc_id) << "not in the Wrapper Chain with id = " << to_string(this->_wc_id) <<". Can't delete." <<std::endl ;
        exit(0) ;
    }
}

uint64_t Wrapper_Chain::Get_Closest_TT_id(uint64_t tt_key) {
    auto itlow = _tt_to_id->lower_bound(tt_key) ;

    if (itlow == _tt_to_id->end()) {
        std::cerr << "No tt element found. tt_key = " << std::to_string(tt_key) << ".\t wc_id = " << std::to_string(this->_wc_id) << std::endl ;
        exit(0) ;
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