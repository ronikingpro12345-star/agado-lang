//this AGADO interpted programing language//
#include <iostream>
#include <vector>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <functional>
#include <variant>
#include <cmath>
#include <stack>
#include <fstream>
#include "ast.hpp"
#include "semantic.hpp"
using namespace std;

struct Block {
    int start;
    int end;
};
struct UserFunc {
    string return_type;
    int start_line;
    int end_line;
    vector<string> param_names;
};

class intre{
    public:
set<string>active_library;
vector<string>full_code;
vector<string>order;
set<string>key_word;
map<string, UserFunc> user_funcs;
map<string, map<string, function<double(vector<string>)>>> libs;

map<string, string> var_type;
vector<string>lines;
vector<map<string, variant<int,double,float,long long,char,string,bool>>> scope_stack;
variant<int,double,float,long long,char,string,bool> function_return_value;

bool function_return_flag = false;
bool break_flag = false;
bool continue_flag = false;
bool runtime_error_flag = false;

void write_to_file(const string& filename){

    ofstream fout(filename);

    for(string &line : full_code){
        fout << line << endl;
    }

    fout.close();

    cout << "Code saved to: " << filename << endl;
}

void load_from_file(const string& filename){

    ifstream fin(filename);

    // 🔥 If file does not exist → create it
    if(!fin){
        ofstream fout(filename);
        cout << "New file created: " << filename << endl;
        fout.close();
        return;
    }

    cout << "Loading file: " << filename << endl;

    string line;
    while(getline(fin, line)){
        if(line.empty()) continue;
        full_code.push_back(line);
    }

    fin.close();
}
vector<Block> blocks;

double eval(ASTNode* node){

    if(node==nullptr)
        return 0;


    if(auto n=
       dynamic_cast<NumberNode*>(node))
    {
        return n->value;
    }


    if(auto v=
      dynamic_cast<VariableNode*>(node))
    {
        return get_number(
            v->name
        );
    }


    if(auto b=
       dynamic_cast<BinaryNode*>(node))
    {
        double L=
        eval(
            b->left
        );

        double R=
        eval(
            b->right
        );


        if(b->op=="+")
            return L+R;

        if(b->op=="-")
            return L-R;

        if(b->op=="*")
            return L*R;

        if(b->op=="/")
            return L/R;

        if(b->op=="**")
            return pow(L,R);

        if(b->op=="==")
            return L==R;

        if(b->op=="!=")
            return L!=R;

        if(b->op==">")
            return L>R;

        if(b->op=="<")
            return L<R;

        if(b->op==">=")
            return L>=R;

        if(b->op=="<=")
            return L<=R;

        if(b->op=="&&")
            return L&&R;

        if(b->op=="||")
            return L||R;
    }

    return 0;
}
void build_blocks(vector<string>& code){
    stack<int> st;

    for(int i = 0; i < code.size(); i++){

        tokenize(code[i]);  // IMPORTANT

        for(auto &tok : order){

            if(tok == "{"){
                st.push(i);
            }
            else if(tok == "}"){
                if(st.empty()){
                    cout << "Error: unmatched }\n";
                    continue;
                }

                int start = st.top();
                st.pop();

                blocks.push_back({start, i});
            }
        }
    }
}
void execute(ASTNode* node)
{
    if(!node) return;

    if(auto a=
       dynamic_cast<AssignNode*>(node))
    {
        double val=
        eval(a->value);

        set_var(
            a->name,
            val
        );
    }

    else if(auto o=
            dynamic_cast<OutputNode*>(node))
    {
        cout<<
        eval(
        o->expr
        );
    }

    else if(auto i=
            dynamic_cast<InputNode*>(node))
    {
        double x;

        cin>>x;

        set_var(
        i->name,
        x
        );
    }
}
ASTNode* parseStatement(
    vector<string>& t
)
{
    if(t.empty())
        return nullptr;

    ASTParser parser;

    // =====================
    // let: int x = expr
    // =====================

    if(t[0]=="let:")
    {
        string type=t[1];
        string name=t[2];

        ASTNode* value=nullptr;

        if(
           t.size()>4 &&
           t[3]=="="
        )
        {
            value=

            parser.parse(
                t,
                4,
                t.size()-1
            );
        }

        return new DeclareNode(
            type,
            name,
            value
        );
    }


    // =====================
    // op: expression
    // =====================

    if(t[0]=="op:")
    {
        ASTNode* expr=

        parser.parse(
            t,
            1,
            t.size()-1
        );

        return new OutputNode(
            expr
        );
    }


    // =====================
    // ip: variable
    // =====================

    if(
       t[0]=="ip:" &&
       t.size()>1
    )
    {
        return new InputNode(
            t[1]
        );
    }


    // =====================
    // x = expression
    // =====================

    if(
       t.size()>2 &&
       t[1]=="="
    )
    {
        string name=t[0];

        ASTNode* expr=

        parser.parse(
            t,
            2,
            t.size()-1
        );

        return new AssignNode(
            name,
            expr
        );
    }

    return nullptr;
}
double get_number(string s){

    // VARIABLE
    if(find_var(s) != nullptr){
        auto &v = *find_var(s);

        if(holds_alternative<int>(v)) return get<int>(v);
        if(holds_alternative<double>(v)) return get<double>(v);
        if(holds_alternative<float>(v)) return get<float>(v);
        if(holds_alternative<long long>(v)) return get<long long>(v);
        if(holds_alternative<bool>(v)) return get<bool>(v) ? 1 : 0;
    }

    // NUMBER
    if(is_number(s)){
       return safe_stod(s);
    }

    //❌ FINAL PROTECTION
   cout << "Error: invalid token in expression: " << s << endl;
throw runtime_error("STOP");
}
int find_block_end(int start){
    for(auto &b : blocks){
        if(b.start == start)
            return b.end;
    }
    return -1;
}
int find_block_end(vector<string>& lines, int start){
    int depth = 0;
    bool started = false;

    for(int i = start; i < lines.size(); i++){

        tokenize(lines[i]);  // IMPORTANT

        for(auto &tok : order){

            if(tok == "{"){
                depth++;
                started = true;
            }
            else if(tok == "}"){
                depth--;
            }

            if(started && depth == 0){
                return i;
            }
        }
    }

    return -1;
}
void type_check(string type, variant<int,double,float,long long,char,string,bool> val) {

    if(type == "int" && !holds_alternative<int>(val)) {
        throw runtime_error("Type error: expected int");
    }

    if(type == "double" && !holds_alternative<double>(val)) {
        throw runtime_error("Type error: expected double");
    }

    if(type == "string" && !holds_alternative<string>(val)) {
        throw runtime_error("Type error: expected string");
    }

    if(type == "char" && !holds_alternative<char>(val)) {
        throw runtime_error("Type error: expected char");
    }

    if(type == "float" && !holds_alternative<float>(val)) {
        throw runtime_error("Type error: expected float");
    }

    if(type == "longlong" && !holds_alternative<long long>(val)) {
        throw runtime_error("Type error: expected longlong");
    }

    if(type == "bool" && !holds_alternative<bool>(val)) {
        throw runtime_error("Type error: expected bool");
    }
}
//tokenize//
void tokenize(const string &line) {
    order.clear();
    string temp = "";

    for(size_t i = 0; i < line.size(); i++) {
        char c = line[i];

        // 🔥 SPACE
        if(c == ' '){
            if(!temp.empty()){
                order.push_back(temp);
                temp = "";
            }
            continue;
        }

      // 🔥 STRING (with escape support)
if(c == '"'){
    if(!temp.empty()){
        order.push_back(temp);
        temp = "";
    }

    string str = "";
    i++;

    while(i < line.size()){
        if(line[i] == '\\' && i + 1 < line.size()){
            if(line[i+1] == 'n') str += '\n';
            else if(line[i+1] == 't') str += '\t';
            else if(line[i+1] == '"') str += '"';
            else str += line[i+1];
            i += 2;
            continue;
        }

        if(line[i] == '"') break;

        str += line[i];
        i++;
    }

    order.push_back("\"" + str + "\"");
    continue;
}

        // 🔥 DOT
        if(c == '.'){

    // 🔥 decimal number support
    if(!temp.empty() && isdigit(temp.back())){
        temp += c;
        continue;
    }

    if(!temp.empty()){
        order.push_back(temp);
        temp = "";
    }

    order.push_back(".");
    continue;
}
       if(c == '-' && (i == 0 || line[i-1] == '(' || line[i-1] == '=')){
    temp += c; // treat as part of number
    continue;
}
if(c == '\''){
    if(!temp.empty()){
        order.push_back(temp);
        temp = "";
    }

    string ch = "";
    ch += c;
    i++;

    while(i < line.size()){
        ch += line[i];

        if(line[i] == '\'') break;

        i++;
    }

    order.push_back(ch);
    continue;
}
        // 🔥 OPERATORS (+ - * / ( ))
      if(c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')'){
    if(!temp.empty()){
        order.push_back(temp);
        temp = "";
    }

    // ** operator
    if(c == '*' && i + 1 < line.size() && line[i+1] == '*'){
        order.push_back("**");
        i++;
    }
    else{
        order.push_back(string(1,c));
    }
    continue;
}
if(c == '&' && i+1 < line.size() && line[i+1] == '&'){
    if(!temp.empty()){
        order.push_back(temp);
        temp = "";
    }
    order.push_back("&&");
    i++;
    continue;
}

if(c == '|' && i+1 < line.size() && line[i+1] == '|'){
    if(!temp.empty()){
        order.push_back(temp);
        temp = "";
    }
    order.push_back("||");
    i++;
    continue;
}

        // 🔥 COMPARISON
        if(c == '>' || c == '<' || c == '=' || c == '!'){
            if(!temp.empty()){
                order.push_back(temp);
                temp = "";
            }

            string op(1, c);
            if(i+1 < line.size() && line[i+1] == '='){
                op += '=';
                i++;
            }

            order.push_back(op);
            continue;
        }

        // 🔥 SPECIAL
        if(c == ';' || c == '{' || c == '}' || c == ',' ){
            if(!temp.empty()){
                order.push_back(temp);
                temp = "";
            }
            order.push_back(string(1, c));
            continue;
        }

       if(c == ':' && i+1 < line.size() && line[i+1] == ':'){
    if(!temp.empty()){
        order.push_back(temp);
        temp = "";
    }

    order.push_back("::");
    i++;
    continue;
}

        // 🔥 NORMAL CHAR 
        temp += c;
    }

    if(!temp.empty()) order.push_back(temp);
}
int execute_for_block(int start){

    // 🔥 Parse lines
    int i = start + 1;

    vector<string> init_line, cond_line, step_line;

    // --- READ HEADER ---
    for(; i < full_code.size(); i++){

        tokenize(full_code[i]);
        vector<string> t = order;

        if(t.empty()) continue;

        if(t[0] == "let:"){
            init_line = t;
        }
        else if(t[0] == "cond:"){
            cond_line = t;
            cond_line.erase(cond_line.begin()); // remove cond:
            // ✅ FIX: remove accidental "{"
if(!cond_line.empty()){
    if(cond_line.back() == "{"){
        cond_line.pop_back();
    }
    else if(cond_line.back().find("{") != string::npos){
        size_t pos = cond_line.back().find("{");
        cond_line.back() = cond_line.back().substr(0, pos);
    }
}
        }
       else if(t[0] == "step:"){
    step_line = t;
    step_line.erase(step_line.begin());

    // ✅ FIX: remove accidental "{"
    if(!step_line.empty()){
        if(step_line.back() == "{"){
            step_line.pop_back();
        }
        else if(step_line.back().find("{") != string::npos){
            size_t pos = step_line.back().find("{");
            step_line.back() = step_line.back().substr(0, pos);
        }
    }
}
        else if(t[0] == "{"){
            break;
        }
    }

    int block_start = i + 1;
    int block_end = find_block_end(i);

    // 🔥 INIT
    execute_line_tokens(init_line);
  cout << "[DEBUG] After init\n";
    cout << "[FOR DEBUG] Entered for block\n";

cout << "INIT: ";
for(auto &x : init_line) cout << x << " ";
cout << endl;

cout << "COND: ";
for(auto &x : cond_line) cout << x << " ";
cout << endl;

cout << "STEP: ";
for(auto &x : step_line) cout << x << " ";
cout << endl;

cout << "BLOCK START: " << block_start << endl;
cout << "BLOCK END: " << block_end << endl;
    // 🔥 LOOP
   // 🔥 LOOP
while(true){

    double condition = evaluate_expression(cond_line, 0, cond_line.size()-1);
    if(condition == 0) break;

 for(int j = block_start; j < block_end; j++){

    tokenize(full_code[j]);

    string mode="", type="", varname="", value="";
    parsue_line(order, mode, type, varname, value);

    // 🔥 HANDLE NESTED CONTROL FLOW
    if(mode == "for"){
        execute_for_block(j);
        j = find_block_end(j);
        continue;
    }

    if(mode == "while"){
        execute_while_block(j);
        j = find_block_end(j);
        continue;
    }

    if(mode == "if"){
    j = execute_if_chain(j);

    if(function_return_flag){
        break; // or break depending on your function
    }

    continue;
}

    // 🔥 NORMAL LINE
    execute_line(full_code[j]);

    if(break_flag){
        break_flag = false;
        return block_end;
    }

    if(continue_flag){
        continue_flag = false;
        break;
    }
}

    // 🔥 STEP FIX
    if(step_line.back() != ";")
    step_line.push_back(";");

execute_line_tokens(step_line);

if(step_line.back() == ";")
    step_line.pop_back();
}
return block_end;
}
double cast_value(string type, double val){

    if(type == "int") return (int)val;
    else if(type == "float") return (float)val;
    else if(type == "double") return val;
    else if(type == "longlong") return (long long)val;

    return val;
}
void handle_output(vector<string>& tokens, int l, int r){

    for(int i = l; i <= r; i++){

        // 🔥 STRING
        if(tokens[i].front() == '"' && tokens[i].back() == '"'){
            cout << tokens[i].substr(1, tokens[i].size() - 2);
        }

      // 🔥 COMMA = skip
        else if(tokens[i] == ","){
            continue;
        }

        // 🔥 EXPRESSION / VARIABLE
        else{
            double val = evaluate_expression(tokens, i, i);
            cout << val;
        }
    }
}
void execute_line_tokens(vector<string> tokens){
// 🔥 HANDLE DIRECT ASSIGNMENT (x = x + 1)
if(tokens.size() >= 3 && tokens[1] == "="){
    string var = tokens[0];

    double val = evaluate_expression(tokens, 2, tokens.size()-1);

 set_var(var, cast_value(var_type[var], val));  // 👈 IMPORTANT

    return;
}
    if(tokens.empty()) return;

    // 🔥 REMOVE trailing ";"
    if(tokens.back() == ";") tokens.pop_back();

    // =========================
    // 🔥 MODE DETECTION
    // =========================
    string mode = "";

    if(tokens[0] == "let:") mode = "let";
    else if(tokens[0] == "op:") mode = "op";
    else if(tokens[0] == "ip:") mode = "ip";
    else mode = "assign"; // default

    // =========================
    // 🔥 LET (DECLARATION)
    // =========================
    if(mode == "let"){

        // let: x = expr
        string varl = tokens[1];

string type = "int"; // default

for(int i=2;i<tokens.size();i++)
{
    if(tokens[i] == "=")
    {
        type = infer_type(tokens[i+1]);
        break;
    }
}
        var_type[varl] = type;

        // default init
      set_var(varl, 0);

        // check assignment
        if(tokens.size() > 3 && tokens[3] == "="){

            double val = evaluate_expression(tokens, 4, tokens.size() - 1);
           set_var(varl, cast_value(type, val));
        }

        return;
    }

    // =========================
    // 🔥 INPUT
    // =========================
    if(mode == "ip"){

        string varl = tokens[1];

        if(find_var(varl) == nullptr){
            cout << "Error: variable not declared\n";
            throw runtime_error("STOP");
        }

        double val;
        cin >> val;

        set_var(varl, val);
        return;
    }

    // =========================
    // 🔥 OUTPUT
    // =========================
    if(mode == "op"){

        handle_output(tokens, 1, tokens.size() - 1);
        cout << "\n";
        return;
    }

    // =========================
    // 🔥 ASSIGNMENT
    // =========================
    if(mode == "assign"){

        // x = expr
        string varl = tokens[0];

       if(find_var(varl) == nullptr){
            cout << "Error: variable not declared\n";
            throw runtime_error("STOP");
        }

        if(tokens.size() < 3 || tokens[1] != "="){
            cout << "Error: invalid assignment\n";
            throw runtime_error("STOP");
        }

        double val = evaluate_expression(tokens, 2, tokens.size() - 1);

        set_var(varl, cast_value(var_type[varl], val));
        return;
    }
}
int execute_while_block(int start){

    // 🔥 Extract condition
    tokenize(full_code[start]);
    vector<string> cond_tokens = order;

    // remove "while:"
    cond_tokens.erase(cond_tokens.begin());

    // remove "{"
   if(!cond_tokens.empty()){
    if(cond_tokens.back() == "{"){
        cond_tokens.pop_back();
    }
    else if(cond_tokens.back().find("{") != string::npos){
        // handle "5{" case
        cond_tokens.back().erase(cond_tokens.back().find("{"));
    }
}
    int block_start = start + 1;
    int block_end = find_block_end(start);

    // 🔥 LOOP
    while(true){

        double condition;

        // 🔥 handle "true"
        if(cond_tokens.size() == 1 && cond_tokens[0] == "true"){
            condition = 1;
        }
        else if(cond_tokens.size() == 1 && cond_tokens[0] == "false"){
            condition = 0;
        }
        else{
            condition = evaluate_expression(cond_tokens, 0, cond_tokens.size()-1);
        }

        if(condition == 0) break;

        for(int i = block_start; i < block_end; i++){

    tokenize(full_code[i]);
    string mode="", type="", varname="", value="";
    parsue_line(order, mode, type, varname, value);

 if(mode == "if"){
    i = execute_if_chain(i);

    if(function_return_flag){
        return block_end; // 🔥 propagate return out of loop
    }

    continue;
}

if(mode == "elif" || mode == "else"){
    int end = find_block_end(i);
    i = end;
    continue;
}

// 🔥 ONLY NORMAL STATEMENTS
execute_line(full_code[i]);

    if(break_flag){
        break_flag = false;
        return block_end;
    }

    if(continue_flag){
        continue_flag = false;
        break;
    }
        }
    }
    return block_end;
}
void register_function(int start) {
    tokenize(full_code[start]);

    string mode="", type="", varname="", value="";
    parsue_line(order, mode, type, varname, value);

    if(mode != "func") return;

    int end = find_block_end(start);

    vector<string> params;

for(int i = 0; i < order.size(); i++){
    if(order[i] == "("){
        i++;

        while(i < order.size() && order[i] != ")"){

            // 🔥 ONLY capture variable name
            if(order[i] == "let:" ||
               order[i] == "int" ||
               order[i] == "double" ||
               order[i] == "float" ||
               order[i] == "longlong" ||
               order[i] == "char" ||
               order[i] == "bool" ||
               order[i] == "string" ||
               order[i] == "," ||
               order[i] == "="){
                i++;
                continue;
            }
// ❗ SKIP DEFAULT VALUES
if(is_number(order[i])){
    i++;
    continue;
}
            params.push_back(order[i]);
            i++;
        }
        break;
    }
}

    user_funcs[varname] = {type, start, end, params};
}

variant<int,double,float,long long,char,string,bool> call_user_function(const string& name,vector<string> args)
 {
    if(!user_funcs.count(name)){
        cout << "Error: function not found: " << name << "\n";
        return variant<int,double,float,long long,char,string,bool>(0);
    }

    UserFunc fn = user_funcs[name];
function_return_flag = false;
function_return_value = 0;

    scope_stack.push_back({});
// 🔥 Bind parameters
for(int i = 0; i < fn.param_names.size(); i++){
    string param = fn.param_names[i];

    double val = 0;

    if(i < args.size()){
        val = get_number(args[i]);
    }
else {
        val = 10; // 🔥 TEMP: your default (proper fix later)
    }
    set_var(param, val);
}
    for(int i = fn.start_line + 1; i < fn.end_line; i++){
        tokenize(full_code[i]);
        lines = order;

        string mode="", type="", varname="", value="";
        parsue_line(order, mode, type, varname, value);

       if(mode == "if"){
    i = execute_if_chain(i);

    // 🔥 STOP if return happened
    if(function_return_flag){
    scope_stack.pop_back();   // clean scope
    return function_return_value;  // 🔥 EXIT FUNCTION IMMEDIATELY
}

    continue;
}
        else if(mode == "else" || mode == "elif"){
            int end = find_block_end(i);
            if(end != -1) i = end;
            continue;
        }
       else if(mode == "for"){
    int end = execute_for_block(i);

    if(function_return_flag){
        break;
    }

    if(end != -1) i = end;
    continue;
}
       else if(mode == "while"){
    int end = execute_while_block(i);

    if(function_return_flag){
        break;
    }

    if(end != -1) i = end;
    continue;
}
        else if(mode == "func"){
            int end = find_block_end(i);
            if(end != -1) i = end;
            continue;
        }
      else if(mode == "return"){
    if(fn.return_type == "void"){
        function_return_flag = true;
    }
    else{
        function_return_value = evaluate_typed_value(lines, 1, fn.return_type);
        type_check(fn.return_type, function_return_value);
        function_return_flag = true;
    }

    break;
}

        else{
            execute_line(full_code[i]);

if(function_return_flag){
    scope_stack.pop_back();
    return function_return_value;
}
        }
    }
    scope_stack.pop_back();

if(function_return_flag){
    return function_return_value;
}

if(fn.return_type == "void"){
    return 0;
}

cout << "Error: missing return in function: " << name << "\n";
throw runtime_error("Missing return");
}

variant<int,double,float,long long,char,string,bool>
evaluate_typed_value(vector<string>& tokens, int start, const string& type) {
    if(type == "string"){
        if(start < tokens.size()) return parse_value(tokens[start], "string");
    }
    else if(type == "char"){
        if(start < tokens.size()) return parse_value(tokens[start], "char");
    }
    else if(type == "bool"){
        if(start < tokens.size()) {
            if(tokens[start] == "true") return true;
            if(tokens[start] == "false") return false;
            double x = evaluate_expression(tokens, start, tokens.size()-1);
            return x != 0;
        }
    }
    else if(type == "int"){
        double x = evaluate_expression(tokens, start, tokens.size()-1);
        return (int)x;
    }
    else if(type == "double"){
        double x = evaluate_expression(tokens, start, tokens.size()-1);
        return x;
    }
    else if(type == "float"){
        double x = evaluate_expression(tokens, start, tokens.size()-1);
        return (float)x;
    }
    else if(type == "longlong"){
        double x = evaluate_expression(tokens, start, tokens.size()-1);
        return (long long)x;
    }

    return string("");
}

void parsue_line(const vector<string>& lines, string &mode, string &type, string &varname, string &value)
{
    mode=""; type=""; varname=""; value="";

    for(size_t i=0; i<lines.size(); i++){
        string c = lines[i];

        if(c==";") break;
if(lines.size() > 0 && lines[0] == "func:"){
    mode = "func";

    if(lines.size() >= 3){
        type = lines[1];
        varname = lines[2];
    }

    return;
}
        // 🔥 MODE
        if(c=="using:") mode="using";
         else if(c=="for:") mode="for";
         else if(c=="cond:") mode="cond";
         else if(c=="func:") mode="func";
         else if(c=="return") mode="return";
   else if(c=="step:") mode="step";
          else if(c=="while:") mode="while";
        else if(c=="let:") mode="let";
        else if(c=="op:") mode="op";
        else if(c=="ip:") mode="ip";
        else if(c=="if:") mode="if";
       else if(c=="elif:" ){
    mode = "elif";
}
       else if(c=="else:" || c=="else") mode="else";
          else if(c=="break") mode="break";
          else if(c=="continue") mode="continue";
        // 🔥 TYPE (ONLY for let)
       // NEW: let: int x
else if(mode=="let")
{
    if(i == 1)
    {
        varname = c;

        for(int j=i+1;j<lines.size();j++)
        {
            if(lines[j] == "=")
            {
                type = infer_type(lines[j+1]);
                break;
            }
        }
    }
}
        // 🔥 ASSIGNMENT
        else if(c=="="){
            if(mode != "let" && i>0){
                varname = lines[i-1];
            }
            if(i+1 < lines.size()){
                value = lines[i+1];
            }
            if(mode=="") mode="assign";
        }

        // 🔥 SIMPLE MODES
        else if(mode=="using" || mode=="op" || mode=="ip"){
            varname = c;
        }
    }
} 
//expression evaluator//
double evaluate_expression(
    vector<string>& tokens,
    int l,
    int r
)
{
    vector<string> expr;

    for(
        int i=l;
        i<=r &&
        i<tokens.size();
        i++
    )
    {
        if(tokens[i]==";")
            break;

        expr.push_back(
            tokens[i]
        );
    }

    expr=
    resolve_user_func_calls(
        expr,
        0,
        expr.size()-1
    );

    ASTParser parser;

    ASTNode* root=

    parser.parse(
        expr,
        0,
        expr.size()-1
    );

    double result=
    eval(root);

    free_ast(root);

    return result;
}
bool is_number(string s){
    if(s.empty()) return false;

    int dot = 0;
    int start = (s[0] == '-') ? 1 : 0;

    for(int i = start; i < s.size(); i++){
        if(s[i] == '.'){
            dot++;
            if(dot > 1) return false;
        }
        else if(!isdigit(s[i])){
            return false;
        }
    }

    return true;
}
variant<int,double,float,long long,char,string,bool> eval_expr(vector<string>& lines, int start)
{
    // 🔥 Start from first value after "="
    variant<int,double,float,long long,char,string,bool> result;

    // get first value
    string first = lines[start];

    if(isdigit(first[0])){
        result = stoi(first);
    } else if(first[0] == '"'){
        result = first.substr(1, first.size()-2);
    } else {
        result = *find_var(first); // variable
    }

    // 🔥 process rest: + - * /
    for(int i = start + 1; i +1 < lines.size(); i += 2)
    {
        if(lines[i] == ";") break;

        string op = lines[i];
        string next = lines[i+1];

        int r;

        if(isdigit(next[0])) r = stoi(next);
        else r = get<int>(*find_var(next));

        int l = get<int>(result);

        if(op == "+") result = l + r;
        else if(op == "-") result = l - r;
        else if(op == "*") result = l * r;
        else if(op == "/") result = l / r;
    }

    return result;
}
variant<int,double,float,long long,char,string,bool> parse_value(string val, string type)
{
    // 🔥 STRING ("hello")
    if(type == "string"){
        if(val.size() >= 2 && val[0] == '"' && val.back() == '"'){
            return val.substr(1, val.size() - 2);
        }
        return val;
    }

    // 🔥 CHAR ('a')
    if(type == "char"){
        return val[1];
    }
if(type == "bool"){
    if(val == "true") return true;
    if(val == "false") return false;
}
    // 🔥 INT
    if(type == "int"){
        return stoi(val);
    }

    // 🔥 DOUBLE
    if(type == "double"){
        return stod(val);
    }

    // 🔥 FLOAT
    if(type == "float"){
        return stof(val);
    }

    // 🔥 LONGLONG
    if(type == "longlong"){
        return stoll(val);
    }

    // fallback
    return val;
}
variant<int,double,float,long long,char,string,bool>* find_var(string name){
    for(int i = scope_stack.size()-1; i >= 0; i--){
        if(scope_stack[i].count(name))
            return &scope_stack[i][name];
    }
    return nullptr;
}

void set_var(string name, variant<int,double,float,long long,char,string,bool> value){
    for(int i = scope_stack.size()-1; i >= 0; i--){
        if(scope_stack[i].count(name)){
            scope_stack[i][name] = value;
            return;
        }
    }
    scope_stack.back()[name] = value;
}
string evaluate_arg(vector<string> arg_tokens){
    arg_tokens = resolve_user_func_calls(arg_tokens, 0, arg_tokens.size()-1);
    double val = evaluate_expression(arg_tokens, 0, arg_tokens.size()-1);
    return to_string(val);
}
vector<string> resolve_user_func_calls(vector<string> tokens, int start, int end){
    vector<string> out;

    for(int i = start; i <= end && i < tokens.size(); i++){
        if(tokens[i] == ";") break;

       if(i + 1 < tokens.size() &&
   tokens[i+1] == "(" &&
   user_funcs.count(tokens[i]))
{
    vector<string> args;
    int j = i + 2;

    // 🔥 Extract arguments
vector<string> current;
int depth = 0;

while(j < tokens.size()){
    if(tokens[j] == "(") depth++;

    if(tokens[j] == ")"){
        if(depth == 0){
            if(!current.empty()) args.push_back(evaluate_arg(current));
            break;
        }
        depth--;
    }

    if(tokens[j] == "," && depth == 0){
        args.push_back(evaluate_arg(current));
        current.clear();
    }
    else{
        current.push_back(tokens[j]);
    }

    j++;
}
    // 🔥 Call with args
    auto ret = call_user_function(tokens[i], args);
    string rtype = user_funcs[tokens[i]].return_type;

    if(rtype == "void"){
        i = j;   // skip till ')'
        continue;
    }

    if(holds_alternative<int>(ret)) out.push_back(to_string(get<int>(ret)));
    else if(holds_alternative<double>(ret)) out.push_back(to_string(get<double>(ret)));
    else if(holds_alternative<float>(ret)) out.push_back(to_string(get<float>(ret)));
    else if(holds_alternative<long long>(ret)) out.push_back(to_string(get<long long>(ret)));
    else if(holds_alternative<bool>(ret)) out.push_back(get<bool>(ret) ? "true" : "false");
    else if(holds_alternative<char>(ret)) out.push_back(string(1, get<char>(ret)));
    else if(holds_alternative<string>(ret)) out.push_back("\"" + get<string>(ret) + "\"");

    i = j-1;   // 🔥 IMPORTANT: jump to ')'
    continue;
}
        out.push_back(tokens[i]);
    }

    return out;
}

void execute_line(string line) {
    // 1️⃣ Tokenize
    tokenize(line);
    lines = order;

    // 🔥 DEBUG: show tokens
    cout << "\n[DEBUG] TOKENS: ";
    for(auto &t : lines) cout << "[" << t << "] ";
    cout << endl;

    // 2️⃣ Parse line
    string mode, type, varname, value;
    parsue_line(order, mode, type, varname, value);

    cout << "[DEBUG] MODE: " << mode 
         << " TYPE: " << type 
         << " VAR: " << varname 
         << " VALUE: " << value << endl;

    // 3️⃣ Execute
    auto assign_value = [&](string t, double res) {
        variant<int,double,float,long long,char,string,bool> temp_val;
        if(t == "int") temp_val = (int)res;
        else if(t == "double") temp_val = res;
        else if(t == "float") temp_val = (float)res;
        else if(t == "longlong") temp_val = (long long)res;
        else if(t == "bool") temp_val = (res != 0);
        else temp_val = res;

        type_check(t, temp_val);
        return temp_val;
    };
auto assign_variant_value = [&](string target_type,
    variant<int,double,float,long long,char,string,bool> val)
    -> variant<int,double,float,long long,char,string,bool>
{
    if(target_type == "int"){
        if(holds_alternative<int>(val)) return get<int>(val);
        if(holds_alternative<double>(val)) return (int)get<double>(val);
        if(holds_alternative<float>(val)) return (int)get<float>(val);
        if(holds_alternative<long long>(val)) return (int)get<long long>(val);
        if(holds_alternative<bool>(val)) return (int)get<bool>(val);
    }
    else if(target_type == "double"){
        if(holds_alternative<int>(val)) return (double)get<int>(val);
        if(holds_alternative<double>(val)) return get<double>(val);
        if(holds_alternative<float>(val)) return (double)get<float>(val);
        if(holds_alternative<long long>(val)) return (double)get<long long>(val);
        if(holds_alternative<bool>(val)) return (double)get<bool>(val);
    }
    else if(target_type == "float"){
        if(holds_alternative<int>(val)) return (float)get<int>(val);
        if(holds_alternative<double>(val)) return (float)get<double>(val);
        if(holds_alternative<float>(val)) return get<float>(val);
        if(holds_alternative<long long>(val)) return (float)get<long long>(val);
        if(holds_alternative<bool>(val)) return (float)get<bool>(val);
    }
    else if(target_type == "longlong"){
        if(holds_alternative<int>(val)) return (long long)get<int>(val);
        if(holds_alternative<double>(val)) return (long long)get<double>(val);
        if(holds_alternative<float>(val)) return (long long)get<float>(val);
        if(holds_alternative<long long>(val)) return get<long long>(val);
        if(holds_alternative<bool>(val)) return (long long)get<bool>(val);
    }
    else if(target_type == "bool"){
        if(holds_alternative<int>(val)) return get<int>(val) != 0;
        if(holds_alternative<double>(val)) return get<double>(val) != 0;
        if(holds_alternative<float>(val)) return get<float>(val) != 0;
        if(holds_alternative<long long>(val)) return get<long long>(val) != 0;
        if(holds_alternative<bool>(val)) return get<bool>(val);
    }
    else if(target_type == "char"){
        if(holds_alternative<char>(val)) return get<char>(val);
    }
    else if(target_type == "string"){
        if(holds_alternative<string>(val)) return get<string>(val);
    }

    throw runtime_error("Type mismatch in function return assignment");
};

    auto evaluate_assignment = [&](int start, int end) -> double {
        try { return evaluate_expression(lines, start, end); }
        catch(...) { runtime_error_flag = true; return 0; }
    };
if(mode == "" && lines.size() >= 2 &&
   lines[1] == "(" &&
   user_funcs.count(lines[0]))
{
    vector<string> args;
    int j = 2;

    while(j < lines.size() && lines[j] != ")"){
        if(lines[j] != ",")
            args.push_back(lines[j]);
        j++;
    }

    call_user_function(lines[0], args);
    return;
}
// ❌ NEVER EXECUTE FUNCTION DEFINITIONS
if(mode == "func"){
    return;
}
    // 🔹 USING: include library
    if(mode == "using") {
        active_library.insert(varname);
        return;
    }

    // 🔹 LET: declaration
  // 🔹 LET: declaration
if(mode == "let") {
    if(type.empty())
{
    int eq = -1;

    for(int i=0;i<lines.size();i++)
    {
        if(lines[i] == "=")
        {
            eq = i;
            break;
        }
    }

    if(eq != -1 && eq + 1 < lines.size())
        type = infer_type(lines[eq+1]);
}
    var_type[varname] = type;

    // Check for assignment '='
    int eq = -1;
    for(int i = 0; i < lines.size(); i++) 
        if(lines[i] == "=") { eq = i; break; }

    if(eq != -1 && eq+1 < lines.size()) {
        double res = 0;
        bool isLibFuncCall = false;

for(int i = eq+1; i < lines.size(); i++) {
    if(lines[i] == "::") {
        isLibFuncCall = true;
        break;
    }
}

if(eq != -1 && eq+2 < lines.size() &&
   lines[eq+2] == "(" &&
   user_funcs.count(lines[eq+1]))
{
    string func_name = lines[eq+1];

    vector<string> args;
    int j = eq + 3;

    while(j < lines.size() && lines[j] != ")"){
        if(lines[j] != ",")
            args.push_back(lines[j]);
        j++;
    }

    auto ret = call_user_function(func_name, args);

    auto final_val = assign_variant_value(type, ret);
    type_check(type, final_val);
    set_var(varname, final_val);

    return;
}

if(isLibFuncCall) {


    string lib, func;

    for(int i = eq+1; i < lines.size(); i++){
        if(lines[i] == "::"){
            lib = lines[i-1];
            func = lines[i+1];
            break;
        }
    }

    if(!active_library.count(lib)) {
        cout << "Error: library not included: " << lib << "\n";
        return;
    }

    vector<string> args;
    int paren_start = -1, paren_end = -1;
    int depth = 0;

    for(int i = eq+1; i < lines.size(); i++){
        if(lines[i] == "("){
            if(depth == 0) paren_start = i;
            depth++;
        }
        else if(lines[i] == ")"){
            depth--;
            if(depth == 0){
                paren_end = i;
                break;
            }
        }
    }

    // collect args
    for(int i = paren_start+1; i < paren_end; i++){
        if(lines[i] != ",")
            args.push_back(lines[i]);
    }

    double func_result = 0;

    if(libs[lib].count(func)) 
        func_result = libs[lib][func](args);
    else { 
        cout << "Error: function not found: " << func << "\n"; 
        return; 
    }
    // 🔥 BUILD NEW EXPRESSION
    vector<string> new_expr;

    // before function call
    for(int i = eq+1; i < eq+1; i++){
        new_expr.push_back(lines[i]);
    }

    // push result
    new_expr.push_back(to_string(func_result));

    // after function call
    for(int i = paren_end+1; i < lines.size(); i++){
        new_expr.push_back(lines[i]);
    }

    // 🔥 evaluate full expression
    res = evaluate_expression(new_expr, 0, new_expr.size()-1);
}
        else {
            // Normal expression assignment
            res = evaluate_assignment(eq+1, lines.size()-1);
        }

        // Assign value to variable
        try { set_var(varname, assign_value(type, res)); }
        catch(exception &e) { 
            cout << e.what() << endl; 
            runtime_error_flag = true; 
            return; 
        }
    } 
    else {
        // Default initialization if no assignment
        if(type == "int") set_var(varname, 0);
        else if(type == "double") set_var(varname, 0.0);
        else if(type == "float") set_var(varname, 0.0f);
        else if(type == "longlong") set_var(varname, (long long)0);
        else if(type == "bool") set_var(varname, false);
        else if(type == "char") set_var(varname, 'a');
        else set_var(varname, string(""));
    }
    return;
}
// 🔹 BREAK
if(mode == "break"){
    break_flag = true;
    return;
}

// 🔹 FOR LOOP

// 🔹 CONTINUE
if(mode == "continue"){
    continue_flag = true;
    return;
}

    // 🔹 ASSIGN: variable assignment
    if(mode == "assign") {
    if(!find_var(varname)) {
        cout << "Error: variable " << varname << " not declared\n";
        return;
    }

    int eq = -1;
    for(int i = 0; i < lines.size(); i++){
        if(lines[i] == "="){
            eq = i;
            break;
        }
    }

    if(eq != -1) {
        double res = evaluate_assignment(eq+1, lines.size()-1);
        try {
            *find_var(varname) = assign_value(var_type[varname], res);
        }
        catch(exception &e) {
            cout << e.what() << endl;
            runtime_error_flag = true;
            return;
        }
    }
    return;
}
    // 🔹 OP: out/ print
 if(mode == "op") {
    if(!active_library.count("ioput")) {
        cout << "Error: ioput library not included\n";
        return;
    }

    vector<string> part;

    auto print_part = [&](vector<string> part_tokens) {

    if(part_tokens.empty()) return;

    // 🔥 single token handling
    if(part_tokens.size() == 1){

        string tok = part_tokens[0];

        // STRING LITERAL
        if(tok.size() >= 2 &&
           tok.front() == '"' &&
           tok.back() == '"'){

            cout << tok.substr(1, tok.size()-2);
            return;
        }

        // CHAR LITERAL
        if(tok.size() >= 3 &&
           tok.front() == '\'' &&
           tok.back() == '\''){

            cout << tok[1];
            return;
        }

        // VARIABLE
        auto ptr = find_var(tok);

        if(ptr != nullptr){

            auto &v = *ptr;

            if(holds_alternative<int>(v))
                cout << get<int>(v);

            else if(holds_alternative<double>(v))
                cout << get<double>(v);

            else if(holds_alternative<float>(v))
                cout << get<float>(v);

            else if(holds_alternative<long long>(v))
                cout << get<long long>(v);

            else if(holds_alternative<bool>(v))
                cout << (get<bool>(v) ? "true" : "false");

            else if(holds_alternative<char>(v))
                cout << get<char>(v);

            else if(holds_alternative<string>(v))
                cout << get<string>(v);

            return;
        }
    }

    // 🔥 expression fallback
    try{
        double val = evaluate_expression(part_tokens, 0, part_tokens.size()-1);
        cout << val;
    }
    catch(...){
        runtime_error_flag = true;
    }
};
    // 🔥 Split by comma and process
    for(int i = 1; i < lines.size(); i++){
        string token = lines[i];

        if(token == ";"){
            print_part(part);
            break;
        }

        if(token == ","){
            print_part(part);
            cout << " ";
            part.clear();
            continue;
        }

        part.push_back(token);
    }

    // last part (if no semicolon caught it)
   // 🔥 Only print if not already printed
if(!part.empty() && lines.back() != ";"){
    print_part(part);
}

    cout << endl;
    return;
}
    // 🔹 IP: input
    if(mode == "ip") {
        if(!active_library.count("ioput")) { cout << "Error: ioput library not included\n"; return; }
        if(!find_var(varname)) { cout << "Error: variable " << varname << " not declared\n"; return; }

        string t = var_type[varname];
        if(t=="int"){ int x; cin >> x; set_var(varname, x);}
        else if(t=="double"){ double x; cin >> x; set_var(varname, x);}
        else if(t=="float"){ float x; cin >> x; set_var(varname, x);}
        else if(t=="longlong"){ long long x; cin >> x; set_var(varname, x);}
        else if(t=="char"){ char x; cin >> x; set_var(varname, x);}
        else { string x; cin.ignore(numeric_limits<streamsize>::max(),'\n'); getline(cin,x); set_var(varname,x);}
        return;
    }
}
void free_ast(ASTNode* node)
{
    if(node==nullptr)
        return;

    if(auto b=
       dynamic_cast<BinaryNode*>(node))
    {
        free_ast(b->left);
        free_ast(b->right);
    }

    delete node;
}
void execute_block(int start,int end)
{
    if(
        full_code.empty() ||
        start>end ||
        start<0 ||
        end>=full_code.size()
    )
        return;

    // create local scope only
    scope_stack.push_back({});

    for(
        int i=start;
        i<=end;
        i++
    )
    {
        execute_line(
            full_code[i]
        );

        // function return
        if(
           function_return_flag
        )
            break;

        // loop control
        if(
           break_flag ||
           continue_flag
        )
            break;
    }

    // remove local scope
    scope_stack.pop_back();
}
void execute_if_else(int start) {
    // tokenize the condition line
    tokenize(full_code[start]);
    vector<string> cond_tokens = order;
    cond_tokens.erase(cond_tokens.begin()); // remove "if:"

 double cond_result = evaluate_expression(cond_tokens, 0, cond_tokens.size()-1);

    int block_start = start+1;
    int block_end = find_block_end(start);

    if(cond_result != 0) {
        execute_block(block_start, block_end-1);
    } else {
        // look for else:
        if(block_end+1 < full_code.size() && full_code[block_end+1].find("else:") != string::npos){
            int else_start = block_end+2; // next line after {
            int else_end = find_block_end(block_end+1);
            execute_block(else_start, else_end-1);
        }
    }
}

double safe_stod(const string &s) {
    try {
        size_t idx;
        double val = stod(s, &idx);

        // 🔥 ensure FULL string is valid
        if(idx != s.size()) throw invalid_argument("extra chars");

        return val;
    } catch (...) {
        cout << "Error: invalid number -> " << s << endl;
        throw runtime_error("STOP"); // 🔥 stop execution
    }
}
void init_math_lib() {

    // ➕ ADD
    libs["math"]["add"] = [this](vector<string> args) -> double {
        double sum = 0;
        for(auto &a : args){
            sum += get_number(a);
        }
        return sum;
    };

    // ➖ SUB
    libs["math"]["sub"] = [this](vector<string> args) -> double {
        if(args.empty()) return 0;

        double res = get_number(args[0]);
        for(int i = 1; i < args.size(); i++){
            res -= get_number(args[i]);
        }
        return res;
    };

    // ✖ MUL
    libs["math"]["mul"] = [this](vector<string> args) -> double {
        double res = 1;
        for(auto &a : args){
            res *= get_number(a);
        }
        return res;
    };

    // ➗ DIV
    libs["math"]["div"] = [this](vector<string> args) -> double {
        if(args.empty()) return 0;

        double res = get_number(args[0]);
        for(int i = 1; i < args.size(); i++){
            double x = get_number(args[i]);
            if(x == 0){
                cout << "Error: division by zero\n";
                return 0;
            }
            res /= x;
        }
        return res;
    };

    // 🔥 SQUARE
    libs["math"]["sq"] = [this](vector<string> args) -> double {
        double x = get_number(args[0]);
        return x * x;
    };

    // 🔥 SQRT
    libs["math"]["sqrt"] = [this](vector<string> args) -> double {
        double x = get_number(args[0]);
        if(x < 0){
            cout << "Error: negative sqrt\n";
            return 0;
        }
        return sqrt(x);
    };

    // 🔥 ABS
    libs["math"]["abs"] = [this](vector<string> args) -> double {
        double x = get_number(args[0]);
        return (x < 0 ? -x : x);
    };

    // 🔥 ROUND
    libs["math"]["round"] = [this](vector<string> args) -> double {
        double x = get_number(args[0]);
        return (long long)(x + 0.5);
    };

    // 🔥 CEIL
    libs["math"]["ceil"] = [this](vector<string> args) -> double {
        double x = get_number(args[0]);
        long long r = (long long)x;
        if(x > r) r++;
        return r;
    };

    // 🔥 FLOOR
    libs["math"]["floor"] = [this](vector<string> args) -> double {
        double x = get_number(args[0]);
        return (long long)x;
    };
}
void get_line(int &i)
{
    lines.clear();

    while(i < full_code.size() && full_code[i] != ";"){
        lines.push_back(full_code[i]);
        i++;
    }

    if(i < full_code.size() && full_code[i] == ";"){
        lines.push_back(";");
    }
}
int execute_if_chain(int start) {
    int i = start;
    bool executed = false;

    while(true) {

        tokenize(full_code[i]);
        string mode="", type="", varname="", value="";
        parsue_line(order, mode, type, varname, value);

        // =====================
        // 🔥 IF / ELSE IF
        // =====================
       if(mode == "if" || mode == "elif") {

    vector<string> cond_tokens;

    for(auto &t : order){

       if(t == "if:" || t == "elif:" )
    continue;

if(t == "else" || t == "else:")
    continue;

        if(t == "{") break;

        cond_tokens.push_back(t);
    }

    // 🔥 ONLY HERE evaluate
   double cond_result = 0;

if(!executed){
    cond_result = evaluate_expression(cond_tokens, 0, cond_tokens.size()-1);
}
    cout << "[IF DEBUG] cond_result: " << cond_result << endl;

    int block_start = i + 1;
    int block_end   = find_block_end(i);

    if(!executed && cond_result != 0){
        execute_block(block_start, block_end-1);
if(function_return_flag) return block_end;
        if(break_flag || continue_flag) return i;

        executed = true;
    }

    i = block_end + 1;
}
        // =====================
        // 🔥 ELSE
        // =====================
        else if(mode == "else") {

            int block_start = i + 1;
            int block_end   = find_block_end(i);

            if(!executed){
                execute_block(block_start, block_end-1);

                if(break_flag || continue_flag) return i;
            }

           i = block_end + 1;
break;  // 🔥 VERY IMPORTANT
        }
    
        else {
    break;  // 🔥 STOP when chain ends
}
    }
    return i;
}
};
int main()
{
    intre o1;
    o1.scope_stack.push_back({});
    o1.init_math_lib();

    string filename;
    cout << "Enter file name (without extension): ";
    cin >> filename;

    filename += ".agd";

    // 🔥 Always load file
    o1.load_from_file(filename);

    // 🔥 If still empty → tell user and exit
    if(o1.full_code.empty()){
        cout << "File is empty. Please write code in " << filename << " and run again.\n";
        return 0;
    }
o1.build_blocks(o1.full_code);

for(int i = 0; i < o1.full_code.size(); i++){
    o1.tokenize(o1.full_code[i]);

    string mode="", type="", varname="", value="";
    o1.parsue_line(o1.order, mode, type, varname, value);

    if(mode == "func"){
        o1.register_function(i);
        int end = o1.find_block_end(i);
        if(end != -1) i = end;
    }
}

    // 🔥 Execute directly
 for(int i = 0; i < o1.full_code.size(); i++)
{
    o1.tokenize(o1.full_code[i]);
    o1.lines = o1.order;

    string mode="", type="", varname="", value="";
    o1.parsue_line(o1.order, mode, type, varname, value);

  if(mode == "if"){
    i = o1.execute_if_chain(i);

    // 🔥 STOP if return happened
    if(o1.function_return_flag){
        break;
    }

    continue;
}
else if(mode == "func"){
    int end = o1.find_block_end(i);

    // 🔥 SKIP ENTIRE FUNCTION BLOCK SAFELY
    if(end != -1){
        i = end;
    } else {
        cout << "Error: function block not closed properly\n";
        return 0;
    }

    continue;
}
    else if(mode == "else" || mode == "elif"){
        int end = o1.find_block_end(i);
        if(end != -1) i = end;
        continue;
    }

   else if(mode == "for"){
    int end = o1.execute_for_block(i);

    if(o1.function_return_flag){
        break;
    }
    if(end != -1) i = end;
    continue;
}
    else if(mode == "while"){
    int end = o1.execute_while_block(i);

    if(o1.function_return_flag){
        break;
    }

    if(end != -1) i = end;
    continue;
}

    else{
        o1.execute_line(o1.full_code[i]);
    }
}
    return 0;
}
