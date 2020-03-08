#include "SoC_Lexer.h"

vector<scanchain_t>* SoC_Lexer::Get_SoC_scanchains()
{
    vector<scanchain_t> *scanchains =  new vector<scanchain_t> ;
    scanchain_t tem_sc ;
    while (Get_scanchain())
    {
        tem_sc.test_time = _sc_tt ;
        tem_sc.out_layer = 0 ;
        tem_sc.in_layer = 0 ;
        tem_sc.wrapper_chain = 0 ;
        scanchains->push_back(tem_sc) ;
    }
    return scanchains ;
}

uint8_t SoC_Lexer::Get_scanchain()
{
    char *lexeme = _tok->get_token() ;
    if (_avail_sc > 0) {
        _avail_sc -= 1 ;
        _sc_tt = (uint64_t)atoi(lexeme) ;
        return 1;
    }
    while (lexeme) 
    {
       if (Strcmpi(lexeme, "Scanchains") == 0) {
            _avail_sc = (uint64_t)atoi(_tok->get_token()) ;      // gets no of scanchains available
            
            // gets  the symbol ':'
            _tok->get_token() ;

            return Get_scanchain() ;
       }
       lexeme = _tok->get_token() ;
    }
    
    return 0 ;
}


vector<scanchain_t>* SoC_Lexer::Get_RandLayer_scanchains(uint8_t max_layer)
{
    vector<scanchain_t> *scanchains = Get_SoC_scanchains() ;

    // Initializes the random seed
    //srand(time(NULL)) ;

    for (uint64_t i = 0; i < scanchains->size(); i++)
    {
        scanchains->at(i).in_layer = rand() % max_layer ;
        scanchains->at(i).out_layer = rand() % max_layer ;
        scanchains->at(i).sc_id = i ;
        //cout<<"Sc_tt = "<<scanchains->at(i).test_time<<"\tin_layer = "<<to_string(scanchains->at(i).in_layer)<<"\tout_layer = "<<to_string(scanchains->at(i).out_layer)<<"\twrapper chain = "<<to_string(scanchains->at(i).wrapper_chain)<<endl ;
    }

    return scanchains ;
}