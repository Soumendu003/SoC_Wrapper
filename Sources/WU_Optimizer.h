#ifndef WU_OPTIMIZER_H
#define WU_OPTIMIZER_H

#include "WU_Group.h"
#include<set>
#include<vector>
#include<map>

using namespace std ;

typedef struct solution_t {
    uint64_t max_tt ;
    uint64_t tsv_count ;

    solution_t() : max_tt((uint64_t)0), tsv_count((uint64_t)0) {}

    bool Equals(solution_t *sol) ;
    bool Better(solution_t *sol) ;
    void Copy(solution_t *sol) ;
} solution_t ;

class WU_Optimizer
{
    public:
        explicit WU_Optimizer(vector<scanchain_t> *scanchains, uint64_t wc_count)
        : _scanchains(scanchains), _wc_count(wc_count) {}

        ~WU_Optimizer() {}

        void Init_Optimizer() ;

    private:

        solution_t *Scan_Chains_Assignment(WU_Group *parent_group) ;
        solution_t* TAD(WU_Group *group) ;
        solution_t* Merge_Solution(solution_t *sol1, solution_t *sol2) ;

    private:
        vector<scanchain_t> *_scanchains ;
        uint64_t _wc_count ;
        uint64_t _beta ;
        uint64_t _afpha ;
};


#endif