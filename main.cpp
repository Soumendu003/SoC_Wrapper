#include "SoC_Lexer.h"
#include "Wrapper_Optimizer.h"
#include<stdlib.h>
#include<time.h>
#include<map>

using namespace std ;

int main()
{
    //Initializes rand seed
    srand(time(NULL)) ;

    SoC_Lexer sc_lex("Benchmarks/a586710.soc") ;
    //sc_lex.Get_SoC_scanchains() ; 
    Wrapper_Optimizer wo = Wrapper_Optimizer(sc_lex.Get_RandLayer_scanchains(6)) ;
    wo.Two_Phase_Optimizer(6, 4) ;
    return 0;
}