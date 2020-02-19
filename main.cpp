
#include "SoC_Lexer.h"

int main()
{
    SoC_Lexer sc_lex("Benchmarks/a586710.soc") ;
    sc_lex.Get_SoC_scanchains() ; 
    
    return 0;
}