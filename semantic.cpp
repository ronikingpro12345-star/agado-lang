#include "semantic.hpp"
#include <cctype>

using namespace std;

bool is_bool_literal(const string& s){
    return s=="true" || s=="false";
}

bool is_string_literal(const string& s){
    return s.size()>=2 &&
           s.front()=='"' &&
           s.back()=='"';
}

bool is_char_literal(const string& s){
    return s.size()==3 &&
           s.front()=='\'' &&
           s.back()=='\'';
}

bool is_float_literal(const string& s){
    return s.size()>1 &&
           s.back()=='f';
}

bool is_double_literal(const string& s){

    bool dot=false;

    for(char c:s){

        if(c=='.'){
            if(dot) return false;
            dot=true;
        }
        else if(!isdigit(c))
            return false;
    }

    return dot;
}

bool is_int_literal(const string& s){

    if(s.empty()) return false;

    for(char c:s){
        if(!isdigit(c))
            return false;
    }

    return true;
}

bool is_longlong_literal(const string& s){

    if(!is_int_literal(s))
        return false;

    try{
        long long x = stoll(s);
        return x > 2147483647LL;
    }
    catch(...){
        return false;
    }
}

string infer_type(const string& value){

    if(is_bool_literal(value))
        return "bool";

    if(is_string_literal(value))
        return "string";

    if(is_char_literal(value))
        return "char";

    if(is_float_literal(value))
        return "float";

    if(is_longlong_literal(value))
        return "longlong";

    if(is_double_literal(value))
        return "double";

    if(is_int_literal(value))
        return "int";

    return "unknown";
}