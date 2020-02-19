#include "Wrapper_Optimizer.h"

void Wrapper_Optimizer::Simulated_Annelation()
{
    double T = 60000 ;    // Initial Temperature
    uint16_t step = _sc_array->size() ;     //No os intermediate step per T(temperature) value

    while (T > 1)
    {
        for (uint16_t i = 0; i < step; i++)
        {
            Minimize_TSV_Phase() ;
        }

        for(uint16_t i = 0; i < step; i++)
        {
            Minimize_TT_Phase() ;
        }

        T = 0.9*T ;     // reduces the T value 10%
    }
    
}