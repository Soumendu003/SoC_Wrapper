#include "WU_Optimizer.h"

void WU_Optimizer::Init_Optimizer()
{
    WU_Group *group = new WU_Group() ;

    for (uint64_t i = 0; i < _scanchains->size(); i++){
        group->Insert_SC(_scanchains->at(i)) ;
    }

    for (uint64_t i = 0; i < _wc_count; i++)
    {
        group->Insert_WC(i) ;
    }

    Scan_Chains_Assignment(group) ;

    delete group ;
}


bool solution_t::Equals(solution_t *sol)
{
    if (max_tt == sol->max_tt) {
        if (tsv_count == sol->tsv_count) {
            return true ;
        }
    }
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
                    curr_gain = parent_group->Calculate_Gain(*it) ;
                    curr_sc = *it ;
                    gain_flag = 1 ;
                } else {
                    double tem = parent_group->Calculate_Gain(*it) ;
                    if (tem > curr_gain){
                        curr_gain = tem ;
                        curr_sc = *it ;
                    }
                }
            }
            delete sc_ids ;
            
            parent_group->Interchange_SC_SubGroup(curr_sc) ;
            solution_t *tem_solution = Merge_Solution(TAD(group1), TAD(group2)) ;

            if (tem_solution->Better(curr_solution)) {
                //curr_solution = tem_solution ;
                curr_solution->Copy(tem_solution) ;
                parent_group->Store_state() ;
            }
            delete tem_solution ;

            if (Unlocked_Set->erase(curr_sc) != 1) {
                cerr<<"Can't delete sc_id = "<<to_string(curr_sc)<<"  from the Unlocked Set."<<endl;
                exit(0) ;
            }
        }

        parent_group->Restore_state() ;

        solution_t *sol_grp1 = Scan_Chains_Assignment(group1) ;
        solution_t *sol_grp2 = Scan_Chains_Assignment(group2) ;

        solution_t *tem_solution = Merge_Solution(sol_grp1, sol_grp2) ;

        delete sol_grp1 ;
        delete sol_grp2 ;

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

    for (auto i = 0 ; i < Scanchains->size(); i++)
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

    for (auto i = 0 ; i < Scanchains->size(); i++)
    {
        tot_tt += Scanchains->at(i).test_time ;
    }

    double avg_tt = (double)tot_tt / Scanchains->size() ;

    double diff = abs(Scanchains->at(0).test_time - avg_tt) ;
    uint64_t ret_val = Scanchains->at(0).test_time ;

    for (auto i = 0 ; i < Scanchains->size(); i++)
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

    for (auto i = 0 ; i < Scanchains->size(); i++)
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

    for (auto i = 0 ; i < Scanchains->size(); i++)
    {
        if (Scanchains->at(i).test_time >= standard_chain_tt)
        {
            uint64_t diff_val ;
            if (Scanchains->at(i).test_time > mean_chain_tt) {
                diff_val = Scanchains->at(i).test_time - mean_chain_tt ;
            } else {
                diff_val = mean_chain_tt - Scanchains->at(i).test_time ;
            }
            tad_chain_t tem ;
            tem.tt_key = diff_val ;
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

vector<tam_t>* WU_Optimizer::Get_WC_array(WU_Group *group)
{
    const vector<uint64_t> *WC_id_array = group->Get_WC_id_array() ;

    vector<tam_t> *WC_array = new vector<tam_t> ;
    for (auto i = 0; i < WC_id_array->size(); i++)
    {
        tam_t tem = tam_t(WC_id_array->at(i)) ;
        WC_array->push_back(tem) ;
    }

    return WC_array ;
}

void WU_Optimizer::First_Assignment(vector<tad_chain_t> *std_greater_chains, vector<tam_t> *WC_array)
{
    uint64_t wc_i = 0 ;
    for (auto i = 0; i < std_greater_chains->size(); i++)
    {
        if (wc_i >= WC_array->size()){
            wc_i = 0 ;
        }
        WC_array->at(wc_i).Insert_sc(std_greater_chains->at(i).sc) ;
        wc_i += 1 ;
    }
}

solution_t* WU_Optimizer::TAD(WU_Group *group)
{
    if (group->Get_WC_Size() <= 1){
        //  Simple tt calculation and Tsv count
    }

    double adj = group->Get_adj() ;
    
    vector<scanchain_t> *Scanchains = group->Get_Scanchains() ;

    // sorts Scanchains in descending order of their test time.
    sort(Scanchains->begin(), Scanchains->end(), Wrapper_Optimizer::SC_tt_compare) ;

    uint64_t standard_chain_tt = Get_Standard_Chain(Scanchains, adj) ;
    uint64_t mean_chain_tt = Get_Mean_Chain(Scanchains) ;

    vector<tad_chain_t> *std_greater_chains = Get_Standard_greater_Chains(Scanchains, standard_chain_tt) ;

    vector<tam_t> *WC_array = Get_WC_array(group) ;

    First_Assignment(std_greater_chains, WC_array) ;

    vector<tad_chain_t> *D1 = Get_D1_Array(Scanchains, standard_chain_tt, mean_chain_tt) ;

    // sorts D1 in descending order of their tt_key value ;
    sort(D1->begin(), D1->end(), WU_Optimizer::TAD_chain_compare) ;

    Second_Assignment(D1, WC_array) ;

    Calculate_TSV(WC_array) ;

    solution_t *ret_val = Get_TAD_result(WC_array) ;

    group->Set_TAD_result(ret_val->max_tt, ret_val->tsv_count) ;

    delete D1 ;
    delete WC_array ;
    delete Scanchains ;

    return ret_val ;
}

solution_t* Get_TAD_result(vector<tam_t>* WC_array)
{
    solution_t *ret_sol = new solution_t ;

    uint8_t tt_flag = 0 ;

    for (auto i = 0; i < WC_array->size(); i++)
    {
        ret_sol->tsv_count += WC_array->at(i).tsv_count ;
        
        if (!tt_flag) {
            ret_sol->max_tt = WC_array->at(i).tt ;
            tt_flag = 1 ;
        } else if (ret_sol->max_tt < WC_array->at(i).tt) {
            ret_sol->max_tt = WC_array->at(i).tt ;
        }
    }

    return ret_sol ;
}