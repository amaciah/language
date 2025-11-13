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


// ----- TYPES -----

DataType* new_int(int value)
{
    DataType* data = (DataType*) malloc(sizeof(DataType));
    data->type = INT;
    data->value.integer = value;
    return data;
}

DataType* new_float(double value)
{
    DataType* data = (DataType*) malloc(sizeof(DataType));
    data->type = FLOAT;
    data->value.decimal = value;
    return data;
}

DataType* promote(DataType* data, TypePriority type)
{
    switch (data->type)
    {
    case INT:
        if (type == FLOAT)
        {
            DataType* promoted = new_float((float) data->value.integer);
            free_value(data);
            return promoted;
        }
        return NULL;

    case FLOAT:
        if (type == FLOAT)
            return data;
        return NULL;

    default:
        return NULL;
    }
}

const char* get_type_representation(TypePriority type)
{
    switch (type)
    {
    case INT:
        return "INT";

    case FLOAT:
        return "FLOAT";

    default:
        return "UNKNOWN";
    }
}

int print_value(const DataType* data)
{
    switch (data->type)
    {
    case INT:
        return printf("%d", data->value.integer);

    case FLOAT:
        return printf("%lf", data->value.decimal);

    default:
        return -1;
    }
}

void free_value(DataType* data)
{
    free(data);
}

TypePriority max_priority(TypePriority type1, TypePriority type2)
{
    return (type1 >= type2) ? type1 : type2;
}


// ----- NODES -----

ASTNode* new_number_node(const Token* number)
{
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->class = Number;
    node->type = number->type;
    node->pos = number->pos;
    node->data.number.value = number;
    return node;
}

ASTNode* new_un_op_node(const Token* sign, ASTNode* value)
{
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->class = UnOp;
    node->type = value->type;
    node->pos = sign->pos;
    node->data.unary.value = value;
    node->data.unary.sign = sign;
    return (ASTNode*) node;
}

ASTNode* new_bin_op_node(const Token* op, ASTNode* left, ASTNode* right)
{
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->class = BinOp;
    node->type = max_priority(left->type, right->type);
    node->pos = left->pos;
    node->data.binary.op = op;
    node->data.binary.left = left;
    node->data.binary.right = right;
    return node;
}

int print_node(const ASTNode* node)
{
    int i = 0;
    switch (node->class)
    {
    case Number:
        return print_token(node->data.number.value);

    case UnOp:
        i += printf("(SIGN:");
        i += print_token(node->data.unary.sign);
        i += printf(", ");
        i += print_node(node->data.unary.value);
        i += printf(")");
        return i;

    case BinOp:
        i += print_token(node->data.binary.op);
        i += printf("(");
        i += print_node(node->data.binary.left);
        i += printf(", ");
        i += print_node(node->data.binary.right);
        i += printf(")");
        return i;
    
    default:
        return -1;
    }
}

void free_node(ASTNode* node)
{
    switch (node->class)
    {
    case Number:
        free(node);
        break;

    case UnOp:
        free_node(node->data.unary.value);
        free(node);
        break;

    case BinOp:
        free_node(node->data.binary.left);
        free_node(node->data.binary.right);
        free(node);
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
    Error e = { .type = type, .pos = pos, };
    strncpy(e.details, details, MAX_ERR_DET_LEN);
    return e;
}

int print_error(const Error e)
{
    return printf("%s at line %d, column %d: %s\n", ErrorRepr[e.type],
                  e.pos.row, e.pos.col, e.details);
}
