#include "TT_Movement.h"

void TT_Movement::Update_TT_Move(tt_move_t val)
{
    auto it = _dir_to_val->find(make_pair(val.source_wc, val.destination_wc)) ;
    if (it != _dir_to_val->end()) {
        auto it_val = _val_to_dir->equal_range(it->second) ;
        for (auto i = it_val.first; i != it_val.second; i++)
        {
            if (i->second.source == val.source_wc && i->second.destination == val.destination_wc) {
                _val_to_dir->erase(i) ;
                Insert_Val_To_Dir(val) ;
                break ;
            }
        }
        it->second = val.delta_entropy ;
        return ;
    } else {
        _dir_to_val->insert({make_pair(val.source_wc, val.destination_wc), val.delta_entropy}) ;
        Insert_Val_To_Dir(val) ;
        return ;
    }
}

const tt_move_t& TT_Movement::Get_TT_Move()
{
    static tt_move_t ret_val ;
    for (auto i = _val_to_dir->rbegin(); i != _val_to_dir->rend(); i++)
    {
        if (Is_Lock_Free(i->second))
        {
            ret_val.delta_entropy = i->first ;
            ret_val.source_wc = i->second.source;
            ret_val.destination_wc = i->second.destination ;
            ret_val.sc_id = i->second.sc_id ;
            Lock(ret_val) ;
            return ret_val ;
        }
    }
}

void TT_Movement::Reset_Lock()
{
    _tt_lock_set->clear() ;
    while (!_tt_lock_queue->empty()) 
    {
        _tt_lock_queue->pop() ;
    }
    
}

uint8_t TT_Movement::Is_Lock_Free(tt_dir_t val)
{
    if (_tt_lock_set->find(make_tuple(val.source, val.destination, val.sc_id)) != _tt_lock_set->end()) {
        return 0 ;      // Has lock
    }
    return 1 ;
}

void TT_Movement::Lock(tt_move_t val)
{
    tt_dir_t tem ;
    if (_tt_lock_set->size() >= _lock_size) {
        tem = _tt_lock_queue->front() ;
        auto it = _tt_lock_set->find(make_tuple(tem.source, tem.destination, tem.sc_id)) ;
        if (it == _tt_lock_set->end()) {
            std::cerr<<"lock_set and lock_queue value mismatch."<<std::endl;
            exit(0) ;
        }
        _tt_lock_set->erase(it) ;
        _tt_lock_queue->pop() ;
    }

    tem.source = val.source_wc ;
    tem.destination = val.destination_wc ;
    tem.sc_id= val.sc_id ;
    _tt_lock_set->insert(make_tuple(tem.source, tem.destination, tem.sc_id)) ;
    _tt_lock_queue->push(tem) ;
    return ;
}

void TT_Movement::Insert_Val_To_Dir(tt_move_t val)
{
    tt_dir_t tem ;
    tem.source = val.source_wc;
    tem.destination = val.destination_wc ;
    tem.sc_id = val.sc_id ;
    _val_to_dir->insert({val.delta_entropy, tem}) ;
}