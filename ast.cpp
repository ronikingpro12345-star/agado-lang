#include "ast.hpp"
#include <iostream>
#include <cctype>

bool ASTParser::isOperator(
    const string& s
)
{
    return
    s=="+" ||
    s=="-" ||
    s=="*" ||
    s=="/" ||
    s=="**" ||

    s=="=="||
    s=="!="||

    s=="<" ||
    s==">" ||
    s=="<="||
    s==">="||

    s=="&&"||
    s=="||";
}


int ASTParser::precedence(
    const string& op
)
{
    if(op=="||")
        return 1;

    if(op=="&&")
        return 2;

    if(op=="==" || op=="!=")
        return 3;

    if(
        op=="<"||
        op==">"||
        op=="<="||
        op==">="
    )
        return 4;

    if(
        op=="+"||
        op=="-"
    )
        return 5;

    if(
        op=="*"||
        op=="/"
    )
        return 6;

    if(op=="**")
        return 7;

    return 0;
}



ASTNode* ASTParser::parse(
    vector<string>& tokens,
    int l,
    int r
)
{
    return build(
        tokens,
        l,
        r
    );
}

void printAST(
ASTNode* node,
int depth=0
)
{
    if(!node)
        return;

    for(
        int i=0;
        i<depth;
        i++
    )
    {
        cout<<"  ";
    }

    cout
    <<
    node->getType()
    <<
    "\n";

    //--------------------------------
    // Binary
    //--------------------------------

    if(auto b=
    dynamic_cast<BinaryNode*>(node))
    {
        printAST(
            b->left,
            depth+1
        );

        printAST(
            b->right,
            depth+1
        );
    }

    //--------------------------------
    // Program
    //--------------------------------

    if(auto p=
    dynamic_cast<ProgramNode*>(node))
    {
        for(auto s:
        p->statements)
        {
            printAST(
                s,
                depth+1
            );
        }
    }

    //--------------------------------
    // Block
    //--------------------------------

    if(auto b=
    dynamic_cast<BlockNode*>(node))
    {
        for(auto s:
        b->statements)
        {
            printAST(
                s,
                depth+1
            );
        }
    }

    //--------------------------------
    // If
    //--------------------------------

    if(auto i=
    dynamic_cast<IfNode*>(node))
    {
        printAST(
            i->condition,
            depth+1
        );

        printAST(
            i->body,
            depth+1
        );
    }

    //--------------------------------
    // While
    //--------------------------------

    if(auto w=
    dynamic_cast<WhileNode*>(node))
    {
        printAST(
            w->condition,
            depth+1
        );

        printAST(
            w->body,
            depth+1
        );
    }

    //--------------------------------
    // Function
    //--------------------------------

    if(auto f=
    dynamic_cast<FunctionNode*>(node))
    {
        printAST(
            f->body,
            depth+1
        );
    }
}
ASTNode* ASTParser::build(
    vector<string>& tokens,
    int l,
    int r
)

{
    if(l>r)
        return nullptr;

// STEP 5: function call detection
if(l < r && tokens[l+1] == "(")
{
   vector<ASTNode*> args;

CallNode* node = new CallNode(tokens[l], args);
    node->name = tokens[l];

    int i = l + 2;
    int start = i;
    int depth = 0;

    for(; i <= r; i++)
    {
        if(tokens[i] == "(") depth++;
        if(tokens[i] == ")") depth--;

        if((tokens[i] == "," && depth == 0) || i == r)
        {
            args.push_back(build(tokens, start, i - 1));
            start = i + 1;
        }
    }

    node->args = args;
    return node;
}
    // remove outer brackets
    // STEP 1: unary operators
if(l == r - 1)
{
    if(tokens[l] == "-" )
    {
        ASTNode* child = build(tokens, l + 1, r);
        return new UnaryNode("-", child);
    }

    if(tokens[l] == "!")
    {
        ASTNode* child = build(tokens, l + 1, r);
        return new UnaryNode("!", child);
    }
}
    if(
       tokens[l]=="(" &&
       tokens[r]==")"
    )
    {
        return build(
            tokens,
            l+1,
            r-1
        );
    }


    // single token

    if(l==r)
    {
        string t=tokens[l];

       bool isNumber = true;
bool dot = false;
int start = (t[0] == '-') ? 1 : 0;

for(int i = start; i < t.size(); i++)
{
    if(t[i] == '.')
    {
        if(dot) isNumber = false;
        dot = true;
    }
    else if(!isdigit(t[i]))
    {
        isNumber = false;
    }
}

if(isNumber)
{
    return new NumberNode(stod(t));
}
        return new VariableNode(
            t
        );
    }



    // find weakest operator

    int depth=0;

    int best=-1;



    for(
        int i=r;
        i>=l;
        i--
    )
    {
        if(tokens[i]==")")
        {
            depth++;
            continue;
        }

        if(tokens[i]=="(")
        {
            depth--;
            continue;
        }

        if(depth!=0)
            continue;


        if(
            isOperator(
                tokens[i]
            )
        )
        {
            if(
               best==-1 ||

               precedence(
               tokens[i]
               )

               <

               precedence(
               tokens[best]
               )
            )
            {
                best=i;
            }
        }
    }



    if(best==-1)
    {
        return nullptr;
    }


    ASTNode* left=

    build(
        tokens,
        l,
        best-1
    );



    ASTNode* right=

    build(
        tokens,
        best+1,
        r
    );


    return new BinaryNode(

        tokens[best],

        left,

        right
    );
}
//=====================================
// STEP 4
// Parse whole program
//=====================================

