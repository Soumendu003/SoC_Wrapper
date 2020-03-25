#include "SoC_Lexer.h"

void SoC_Lexer::Init_SoC_Lexer()
{
    //assert (_scanchains == 0) ;

    Get_SoC_Wrapper_Elements() ;
}

void SoC_Lexer::Get_SoC_Wrapper_Elements()
{
    _scanchains = new vector<scanchain_t> ;
    _io_cells = new vector<io_cell_t> ;

    char* lexeme = _tok->get_token() ;

    while (lexeme)
    {
        if (Strcmpi(lexeme, "Scanchains") == 0) {
            Get_scanchains() ;
       } else if (Strcmpi(lexeme, "Inputs") == 0) {
           Get_input_cell() ;
       } else if (Strcmpi(lexeme, "Outputs") == 0) {
           Get_output_cell() ;
       }
       lexeme = _tok->get_token() ;
    }
    
}

void SoC_Lexer::Get_output_cell()
{
    _tok->get_token() ;  // Gets output port  size

    io_cell_t tem ;
    tem.io_id = _io_cells->size() ;
    tem.type = wrapper_element_type::OUT_CELL ;
    _io_cells->push_back(tem) ;
}

void SoC_Lexer::Get_input_cell()
{
    _tok->get_token() ;  // Gets input port  size

    io_cell_t tem ;
    tem.io_id = _io_cells->size() ;
    tem.type = wrapper_element_type::IN_CELL ;
    _io_cells->push_back(tem) ;
}

void SoC_Lexer::Get_scanchains()
{
    _avail_sc = (uint64_t)atoi(_tok->get_token()) ;      // gets no of scanchains available
       
    // gets  the symbol ':'
    _tok->get_token() ;

    char *lexeme = 0 ;
    while (_avail_sc > 0)
    {
        lexeme = _tok->get_token() ;
        _avail_sc -= 1 ;

        _sc_tt = (uint64_t)atoi(lexeme) ;

        scanchain_t tem ;
        tem.sc_id = _scanchains->size() ;
        tem.test_time = _sc_tt ;
        _scanchains->push_back(tem) ;
    }
}


vector<scanchain_t>* SoC_Lexer::Get_RandLayer_scanchains(uint8_t max_layer)
{
    //assert (_scanchains != 0) ;
    
    for (uint64_t i = 0; i < _scanchains->size(); i++)
    {
        _scanchains->at(i).in_layer = rand() % max_layer ;
        _scanchains->at(i).out_layer = rand() % max_layer ;
        _scanchains->at(i).sc_id = i ;
        //cout<<"Sc_tt = "<<scanchains->at(i).test_time<<"\tin_layer = "<<to_string(scanchains->at(i).in_layer)<<"\tout_layer = "<<to_string(scanchains->at(i).out_layer)<<"\twrapper chain = "<<to_string(scanchains->at(i).wrapper_chain)<<endl ;
    }

    return _scanchains ;
}

vector<io_cell_t>* SoC_Lexer::Get_RandLayer_iocells(uint8_t max_layer)
{
    for (uint64_t i = 0; i < _io_cells->size(); i++)
    {
        _io_cells->at(i).layer = rand() % max_layer ;
        _io_cells->at(i).io_id = i ;
    }

    return _io_cells ;
}