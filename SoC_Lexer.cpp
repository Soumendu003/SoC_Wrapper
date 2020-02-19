#include "SoC_Lexer.h"


vector<scanchain_t>* SoC_Lexer::Get_SoC_scanchains()
{
    vector<scanchain_t> *scanchains =  new vector<scanchain_t> ;
    scanchain_t tem_sc ;
    while (Get_scanchain())
    {
        tem_sc.test_time = _sc_tt ;
        cout << _sc_tt << "\t" ;
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
    while (lexeme) {
       if (Strcmpi(lexeme, "Scanchains") == 0) {
            _avail_sc = (uint64_t)atoi(_tok->get_token()) ;      // gets no of scanchains available
            
            // gets  the symbol ':'
            _tok->get_token() ;

            return Get_scanchain() ;
       }
       lexeme = _tok->get_token() ;
       //cout<<"lexeme got = "<<lexeme<<endl ; 
    }
    
    return 0 ;
}