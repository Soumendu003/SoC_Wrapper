#include "Optimizer_Comparator.h"
#include<stdlib.h>
#include<time.h>
#include<map>

using namespace std ;

int main()
{
    //Initializes rand seed
    srand(time(NULL)) ;

    //SoC_Lexer sc_lex("Benchmarks/d281.soc") ;
    
    Optimizer_Comparator *op_comp = new Optimizer_Comparator() ;
    op_comp->Compare("Benchmarks/d281.soc") ;
    delete op_comp ;
    return 0;
}