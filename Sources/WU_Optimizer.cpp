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