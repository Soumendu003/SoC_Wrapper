#include"Header.h"

// Compares two string ignoring the case of letters
int Strcmpi(const char* str1, const char* str2){
    uint32_t i;
     
    if(strlen(str1)!=strlen(str2))
        return -1;
         
    for(i=0;i<strlen(str1);i++){
        if(toupper(str1[i])!=toupper(str2[i]))
            return str1[i]-str2[i];
    }
    return 0;
}

bool Prefix_Match(string Str, string Pre_Str){
    if(Pre_Str.size() == 0 || Str.size() < Pre_Str.size()){
        return false;
    }
    for(uint16_t i = 0; i < Pre_Str.size(); i++){
        if(Str[i] != Pre_Str[i]){
            return false;
        }
    }
    return true;
}

int Min(int a, int b){
    if(a > b){
        return b;
    }
    return a;
}