#include "Optimizer_Comparator.h"

void Optimizer_Comparator::Compare(char* file_name)
{
    _curr_benchmark = file_name ;

    delete _soc_lexer ;

    _soc_lexer = new SoC_Lexer(_curr_benchmark) ;

    _soc_lexer->Init_SoC_Lexer() ;
    uint8_t max_layer = 6 ;

    _curr_scanchain = _soc_lexer->Get_RandLayer_scanchains(max_layer) ;

    delete _wrapper_optimizer ;

    _wrapper_optimizer = new Wrapper_Optimizer(_curr_scanchain) ;

    cout<<"\n\n\n------------------------------------------------------------------------------"<<endl ;
    cout<<"------------------------------------------------------------------------------"<<endl ;
    cout<<"------------------------------------------------------------------------------"<<endl ;
    cout<<"\n Results for benchmark = "<<_curr_benchmark<<endl<<endl;

    for (uint64_t wc_count = 5; wc_count < 9; wc_count++)
    {
        two_phase_result_t *best_result = 0;
        for (auto i = 0; i < 100; i++)
        {
            two_phase_result_t *tem = _wrapper_optimizer->Two_Phase_Optimizer(max_layer, wc_count) ;
            
            if (best_result == 0){
                best_result = new two_phase_result_t ;
                Wrapper_Optimizer::copy_result(best_result, tem) ;
            } else if (Wrapper_Optimizer::greater_than_result(best_result, tem)) {
                Wrapper_Optimizer::copy_result(best_result, tem) ;
            }
        }
        
        cout<<"\n------------------------------------------------------------------------------\n"<<endl ;
        
        cout<<"\n Results with wrapper chain count = "<<to_string(wc_count)<<endl<<endl ;

        cout<<"Initial Partition time-taken = "<<best_result->initial_partition_time<<" ms."<<endl;
        cout<<"Initial Partition Max TT = "<<to_string(best_result->init_max_tt)<<endl;
        cout<<"Initial Partition TSV count= "<<to_string(best_result->init_tsv)<<"\n\n"<<endl ;

        cout<<"Simulated Annelation time taken = "<<best_result->simulated_annelation_time<<" ms."<<endl;
        cout<<"Simulated Annelation Max TT = "<<to_string(best_result->final_max_tt)<<endl ;
        cout<<"Simulated Annelation TSV count = "<<to_string(best_result->final_tsv)<<endl ;

    }
}