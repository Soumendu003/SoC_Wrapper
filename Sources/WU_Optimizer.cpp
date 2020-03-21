#include "WU_Optimizer.h"

solution_t* WU_Optimizer::Init_Optimizer()
{
    WU_Group *group = new WU_Group() ;

    _highest_tt = (uint64_t)0 ;

    for (uint64_t i = 0; i < _scanchains->size(); i++) 
    {
        group->Insert_SC(_scanchains->at(i)) ;
        if (_scanchains->at(i).test_time > _highest_tt) {
            _highest_tt = _scanchains->at(i).test_time ;
        }
    }

    for (uint64_t i = 0; i < _wc_count; i++)
    {
        group->Insert_WC(i) ;
    }

    cerr<<"Calling Main Scan_chain_Assignment func"<<endl ;

    solution_t *ret_val = Scan_Chains_Assignment(group) ;
    delete group ;

    return ret_val ;
}


bool solution_t::Equals(solution_t *sol)
{
    if (max_tt == sol->max_tt) {
        if (tsv_count == sol->tsv_count) {
            return true ;
        }
    }
    return false ;
}

void solution_t::Copy(solution_t *sol)
{
    max_tt = sol->max_tt ;
    tsv_count = sol->tsv_count ;
}

bool solution_t::Better(solution_t *sol)
{
    if (max_tt < sol->max_tt && tsv_count < sol->tsv_count) {
        return true ;
    } 
    return false ;
}

void tam_t::Insert_sc(scanchain_t sc_ele)
{
    sc_array->push_back(sc_ele) ;
    tt += sc_ele.test_time ;
}

solution_t* WU_Optimizer::Merge_Solution(solution_t *sol1, solution_t *sol2)
{
    solution_t *ret_sol = new solution_t ;

    if (sol2->max_tt > sol1->max_tt) {
        ret_sol->max_tt = sol2->max_tt ;
    } else{
        ret_sol->max_tt = sol1->max_tt ;
    }
    
    ret_sol->tsv_count = sol1->tsv_count + sol2->tsv_count ;

    delete sol1 ;
    delete sol2 ;

    return ret_sol ;
}

solution_t* WU_Optimizer::Scan_Chains_Assignment(WU_Group *parent_group) 
{
    if (parent_group->Get_WC_Size() <= 1){
        solution_t *ret_sol = new solution_t ;
        ret_sol->Copy(TAD(parent_group));
        return ret_sol ;
    }

    parent_group->Randomly_Partition_SubGroups() ;

    WU_Group *group1 = parent_group->Get_SubGroup1() ;
    WU_Group *group2 = parent_group->Get_SubGroup2() ;

    solution_t *curr_solution = Merge_Solution(TAD(group1), TAD(group2)) ;

    set<uint64_t> *Unlocked_Set = new set<uint64_t> ;
    solution_t *last_solution = new solution_t ;
    do {
        //last_solution = curr_solution ;
        last_solution->Copy(curr_solution) ;

        // Clears previously locked scanchains
        Unlocked_Set->clear() ;
        // Reinitializes unlocked set
        parent_group->Set_Insert_all_SC_IDs(Unlocked_Set) ;

        // Stores the state beefore entering KL algo
        parent_group->Store_state() ;

        while (Unlocked_Set->size() > 0)
        {
            vector<uint64_t> *sc_ids ;

            if (group1->Get_Max_tt() > group2->Get_Max_tt()) {
                sc_ids = group1->Get_ScanchainIDs() ;
            } else{
                sc_ids = group2->Get_ScanchainIDs() ;
            }

            double curr_gain ;
            uint64_t curr_sc ;
            uint8_t gain_flag = 0 ;
            for (auto it = sc_ids->begin(); it != sc_ids->end(); it++)
            {
                if (Unlocked_Set->find(*it) == Unlocked_Set->end()) {
                    continue ;
                }
                if (!gain_flag) {
                    //curr_gain = parent_group->Calculate_Gain(*it) ;
                    curr_gain = Calculate_gain(parent_group, *it) ;
                    curr_sc = *it ;
                    gain_flag = 1 ;
                } else {
                    double tem = Calculate_gain(parent_group, *it) ;
                    if (tem > curr_gain){
                        curr_gain = tem ;
                        curr_sc = *it ;
                    }
                }
            }
            delete sc_ids ;

            if (!gain_flag) {
                //cerr<<"Size of sc_ids = "<<to_string(sc_ids->size())<<endl ;
                break ;
            }
            
            parent_group->Interchange_SC_SubGroup(curr_sc) ;
            solution_t *tem_solution = Merge_Solution(TAD(group1), TAD(group2)) ;

            if (tem_solution->Better(curr_solution)) {
                //curr_solution = tem_solution ;
                curr_solution->Copy(tem_solution) ;
                parent_group->Store_state() ;
            }
            delete tem_solution ;

            if (Unlocked_Set->erase(curr_sc) != 1 && gain_flag) {
                cerr<<"Total scs = "<<to_string(parent_group->Get_ScanchainIDs()->size())<<endl ;
                cerr<<"Can't delete sc_id = "<<to_string(curr_sc)<<"  from the Unlocked Set."<<endl;
                cerr<<"Avail Scanchains in unlocked set = "<<endl ;
                for (auto it = Unlocked_Set->begin(); it != Unlocked_Set->end(); it++){
                    cerr<<" avail id = "<<to_string(*it)<<endl ;
                }
                exit(0) ;
            } 
        }

        parent_group->Restore_state() ;

        solution_t *sol_grp1 = Scan_Chains_Assignment(group1) ;
        solution_t *sol_grp2 = Scan_Chains_Assignment(group2) ;

        solution_t *tem_solution = Merge_Solution(sol_grp1, sol_grp2) ;

        //delete sol_grp1 ;
        //delete sol_grp2 ;

        curr_solution->Copy(tem_solution) ;

        delete tem_solution ;

    }while (!curr_solution->Equals(last_solution)) ;
    
    delete last_solution ;
    return curr_solution ;
}

