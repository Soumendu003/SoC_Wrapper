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
        
        cout<<"\n\n------------------------------------------------------------------------------\n"<<endl ;
        cout<<"---------------------- OUR RESULT--------------------------------------------------"<<endl ;
        cout<<"------------------------------------------------------------------------------\n"<<endl ;

        cout<<"Initial Partition time-taken = "<<best_result->initial_partition_time<<" ms."<<endl;
        cout<<"Initial Partition Max TT = "<<to_string(best_result->init_max_tt)<<endl;
        cout<<"Initial Partition TSV count= "<<to_string(best_result->init_tsv)<<"\n\n"<<endl ;

        cout<<"Simulated Annelation time taken = "<<best_result->simulated_annelation_time<<" ms."<<endl;
        cout<<"Simulated Annelation Max TT = "<<to_string(best_result->final_max_tt)<<endl ;
        cout<<"Simulated Annelation TSV count = "<<to_string(best_result->final_tsv)<<endl ;


        cout<<"\n\n------------------------------------------------------------------------------\n"<<endl ;
        cout<<"---------------------- Wu et al RESULT--------------------------------------------------"<<endl ;
        cout<<"------------------------------------------------------------------------------\n"<<endl ;

        WU_Optimizer *wu_opt = new WU_Optimizer(_curr_scanchain, wc_count) ;
        auto start = chrono::high_resolution_clock::now();
        solution_t *wu_sol = wu_opt->Init_Optimizer() ;
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

        std::cout<<"Time taken = "+to_string(duration.count())<<" ms."<<std::endl ;
        std::cout<<"\nMax_TT = "<<std::to_string(wu_sol->max_tt)<<std::endl ;
        std::cout<<"\nTSV count = "<<std::to_string(wu_sol->tsv_count)<<std::endl ;

        Write_CSV_result(best_result, wu_sol, to_string(duration.count()), file_name, wc_count) ;

        delete wu_sol ;
        delete wu_opt ;
    }
}

void Optimizer_Comparator::Write_CSV_result(two_phase_result_t * two_phase_result, solution_t *wu_sol, string wu_time, string bench_file, uint64_t wc_count)
{
    try
    {
        // Writes Bench-name
        *_csv << bench_file.substr(string("Benchmarks/").length(), string::npos) ;

        // Writes Current TAM count
        *_csv << to_string(wc_count) ;

        // Writes Init time
        *_csv << two_phase_result->initial_partition_time ;

        // Writes Init Test Time
        *_csv << to_string(two_phase_result->init_max_tt) ;

        // Writes Init TSV
        *_csv << to_string(two_phase_result->init_tsv) ;

        // Writes SA time
        *_csv << two_phase_result->simulated_annelation_time ;

        // Writes SA Test Time
        *_csv << to_string(two_phase_result->final_max_tt) ;

        // Writes SA TSV
        *_csv << to_string(two_phase_result->final_tsv) ;

        // Writes WU time
        *_csv << wu_time ;

        // Writes WU Test Time
        *_csv << to_string(wu_sol->max_tt) ;

        // Writes WU TSV
        *_csv << to_string(wu_sol->tsv_count) ;

        // Writes TT Reduction
        double tt_red = (((double)wu_sol->max_tt - two_phase_result->final_max_tt) / wu_sol->max_tt) * 100 ;
        *_csv << to_string(tt_red) + " %" ;

        // Writes TSV Reduction
        double tsv_red = (((double)wu_sol->tsv_count - two_phase_result->final_tsv) / wu_sol->tsv_count) * 100 ;
        *_csv << to_string(tsv_red) + " %" ;

        // Ends Row
        *_csv << endrow ;
    }
    catch (const std::exception &ex)
    {
        std::cout << "Exception was thrown: " << ex.what() << std::endl;
    }
}

void Optimizer_Comparator::Init_Comparator(const char* output_file)
{
    try
    {
        _csv = new csvfile(output_file, std::ios::out) ;

        // Writing the header 
        *_csv << "Benchmark file" << "TAM count" << "Init time(ms)" << "Init TT" << "Init TSV" << "SA time(ms)" << "SA TT" << "SA TSV" << "WU time(ms)" << "WU TT" << "WU TSV" << "TT Reduction" << "TSV Reduction" << endrow ;
    }
    catch (const std::exception &ex)
    {
        std::cout << "Exception was thrown: " << ex.what() << std::endl;
    }

}