ProgramNode*
ASTParser::
parseProgram(
vector<string>& tokens
)
{
    ProgramNode* p=
    new ProgramNode();

    vector<string> current;

    for(auto &t:tokens)
    {
        //---------------------------------
        // statement ended
        //---------------------------------

        if(t==";")
        {
            StatementNode* s=

            parseStatement(
                current
            );

            if(s!=nullptr)
            {
                p->statements.
                push_back(s);
            }

            current.clear();

            continue;
        }

        current.push_back(t);
    }

    //---------------------------------
    // last statement
    //---------------------------------

    if(!current.empty())
    {
        StatementNode* s=

        parseStatement(
            current
        );

        if(s)
        {
            p->statements.
            push_back(s);
        }
    }

    return p;
}
//=====================================
// STEP 5
// Parse { ... } block
//=====================================

BlockNode*
ASTParser::
parseBlock(
vector<string>& tokens
)
{
    BlockNode* b=
    new BlockNode();

    vector<string> current;

    int depth=0;

    for(auto &t:tokens)
    {
        //---------------------------------
        // skip outer {
        //---------------------------------

        if(t=="{")
        {
            depth++;

            if(depth==1)
                continue;
        }

        //---------------------------------
        // skip outer }
        //---------------------------------

        if(t=="}")
        {
            depth--;

            if(depth==0)
                continue;
        }

        //---------------------------------
        // statement end
        //---------------------------------

        if(t==";" && depth==1)
        {
            StatementNode* s=

            parseStatement(
                current
            );

            if(s)
            {
                b->statements.
                push_back(s);
            }

            current.clear();

            continue;
        }

        current.push_back(t);
    }

    return b;
}
//#include "ast.hpp"

//=====================================
// STEP 3
// Parse one statement
//=====================================

StatementNode*
ASTParser::
parseStatement(
vector<string>& t
)
{
    if(t.empty())
        return nullptr;

    //---------------------------------
    // let: int x = 5
    //---------------------------------

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
            parse(
                t,
                4,
                t.size()-1
            );
        }

        return
        new DeclareNode(
            type,
            name,
            value
        );
    }

    //---------------------------------
    // x = x+1
    //---------------------------------

    if(
        t.size()>2 &&
        t[1]=="="
    )
    {
        string name=t[0];

        ASTNode* expr=

        parse(
            t,
            2,
            t.size()-1
        );

        return
        new AssignNode(
            name,
            expr
        );
    }

    //---------------------------------
    // op: x+5
    //---------------------------------
    if(t[0]=="op:")
    {
        ASTNode* expr=

        parse(
            t,
            1,
            t.size()-1
        );

        return
        new OutputNode(
            expr
        );
    }

    //---------------------------------
    // ip: x
    //---------------------------------

    if(
        t[0]=="ip:"
        &&
        t.size()>1
    )
    {
        return
        new InputNode(
            t[1]
        );
    }

    //---------------------------------
    // return x
    //---------------------------------

    if(t[0]=="return")
    {
        ASTNode* expr=

        parse(
            t,
            1,
            t.size()-1
        );

        return
        new ReturnNode(
            expr
        );
    }
