#pragma once
#include <string>

std::string infer_type(const std::string& value);

bool is_int_literal(const std::string& s);
bool is_double_literal(const std::string& s);
bool is_float_literal(const std::string& s);
bool is_longlong_literal(const std::string& s);
bool is_string_literal(const std::string& s);
bool is_char_literal(const std::string& s);
bool is_bool_literal(const std::string& s);