uint64_t WU_Optimizer::Get_Standard_Chain(vector<scanchain_t> *Scanchains, double adj)
{
    double val_seek = Scanchains->at(0).test_time / adj ;

    double diff = (double)abs(Scanchains->at(0).test_time - val_seek) ;
    uint64_t ret_val = Scanchains->at(0).test_time ;

    for (uint64_t i = 0 ; i < Scanchains->size(); i++)
    {
        double tem_diff = (double)abs(Scanchains->at(i).test_time - val_seek) ;
        if (tem_diff < diff) {
            diff = tem_diff ;
            ret_val = Scanchains->at(i).test_time ;
        }
    }
    return ret_val ;
}

uint64_t WU_Optimizer::Get_Mean_Chain(vector<scanchain_t> *Scanchains)
{
    uint64_t tot_tt = 0 ;

    for (uint64_t i = 0 ; i < Scanchains->size(); i++)
    {
        tot_tt += Scanchains->at(i).test_time ;
    }

    double avg_tt = (double)tot_tt / Scanchains->size() ;

    double diff = abs(Scanchains->at(0).test_time - avg_tt) ;
    uint64_t ret_val = Scanchains->at(0).test_time ;

    for (uint64_t i = 0 ; i < Scanchains->size(); i++)
    {
        double tem_diff = (double)abs(Scanchains->at(i).test_time - avg_tt) ;
        if (tem_diff < diff) {
            diff = tem_diff ;
            ret_val = Scanchains->at(i).test_time ;
        }
    }
    return ret_val ;
}

vector<tad_chain_t>* WU_Optimizer::Get_Standard_greater_Chains(vector<scanchain_t> *Scanchains, uint64_t standard_chain_tt)
{
    vector<tad_chain_t> *ret_array = new vector<tad_chain_t> ;

    for (uint64_t i = 0 ; i < Scanchains->size(); i++)
    {
        if (Scanchains->at(i).test_time >= standard_chain_tt)
        {
            tad_chain_t tem ;
            tem.tt_key = Scanchains->at(i).test_time ;
            tem.sc_flag = (uint8_t)1 ;
            tem.sc = Scanchains->at(i);
            ret_array->push_back(tem) ;
        }
    }
    return ret_array ;
}

vector<tad_chain_t>* WU_Optimizer::Get_D1_Array(vector<scanchain_t> *Scanchains, uint64_t standard_chain_tt, uint64_t mean_chain_tt)
{
    vector<tad_chain_t> *ret_array = new vector<tad_chain_t> ;

    for (uint64_t i = 0 ; i < Scanchains->size(); i++)
    {
        if (Scanchains->at(i).test_time >= standard_chain_tt)
        {
            tad_chain_t tem ;
            tem.tt_key = (long int)Scanchains->at(i).test_time - mean_chain_tt ;
            tem.sc_flag = (uint8_t)0 ;
            ret_array->push_back(tem) ;
        }
        else{
            tad_chain_t tem ;
            tem.tt_key = Scanchains->at(i).test_time ;
            tem.sc_flag = (uint8_t)1 ;
            tem.sc = Scanchains->at(i) ;
            ret_array->push_back(tem) ;
        }
    }
    return ret_array ;
}

