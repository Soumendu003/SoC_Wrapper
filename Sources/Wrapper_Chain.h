#ifndef WRAPPER_CHAIN_H
#define WRAPPER_CHAIN_H

#include"SoC_Lexer.h"
#include"Header.h"
#include<set>
#include<map>
#include<vector>

using namespace std ;

class Wrapper_Chain
{
    public:
        Wrapper_Chain(uint8_t id) : _wc_id(id), _test_time((uint64_t)0)
        {
            _id_to_sc = new map<uint64_t, scanchain_t> ;
            _tt_to_id = new multimap<uint64_t, uint64_t> ;
        }

        ~Wrapper_Chain()
        {
            delete _id_to_sc ;
            delete _tt_to_id ;
        }

        void Insert_SC (const scanchain_t ele) ;
        uint8_t Delete_SC(const uint64_t sc_id) ;
        uint64_t Get_tt() { return _test_time ; }
        uint64_t Get_id() { return _wc_id ; }
        uint64_t Get_Closest_TT_id(uint64_t tt_key) ;
        void PrintScanchains() ;
    private:


    private:
        uint8_t _wc_id ;
        map<uint64_t, scanchain_t> *_id_to_sc ;
        multimap<uint64_t, uint64_t> *_tt_to_id ;
        uint64_t _test_time ;
};

#endif