#ifndef WU_GROUP_H
#define WU_GROUP_H

#include "SoC_Lexer.h"
#include "Wrapper_Chain.h"
#include<map>
#include<vector>

using namespace std ;

class WU_Group
{   
    public:
        explicit WU_Group()
        {
            _id_to_sc = new map<uint64_t, scanchain_t> ;
            _id_to_group = 0 ;
            _stored_id_to_group = 0 ;
            _wc_id_array = new vector<uint64_t> ;
            _max_tt = (uint64_t)0 ;
            _tsv_count = (uint64_t)0 ;
            _freeze_since_last_tad = (uint8_t)0 ;       // Not freezed
            _sub_grp1 = 0 ;
            _sub_grp2 = 0 ;
        }

        ~WU_Group()
        {
            delete _id_to_sc ;
            delete _id_to_group ;
            delete _stored_id_to_group ;
            delete _wc_id_array ;
            delete _sub_grp1 ;
            delete _sub_grp2 ;
        }

        void Insert_SC(const scanchain_t ele) ;
        uint8_t Delete_SC(const uint64_t sc_id) ;
        void Insert_WC(const uint64_t wc_id) { _wc_id_array->push_back(wc_id) ; }
        void Randomly_Partition_SubGroups() ;
        void Interchange_SC_SubGroup(const uint64_t sc_id) ;
        WU_Group* Get_SubGroup1() { return _sub_grp1 ; }
        WU_Group* Get_SubGroup2() { return _sub_grp2 ; }
        vector<uint64_t>* Get_ScanchainIDs() ;
        uint64_t Get_Max_tt() { return _max_tt ; }
        uint64_t Get_TSV() { return _tsv_count ; }
        void Set_Insert_all_SC_IDs(set<uint64_t> *set_ele) ;
        double Calculate_Gain(const uint64_t sc_id) ;
        uint64_t Get_WC_Size() { return _wc_id_array->size() ;}
        void Store_state() ;
        void Restore_state() ;

    private:
        void Update_SC_SubGroup(const uint64_t sc_id, const uint8_t grp) ;
        void Set_Max_tt(uint64_t tt_max) { _max_tt = tt_max ; }

    private:
        map<uint64_t, scanchain_t> *_id_to_sc ;
        map<uint64_t, uint8_t> *_id_to_group ;
        map<uint64_t, uint8_t> *_stored_id_to_group ;
        vector<uint64_t> *_wc_id_array ;
        uint64_t _max_tt ;
        uint64_t _tsv_count ;
        uint8_t _freeze_since_last_tad ;
        WU_Group *_sub_grp1 ;
        WU_Group *_sub_grp2 ;
};

#endif