//---------------------------------
// if: condition { ... }
//---------------------------------

if(t[0]=="if:")
{
    IfNode* node=
    new IfNode();

    int brace=-1;

    for(int i=0;i<t.size();i++)
    {
        if(t[i]=="{")
        {
            brace=i;
            break;
        }
    }

    node->condition=

    parse(
        t,
        1,
        brace-1
    );

    vector<string> blockTokens(
        t.begin()+brace,
        t.end()
    );

    node->body=

    parseBlock(
        blockTokens
    );

    node->elseBody=nullptr;

    return node;
}



//---------------------------------
// while: condition { ... }
//---------------------------------

if(t[0]=="while:")
{
    WhileNode* node=
    new WhileNode();

    int brace=-1;

    for(int i=0;i<t.size();i++)
    {
        if(t[i]=="{")
        {
            brace=i;
            break;
        }
    }

    node->condition=

    parse(
        t,
        1,
        brace-1
    );

    vector<string> blockTokens(
        t.begin()+brace,
        t.end()
    );

    node->body=

    parseBlock(
        blockTokens
    );

    return node;
}
//---------------------------------
// for: init ; cond ; step { ... }
//---------------------------------

if(t[0]=="for:")
{
    ForNode* node=
    new ForNode();

    int s1=-1;
    int s2=-1;
    int brace=-1;

    for(int i=0;i<t.size();i++)
    {
        if(t[i]==";")
        {
            if(s1==-1)
                s1=i;
            else
            {
                s2=i;
                break;
            }
        }
    }

    for(int i=0;i<t.size();i++)
    {
        if(t[i]=="{")
        {
            brace=i;
            break;
        }
    }

    vector<string> initTokens(
        t.begin()+1,
        t.begin()+s1
    );

    vector<string> condTokens(
        t.begin()+s1+1,
        t.begin()+s2
    );

    vector<string> stepTokens(
        t.begin()+s2+1,
        t.begin()+brace
    );

    vector<string> blockTokens(
        t.begin()+brace,
        t.end()
    );

    node->init=
    parseStatement(initTokens);

    node->cond=
    parse(
        condTokens,
        0,
        condTokens.size()-1
    );

    node->step=
    parseStatement(stepTokens);

    node->body=
    parseBlock(blockTokens);

    return node;
}
//---------------------------------
// if(condition){...}
//---------------------------------

if(t[0]=="if")
{
    IfNode* node =
    new IfNode();

    int start=-1;
    int end=-1;

    for(int i=0;i<t.size();i++)
    {
        if(t[i]=="(" && start==-1)
            start=i;

        if(t[i]==")")
            end=i;
    }

    node->condition=

    parse(
        t,
        start+1,
        end-1
    );

    vector<string> bodyTokens(

        t.begin()+end+1,
        t.end()

    );

    node->body=

    parseBlock(
        bodyTokens
    );

    node->elseBody=
    nullptr;

    return node;
}
//---------------------------------
// while(condition){...}
//---------------------------------

if(t[0]=="while")
{
    WhileNode* node=
    new WhileNode();

    int start=-1;
    int end=-1;

    for(int i=0;i<t.size();i++)
    {
        if(t[i]=="(" && start==-1)
            start=i;

        if(t[i]==")")
            end=i;
    }

    node->condition=

    parse(
        t,
        start+1,
        end-1
    );

    vector<string> bodyTokens(

        t.begin()+end+1,
        t.end()

    );

    node->body=

    parseBlock(
        bodyTokens
    );

    return node;
}
//---------------------------------
// func add(a,b){}
//---------------------------------

if(t[0]=="func")
{
    FunctionNode* f=
    new FunctionNode();

    f->name=t[1];

    int start=-1;
    int end=-1;

    for(int i=0;i<t.size();i++)
    {
        if(t[i]=="(")
            start=i;

        if(t[i]==")")
            end=i;
    }

    for(
        int i=start+1;
        i<end;
        i++
    )
    {
        if(t[i]!=",")
        {
            f->params.
            push_back(t[i]);
        }
    }

    vector<string> body(

        t.begin()+end+1,
        t.end()

    );

    f->body=

    parseBlock(
        body
    );

    return f;
}
    return nullptr;
}