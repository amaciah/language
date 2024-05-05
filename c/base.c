#include "base.h"

// ----- TOKENS -----

/**
 * Provides a string representation for each token type
 */
const char* TokenRepr[] = {

    // Number literals
    "INT",
    "FLOAT",

    // Operators
    "PLUS",
    "MINUS",
    "STAR",
    "SLASH",
    "PERCENT",
    "CARET",

    // Parentheses
    "LPAREN",
    "RPAREN",
};

const Token* new_token(Position pos, TokenType type, const char* value)
{
    Token* t = (Token*) malloc(sizeof(Token));
    t->pos = pos;
    t->type = type;
    strncpy(t->value, value ? value : "", MAX_TOK_VAL_LEN);
    return t;
}

int print_token(const Token* t)
{
    int i = printf("%s", TokenRepr[t->type]);
    if (strlen(t->value))
        i += printf(":%s", t->value);
    return i;
}

Position get_next_position(const Token* t)
{
    Position p = t->pos;
    if (strcmp(t->value, ""))
        p.col++;
    else
        p.col += strlen(t->value);
    return p;
}

void free_token(Token* t)
{
    free(t);
}


// ----- NODES -----

ASTNode* new_number_node(const Token* number)
{
    NumberNode* node = (NumberNode*) malloc(sizeof(NumberNode));
    node->base.type = Number;
    node->number = number;
    return (ASTNode*) node;
}

ASTNode* new_un_op_node(ASTNode* value, const Token* sign)
{
    UnOpNode* node = (UnOpNode*) malloc(sizeof(UnOpNode));
    node->base.type = UnOp;
    node->value = value;
    node->sign = sign;
    return (ASTNode*) node;
}

ASTNode* new_bin_op_node(const Token* op, ASTNode* left, ASTNode* right)
{
    BinOpNode* node = (BinOpNode*) malloc(sizeof(BinOpNode));
    node->base.type = BinOp;
    node->op = op;
    node->left = left;
    node->right = right;
    return (ASTNode*) node;
}

int print_node(const ASTNode* node)
{
    NumberNode* nn = NULL;
    UnOpNode* nu = NULL;
    BinOpNode* nb = NULL;

    int i = 0;
    switch (node->type)
    {
        case Number:
            nn = (NumberNode*) node;
            return print_token(nn->number);
            break;

        case UnOp:
            nu = (UnOpNode*) node;
            i += printf("(SIGN:");
            i += print_token(nu->sign);
            i += printf(", ");
            i += print_node(nu->value);
            i += printf(")");
            return i;
            break;

        case BinOp:
            nb = (BinOpNode*) node;
            i += print_token(nb->op);
            i += printf("(");
            i += print_node(nb->left);
            i += printf(", ");
            i += print_node(nb->right);
            i += printf(")");
            return i;
            break;
        
        default:
            return -1;
            break;
    }
}

void free_node(ASTNode* node)
{
    NumberNode* nn = NULL;
    UnOpNode* nu = NULL;
    BinOpNode* nb = NULL;

    switch (node->type)
    {
        case Number:
            nn = (NumberNode*) node;
            free(nn);
            break;

        case UnOp:
            nu = (UnOpNode*) node;
            free_node(nu->value);
            free(nu);
            break;

        case BinOp:
            nb = (BinOpNode*) node;
            free_node(nb->left);
            free_node(nb->right);
            free(nb);
            break;

        default:
            break;
    }
}


// ----- ERRORS -----

/**
 * Provides a string representation for each error type
 */
const char* ErrorRepr[] = {
    "Illegal character",
    "Invalid syntax",
    "Runtime error"
};

const Error new_error(ErrorType type, Position pos, const char* details)
{
    Error e;
    e.type = type;
    e.pos = pos;
    strncpy(e.details, details, MAX_ERR_DET_LEN);
    return e;
}

int print_error(const Error e)
{
    return printf("%s at line %d, column %d: %s\n", ErrorRepr[e.type], e.pos.row, e.pos.col, e.details);
}
