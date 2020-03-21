#ifndef WU_OPTIMIZER_H
#define WU_OPTIMIZER_H

#include "WU_Group.h"
#include "Wrapper_Optimizer.h"   // SC_tt_compare
#include<set>
#include<vector>
#include<map>
#include<algorithm>

using namespace std ;

typedef struct{
    long int tt_key ;
    uint8_t sc_flag ;
    scanchain_t sc ;
} tad_chain_t ;

typedef struct{
    uint64_t node_id ;
    uint8_t node_layer ;
} tsv_node_t ;

typedef struct tsv_edge_t{
    uint64_t node_u ;
    uint64_t node_v ;
    uint8_t weight ;

    static bool Compare_Desc(tsv_edge_t edge1, tsv_edge_t edge2)
    {
        return edge1.weight > edge2.weight ;
    }
    
} tsv_edge_t ;

typedef struct tam_t {
    uint64_t wc_id ;
    uint64_t tsv_count ;
    uint64_t tt ;
    long int No_TAM ;
    long int Sum_TAM ;
    vector<scanchain_t> *sc_array ;

    tam_t(uint64_t id): wc_id(id), tsv_count((uint64_t)0), tt((uint64_t)0), No_TAM((long int)0), Sum_TAM((long int)0)
    {
        sc_array = new vector<scanchain_t> ;
    }

    ~tam_t()
    {
        delete sc_array ;
    }

    void Insert_sc(scanchain_t sc_ele) ;
    static bool Compare_Sum_TAM(tam_t *var1, tam_t *var2)
    {
        return var1->Sum_TAM < var2->Sum_TAM ;
    }
    static bool Compare_Sum_TAM_positive_No_TAM(tam_t *var1, tam_t *var2)
    {
        if (var1->No_TAM > 0 && var2->No_TAM <= 0) {
            return true ;
        } else if (var1->No_TAM <= 0 && var2->No_TAM > 0) {
            return false ;
        } else {
            return var1->Sum_TAM < var2->Sum_TAM ;
        }
    }

} tam_t ;

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
        : _scanchains(scanchains), _wc_count(wc_count), _highest_tt((uint64_t)0) {}

        ~WU_Optimizer() {}

        solution_t* Init_Optimizer() ;

    private:

        solution_t *Scan_Chains_Assignment(WU_Group *parent_group) ;
        solution_t* TAD(WU_Group *group) ;
        solution_t* Merge_Solution(solution_t *sol1, solution_t *sol2) ;
        void First_Assignment(vector<tad_chain_t>* std_greater_chains, vector<tam_t*>* WC_array) ;
        void Second_Assignment(vector<tad_chain_t>* D, vector<tam_t*>* WC_array) ;
        void Calculate_TSV(vector<tam_t*>* WC_array) ;
        double Calculate_gain(WU_Group *group, uint64_t sc_id) ;

    private:
        uint64_t Get_Standard_Chain(vector<scanchain_t> *Scanchains, double adj) ;
        uint64_t Get_Mean_Chain(vector<scanchain_t> *Scanchains) ;
        vector<tad_chain_t>* Get_Standard_greater_Chains(vector<scanchain_t> *Scanchains, uint64_t standard_chain_tt) ;
        vector<tad_chain_t>* Get_D1_Array(vector<scanchain_t> *Scanchains, uint64_t standard_chain_tt, uint64_t mean_chain_tt) ;
        vector<tam_t*>* Get_WC_array(WU_Group *group) ;
        solution_t* Get_TAD_result(vector<tam_t*>* WC_array) ;
        void Calculate_TSV_tam(tam_t &tam_chain) ;

    public:
        static bool TAD_chain_compare(tad_chain_t val1, tad_chain_t val2)
        {   
            return (long int)abs(val1.tt_key) > (long int)abs(val2.tt_key) ;
        }

    private:
        const vector<scanchain_t> *_scanchains ;
        uint64_t _wc_count ;
        uint64_t _highest_tt ;
};


#endif