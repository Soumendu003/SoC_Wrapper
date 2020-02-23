#ifndef WRAPPER_CHAIN_H
#define WRAPPER_CHAIN_H

#include"SoC_Lexer.h"
#include<set>

class Wrapper_Chain
{
    public:
        Wrapper_Chain()
        {
            _sc_set = new set<scanchain_t*> ;
        }

        void Insert_SC (const scanchain_t *sc_ptr) {}
        void Delete_SC(const scanchain_t *sc_ptr) {}

    private:
        

    private:
        set<scanchain_t*> *_sc_set ;
};

#endif