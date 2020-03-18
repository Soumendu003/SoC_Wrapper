#ifndef TT_MOVEMENT_H
#define TT_MOVEMENT_H

#include "Wrapper_Chain.h"
#include<iostream>
#include<map>
#include<set>
#include<queue>

using namespace std ;

typedef struct
{
    uint64_t source_wc ;
    uint64_t destination_wc ;
    uint64_t sc_id ;
    double delta_entropy ;
} tt_move_t ;

typedef struct
{
    uint64_t source ;
    uint64_t destination ;
    uint64_t sc_id ;
} tt_dir_t ;

typedef tt_dir_t tt_dir_t ;


class TT_Movement
{
    public:
        explicit TT_Movement(uint64_t lock_size): _lock_size(lock_size) 
        {
            _dir_to_val = new map<pair<uint64_t, uint64_t>, double> ;
            _val_to_dir = new multimap<double, tt_dir_t> ;
            _tt_lock_set = new set<tuple<uint64_t, uint64_t, uint64_t>> ;
            _tt_lock_queue = new queue<tt_dir_t> ;
            //std::cerr<<"tt_mover created"<<std::endl;
        }

        ~TT_Movement()
        {
            //std::cerr<<"tt_mover destructor called"<<std::endl;
            delete _dir_to_val ;
            delete _val_to_dir ;
            delete _tt_lock_set ;
            delete _tt_lock_queue ;
            //std::cerr<<"tt_mover deleted"<<std::endl;
        }

        void Update_TT_Move(tt_move_t val) ;

        const tt_move_t& Get_TT_Move() ;

        void Reset_Lock() ;

    private:
        void Insert_Val_To_Dir(tt_move_t val) ;
        uint8_t Is_Lock_Free(tt_dir_t val) ;
        void Lock(tt_move_t val) ;

    private:
        uint64_t _lock_size ;
        map<pair<uint64_t, uint64_t>, double> *_dir_to_val ;
        multimap<double, tt_dir_t> *_val_to_dir ;
        set<tuple<uint64_t, uint64_t, uint64_t>> *_tt_lock_set ;
        queue<tt_dir_t> *_tt_lock_queue ;

};


#endif