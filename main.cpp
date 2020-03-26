#include "Optimizer_Comparator.h"
#include<stdlib.h>
#include<time.h>
#include<map>
#include<iostream>

using namespace std ;

int main()
{
    //Initializes rand seed
    srand(time(NULL)) ;

    //SoC_Lexer sc_lex("Benchmarks/d281.soc") ;
    
    Optimizer_Comparator *op_comp = new Optimizer_Comparator() ;

    op_comp->Init_Comparator("Compare_Soc.csv") ;

    op_comp->Compare("Benchmarks/d281.soc") ;
    op_comp->Compare("Benchmarks/a586710.soc") ;
    op_comp->Compare("Benchmarks/d695.soc") ;
    op_comp->Compare("Benchmarks/f2126.soc") ;
    op_comp->Compare("Benchmarks/g1023.soc") ;
    op_comp->Compare("Benchmarks/h953.soc") ;
    op_comp->Compare("Benchmarks/p22810.soc") ;
    op_comp->Compare("Benchmarks/p34392.soc") ;
    op_comp->Compare("Benchmarks/q12710.soc") ;
    op_comp->Compare("Benchmarks/t512505.soc") ;
    op_comp->Compare("Benchmarks/u226.soc") ;
    delete op_comp ;
    return 0;
}