vector<tam_t*>* WU_Optimizer::Get_WC_array(WU_Group *group)
{
    const vector<uint64_t> *WC_id_array = group->Get_WC_id_array() ;

    vector<tam_t*> *WC_array = new vector<tam_t*> ;
    for (uint64_t i = 0; i < WC_id_array->size(); i++)
    {
        tam_t *tem = new tam_t(WC_id_array->at(i)) ;
        WC_array->push_back(tem) ;
    }

    return WC_array ;
}

void WU_Optimizer::First_Assignment(vector<tad_chain_t> *std_greater_chains, vector<tam_t*> *WC_array)
{
    uint64_t wc_i = 0 ;
    for (uint64_t i = 0; i < std_greater_chains->size(); i++)
    {
        if (wc_i >= WC_array->size()){
            wc_i = 0 ;
        }
        WC_array->at(wc_i)->Insert_sc(std_greater_chains->at(i).sc) ;
        WC_array->at(wc_i)->Sum_TAM += std_greater_chains->at(i).tt_key ;
        WC_array->at(wc_i)->No_TAM += 1 ;
        wc_i += 1 ;
    }
}



void WU_Optimizer::Second_Assignment(vector<tad_chain_t>* D, vector<tam_t*>* WC_array)
{
    for (uint64_t i = 0; i < D->size(); i++)
    {
        if (D->at(i).sc_flag) {
            auto it = min_element(WC_array->begin(), WC_array->end(), tam_t::Compare_Sum_TAM) ;

            assert (it != WC_array->end()) ;

            tam_t *ele = *it ;
            ele->Insert_sc(D->at(i).sc) ;
            ele->Sum_TAM += D->at(i).tt_key ;

        } else {
            if (D->at(i).tt_key >= 0){
                auto it = min_element(WC_array->begin(), WC_array->end(), tam_t::Compare_Sum_TAM_positive_No_TAM) ;

                assert(it != WC_array->end()) ;

                tam_t *ele = *it ;
                if (ele->No_TAM <= 0) {
                    continue ;
                }
            
                ele->Sum_TAM += D->at(i).tt_key ;
                ele->No_TAM -= 1 ;

            } else{
                auto it = max_element(WC_array->begin(), WC_array->end(), tam_t::Compare_Sum_TAM_positive_No_TAM) ;

                assert(it != WC_array->end()) ;

                tam_t *ele = *it ;
                if (ele->No_TAM <= 0) {
                    continue ;
                }
            
                ele->Sum_TAM += D->at(i).tt_key ;
                ele->No_TAM -= 1 ;

            }

        }
    }
}

solution_t* WU_Optimizer::TAD(WU_Group *group)
{
    assert (group->Get_WC_Size() > 0) ;

    vector<scanchain_t> *Scanchains = 0 ;
    vector<tam_t*> *WC_array = 0 ;

    if (group->Get_WC_Size() == 1){
        Scanchains = group->Get_Scanchains() ;
        WC_array = Get_WC_array(group) ;
        for (uint64_t i = 0; i < Scanchains->size(); i++)
        {
            WC_array->at(0)->Insert_sc(Scanchains->at(i)) ;
        }
        Calculate_TSV(WC_array) ;

        solution_t *ret_val = Get_TAD_result(WC_array) ;

        group->Set_TAD_result(ret_val->max_tt, ret_val->tsv_count) ;

        delete Scanchains ;
        delete WC_array ;

        return ret_val ;
    }

    double adj = group->Get_adj() ;
    
    Scanchains = group->Get_Scanchains() ;

    // sorts Scanchains in descending order of their test time.
    sort(Scanchains->begin(), Scanchains->end(), Wrapper_Optimizer::SC_tt_compare) ;

    uint64_t standard_chain_tt = Get_Standard_Chain(Scanchains, adj) ;
    uint64_t mean_chain_tt = Get_Mean_Chain(Scanchains) ;

    vector<tad_chain_t> *std_greater_chains = Get_Standard_greater_Chains(Scanchains, standard_chain_tt) ;

    WC_array = Get_WC_array(group) ;

    First_Assignment(std_greater_chains, WC_array) ;

    vector<tad_chain_t> *D1 = Get_D1_Array(Scanchains, standard_chain_tt, mean_chain_tt) ;

    // sorts D1 in descending order of their tt_key value ;
    sort(D1->begin(), D1->end(), WU_Optimizer::TAD_chain_compare) ;

    Second_Assignment(D1, WC_array) ;

    Calculate_TSV(WC_array) ;

    solution_t *ret_val = Get_TAD_result(WC_array) ;

    group->Set_TAD_result(ret_val->max_tt, ret_val->tsv_count) ;

    delete D1 ;
    for (uint64_t i = 0 ; i < WC_array->size(); i++)
    {
        delete WC_array->at(i) ;
    }
    delete WC_array ;
    delete Scanchains ;

    return ret_val ;
}

