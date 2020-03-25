#include "WU_Group.h"

void WU_Group::Insert_SC(const scanchain_t ele)
{
    if (_id_to_sc->find(ele.sc_id) == _id_to_sc->end()) {
        _id_to_sc->insert({ele.sc_id, ele}) ;
    } else {
        std::cerr << "Scanchain element of id = " << std::to_string(ele.sc_id) << " already in the Sub Group. Can't insert again." <<std::endl ;
        exit(0) ;
    }
}

uint8_t WU_Group::Delete_SC(const uint64_t sc_id)
{
    auto it = _id_to_sc->find(sc_id) ;
    if (it != _id_to_sc->end()) {
        _id_to_sc->erase(it) ;
        return 1 ;
    } else {
        std::cerr << "Scanchain element of id = " << std::to_string(sc_id) << "\tnot in the Sub Group. Can't delete." <<std::endl ;
        exit(0) ;
    }
}

void WU_Group::Randomly_Partition_SubGroups()
{
    //assert (_wc_id_array->size() > 1) ;

    delete _sub_grp1 ;
    delete _sub_grp2 ;
    delete _id_to_group ;

    _sub_grp1 = new WU_Group() ;
    _sub_grp2 = new WU_Group() ;
    _id_to_group = new map<uint64_t, uint8_t> ;

    uint8_t flag = 0 ;

    auto M = _id_to_sc;
    for (auto it = M->begin(); it != M->end(); it++)
    {
        if (flag == 0)
        {
            _sub_grp1->Insert_SC(it->second) ;
            Update_SC_SubGroup(it->first, (uint8_t)1) ;
            flag = 1 ;
        } else {
            _sub_grp2->Insert_SC(it->second) ;
            Update_SC_SubGroup(it->first, (uint8_t)2) ;
            flag = 0 ;
        }
    }

    auto V = _wc_id_array ;
    for (auto it = V->begin(); it != V->end(); it++)
    {
        if (flag == 0)
        {
            _sub_grp1->Insert_WC(*it) ;
            flag = 1 ;
        } else {
            _sub_grp2->Insert_WC(*it) ;
            flag = 0 ;
        }
    }
}

void WU_Group::Update_SC_SubGroup(const uint64_t sc_id, const uint8_t grp)
{
    auto it = _id_to_group->find(sc_id) ;

    if (it == _id_to_group->end())
    {
        _id_to_group->insert({sc_id, grp}) ;
    } else {
        it->second = grp ;
    }
}

void WU_Group::Interchange_SC_SubGroup(const uint64_t sc_id)
{
    auto it = _id_to_group->find(sc_id) ;

    if (it == _id_to_group->end()) {
        std::cerr << "Scanchain element of id = " << std::to_string(sc_id) << "\tnot in the _id_to_group Map. Can't interchange." <<std::endl ;
        exit(0) ;
    }

    auto it_sc = _id_to_sc->find(sc_id) ;

    if (it_sc == _id_to_sc->end()) {
        std::cerr << "Scanchain element of id = " << std::to_string(sc_id) << "\tnot in Parent Group. Can't interchange." <<std::endl ;
        exit(0) ;
    }

    if (it->second == 1) {
        _sub_grp1->Delete_SC(sc_id) ;
        _sub_grp2->Insert_SC(it_sc->second) ;
        it->second = 2 ;
    } else {
        _sub_grp2->Delete_SC(sc_id) ;
        _sub_grp1->Insert_SC(it_sc->second) ;
        it->second = 1 ;
    }
}

vector<uint64_t>* WU_Group::Get_ScanchainIDs() 
{
    vector<uint64_t> *ret_array = new vector<uint64_t> ;

    for (auto it = _id_to_sc->begin(); it != _id_to_sc->end(); it++)
    {
        ret_array->push_back(it->first) ;
    }
    return ret_array ;
}

vector<scanchain_t>* WU_Group::Get_Scanchains()
{
    vector<scanchain_t> *ret_array  = new vector<scanchain_t> ;

    for (auto it = _id_to_sc->begin(); it != _id_to_sc->end(); it++)
    {
        ret_array->push_back(it->second) ;
    }
    return ret_array ;
}

void WU_Group::Set_Insert_all_SC_IDs(set<uint64_t> *set_ele)
{
    for (auto it = _id_to_sc->begin(); it != _id_to_sc->end(); it++)
    {
        set_ele->insert(it->first) ;
    }
}

void WU_Group::Store_state() {
    if (_stored_id_to_group == 0) {
        _stored_id_to_group = new map<uint64_t, uint8_t> ;
    }

    _stored_id_to_group->clear() ;

    for (auto it = _id_to_group->begin(); it != _id_to_group->end(); it++)
    {
        _stored_id_to_group->insert({it->first, it->second}) ;
    }
}

void WU_Group::Restore_state() 
{
    for (auto it = _stored_id_to_group->begin(); it != _stored_id_to_group->end(); it++)
    {
        if (_id_to_group->find(it->first)->second != it->second) {
            Interchange_SC_SubGroup(it->first) ;
        }
    }

    _stored_id_to_group->clear() ;
}

double WU_Group::Get_adj() 
{
    //assert (_wc_id_array->size() > 1) ;
    //assert (_id_to_sc->size() > 0) ;

    double ret_val = 1.3 ;

    ret_val += (3*(_wc_id_array->size() - 2)) / (_id_to_sc->size() * 1.6 ) ;

    if (ret_val > 2.9)
    {
        return 2.9 ;
    } else if (ret_val < 1.3)
    {
        return 1.3 ;
    }
    return ret_val ;
}

void WU_Group::Set_TAD_result(uint64_t max_tt, uint64_t tsv_count)
{
    _max_tt = max_tt ;
    _tsv_count = tsv_count ;
}