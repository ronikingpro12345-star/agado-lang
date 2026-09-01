#pragma once

#include <string>
#include <vector>

using namespace std;

//====================
// AST NODES
//====================

struct ASTNode
{
    virtual ~ASTNode() {}
    virtual string getType() const = 0;
};

struct StatementNode : ASTNode{};

struct NumberNode : ASTNode{
    double value;

    NumberNode(double v){ value=v; }

    string getType() const override { return "Number"; }
};

struct VariableNode : ASTNode{
    string name;

    VariableNode(string n){ name=n; }

    string getType() const override { return "Variable"; }
};

struct BinaryNode : ASTNode{
    string op;
    ASTNode* left;
    ASTNode* right;

    BinaryNode(string o, ASTNode* l, ASTNode* r)
        : op(o), left(l), right(r) {}

    string getType() const override { return "Binary"; }
};
struct UnaryNode : ASTNode{
    string op;
    ASTNode* child;

    UnaryNode(string o, ASTNode* c)
        : op(o), child(c) {}

    string getType() const override { return "Unary"; }
};
struct CallNode : ASTNode{
    string name;
    vector<ASTNode*> args;

    CallNode(string n, vector<ASTNode*> a)
        : name(n), args(a) {}

    string getType() const override { return "Call"; }
};
struct AssignNode : StatementNode{

    string name;
    ASTNode* value;

    AssignNode(
        string n,
        ASTNode* v
    )
    :name(n),value(v){}

    string getType() const override{
        return "Assign";
    }
};
    struct DeclareNode : StatementNode{

string type;
string name;

ASTNode* value;
    
DeclareNode(
string t,
string n,
ASTNode* v
)
:type(t),
name(n),
value(v){}

string getType() const override{
return "Declare";
}
};
struct OutputNode : StatementNode{

ASTNode* expr;

OutputNode(ASTNode* e)
:expr(e){}

string getType() const override{
return "Output";
}
};
struct ReturnNode : StatementNode{

ASTNode* expr;

ReturnNode(ASTNode* e)
:expr(e){}

string getType() const override{
return "Return";
}
};
struct BlockNode : ASTNode{

vector<StatementNode*> statements;

string getType() const override{
return "Block";
}
};
struct IfNode : StatementNode{

ASTNode* condition;

BlockNode* body;

BlockNode* elseBody;

string getType() const override{
return "If";
}
};
struct InputNode : StatementNode{

    string name;

    InputNode(
        string n
    )
    :name(n){}

    string getType() const override{
        return "Input";
    }
};
struct WhileNode : StatementNode{

    ASTNode* condition;

    BlockNode* body;

    string getType() const override{
        return "While";
    }
};
struct ProgramNode : ASTNode{

    vector<StatementNode*> statements;

    string getType() const override{
        return "Program";
    }
};
struct ForNode : StatementNode{

    StatementNode* init;

    ASTNode* cond;

    StatementNode* step;

    BlockNode* body;

    string getType() const override{
        return "For";
    }
};
struct FunctionNode : StatementNode{

    string name;

    vector<string> params;

    BlockNode* body;

    string getType() const override{
        return "Function";
    }
};
//====================
// PARSER
//====================

class ASTParser{

public:

    ASTNode* parse(
        vector<string>& tokens,
        int l,
        int r
    );
 ProgramNode* parseProgram(
    vector<string>& tokens
);

StatementNode* parseStatement(
    vector<string>& tokens
);

BlockNode* parseBlock(
    vector<string>& tokens
);
private:

    ASTNode* build(
        vector<string>& tokens,
        int l,
        int r
    );

    int precedence(
        const string& op
    );

    bool isOperator(
        const string& s
    );
};