solution_t* WU_Optimizer::Get_TAD_result(vector<tam_t*>* WC_array)
{
    solution_t *ret_sol = new solution_t ;

    uint8_t tt_flag = 0 ;

    for (uint64_t i = 0; i < WC_array->size(); i++)
    {
        ret_sol->tsv_count += WC_array->at(i)->tsv_count ;
        
        if (!tt_flag) {
            ret_sol->max_tt = WC_array->at(i)->tt ;
            tt_flag = 1 ;
        } else if (ret_sol->max_tt < WC_array->at(i)->tt) {
            ret_sol->max_tt = WC_array->at(i)->tt ;
        }
    }

    return ret_sol ;
}

void WU_Optimizer::Calculate_TSV(vector<tam_t*>* WC_array)
{
    for (uint64_t i = 0; i < WC_array->size(); i++)
    {
        Calculate_TSV_tam(*WC_array->at(i)) ;
    }
}

void WU_Optimizer::Calculate_TSV_tam(tam_t &tam_chain)
{
    tam_chain.tsv_count = 0 ;

    vector<tsv_node_t> *node_array = new vector<tsv_node_t> ;
    set<uint64_t> *node_id_set = new set<uint64_t> ;
    vector<tsv_edge_t> *edge_array = new vector<tsv_edge_t> ;

    uint64_t id = 0 ;
    for (uint64_t i = 0; i < tam_chain.sc_array->size(); i++)
    {
        tsv_node_t in_node;
        in_node.node_id = id ;
        in_node.node_layer = tam_chain.sc_array->at(i).in_layer ;
        node_array->push_back(in_node) ;
        node_id_set->insert(id) ;
        id += 1 ;

        tsv_node_t out_node;
        out_node.node_id = id ;
        out_node.node_layer = tam_chain.sc_array->at(i).out_layer ;
        node_array->push_back(out_node) ;
        node_id_set->insert(id) ;
        id += 1 ;
    }

    for (uint64_t i = 0; i < node_array->size(); i++)
    {
        for (uint64_t j = 0; j < node_array->size(); j++)
        {
            if (i != j ){
                tsv_edge_t edge ;
                edge.node_u = node_array->at(i).node_id ;
                edge.node_v = node_array->at(j).node_id ;
                if (node_array->at(i).node_layer >= node_array->at(j).node_layer){
                    edge.weight = (node_array->at(i).node_layer - node_array->at(j).node_layer) ;
                } else {
                    edge.weight = (node_array->at(j).node_layer - node_array->at(i).node_layer) ;
                }
                edge_array->push_back(edge) ;
            }
        }
    }

    sort(edge_array->begin(), edge_array->end(), tsv_edge_t::Compare_Desc) ;

    while (node_id_set->size() > 0)
    {
        tsv_edge_t edge = edge_array->back() ;

        if (node_id_set->find(edge.node_u) != node_id_set->end()) {
            if (node_id_set->find(edge.node_v) != node_id_set->end()) {
                tam_chain.tsv_count += edge.weight ;
                node_id_set->erase(edge.node_u) ;
                node_id_set->erase(edge.node_v) ;
            }
        }
        edge_array->pop_back() ;
    }

    delete node_id_set ;
    delete node_array ;
    delete edge_array ;
}

double WU_Optimizer::Calculate_gain(WU_Group *group, uint64_t sc_id)
{
    uint64_t curr_tt = group->Get_Max_tt() ;
    uint64_t curr_tsv = group->Get_TSV() ;

    group->Interchange_SC_SubGroup(sc_id) ;

    solution_t* sol = TAD(group) ;

    uint64_t new_tt = sol->max_tt ;
    uint64_t new_tsv = sol->tsv_count ;

    uint64_t beta = max(new_tsv, _highest_tt) + 1000 ;
    long int alpha = 100 - (long int)new_tsv ;

    group->Interchange_SC_SubGroup(sc_id) ;
    group->Set_TAD_result(curr_tt, curr_tsv) ;
    delete sol ;

    if (alpha >= 0) {
        return beta * new_tt + alpha ;
    } else {
        return beta * alpha - new_tt ;
    }
}