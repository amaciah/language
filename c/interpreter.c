#include "interpreter.h"

// ----- INTERPRETER -----

// Auxiliary functions

int isZero(const DataType* value)
{
    switch (value->type)
    {
    case INT:
        return value->value.integer == 0;

    case FLOAT:
        return fabs(value->value.decimal) < 1e-9;

    default:
        return 1;
    }
}

Result visit_node_with_promotion(const ASTNode* node, TypePriority type)
{
    Result res;

    res = visit(node);
    if (res.result == NULL)
        return res;
    
    if (node->type != type)
    {
        res.result = promote(res.result, type);
        if (res.result == NULL)
        {
            char details[MAX_ERR_DET_LEN];
            sprintf(
                details,
                "Unable to convert from %s to %s",
                get_type_representation(node->type),
                get_type_representation(type)
            );
            res.err = new_error(
                RuntimeError,
                node->pos,
                details
            );
        }
    }

    return res;
}


// Private function declarations

Result visit_NumberNode(const ASTNode* node);

Result visit_UnOpNode(const ASTNode* node);

Result visit_BinOpNode(const ASTNode* node);

// Unary operators

Result pos(const DataType* value, const ASTNode* node);

Result neg(const DataType* value, const ASTNode* node);

// Binary operators

Result add(const DataType* left, const DataType* right, const ASTNode* node);

Result sub(const DataType* left, const DataType* right, const ASTNode* node);

Result mul(const DataType* left, const DataType* right, const ASTNode* node);

Result div_(const DataType* left, const DataType* right, const ASTNode* node);

Result mod(const DataType* left, const DataType* right, const ASTNode* node);

Result pow_(const DataType* left, const DataType* right, const ASTNode* node);


// Public functions

Interpreter new_interpreter(const ASTNode* ast)
{
    Interpreter i = { ast };
    return i;
}

Result interpret(Interpreter* i)
{
    return visit(i->ast);
}

Result visit(const ASTNode* node)
{
    Result res;

    switch (node->class)
    {
    case Number:
        return visit_NumberNode(node);

    case UnOp:
        return visit_UnOpNode(node);

    case BinOp:
        return visit_BinOpNode(node);
    
    default:
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            "Unable to interpret node: Type unknown"
        );
        return res;
    }
}


// Private function implementations

Result visit_NumberNode(const ASTNode* node)
{
    Result res;

    switch (node->data.number.value->type)
    {
    case TT_INT:
        res.result = new_int(atoi(node->data.number.value->value));
        break;

    case TT_FLT:
        res.result = new_float(atof(node->data.number.value->value));
        break;
    
    default:
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            "Unable to interpret node: Not a valid number type"
        );
        break;
    }

    return res;
}

Result visit_UnOpNode(const ASTNode* node)
{
    Result res, res_signed;

    res = visit(node->data.unary.value);
    if (res.result == NULL)
        return res;

    switch (node->data.unary.sign->type)
    {
    case TT_ADD:
        res_signed = pos(res.result, node);
        break;

    case TT_SUB:
        res_signed = neg(res.result, node);
        break;
    
    default:
        return res;  // Should not happen
    }

    free_value(res.result);
    return res_signed;
}

Result visit_BinOpNode(const ASTNode* node)
{
    Result left, right, res;

    left = visit_node_with_promotion(node->data.binary.left, node->type);
    if (left.result == NULL)
        return left;
    
    right = visit_node_with_promotion(node->data.binary.right, node->type);
    if (right.result == NULL)
    {
        free_value(left.result);
        return right;
    }

    switch (node->data.binary.op->type)
    {
    case TT_ADD:
        res = add(left.result, right.result, node);
        break;
    
    case TT_SUB:
        res = sub(left.result, right.result, node);
        break;

    case TT_MUL:
        res = mul(left.result, right.result, node);
        break;

    case TT_DIV:
        res = div_(left.result, right.result, node);
        break;

    case TT_MOD:
        res = mod(left.result, right.result, node);
        break;

    case TT_POW:
        res = pow_(left.result, right.result, node);
        break;

    default:
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            "Unable to interpret node: Unknown operator"
        );
        break;
    }
    
    free_value(left.result);
    free_value(right.result);
    return res;
}

Result pos(const DataType* value, const ASTNode* node)
{
    Result res;

    switch (node->type)
    {
    case INT:
        res.result = new_int(+value->value.integer);
        break;

    case FLOAT:
        res.result = new_float(+value->value.decimal);
        break;

    default:
        char details[MAX_ERR_DET_LEN];
        sprintf(
            details, 
            "No positive method defined for type %s", 
            get_type_representation(node->type)
        );
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            details
        );
        break;
    }

    return res;
}

Result neg(const DataType* value, const ASTNode* node)
{
    Result res;

    switch (node->type)
    {
    case INT:
        res.result = new_int(-value->value.integer);
        break;

    case FLOAT:
        res.result = new_float(-value->value.decimal);
        break;

    default:
        char details[MAX_ERR_DET_LEN];
        sprintf(
            details, 
            "No negative method defined for type %s", 
            get_type_representation(node->type)
        );
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            details
        );
        break;
    }

    return res;
}

Result add(const DataType* left, const DataType* right, const ASTNode* node)
{
    Result res;

    switch (node->type)
    {
    case INT:
        res.result = new_int(left->value.integer + right->value.integer);
        break;

    case FLOAT:
        res.result = new_float(left->value.decimal + right->value.decimal);
        break;

    default:
        char details[MAX_ERR_DET_LEN];
        sprintf(
            details, 
            "No addition method defined for type %s", 
            get_type_representation(node->type)
        );
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            details
        );
        break;
    }

    return res;
}

Result sub(const DataType* left, const DataType* right, const ASTNode* node)
{
    Result res;

    switch (node->type)
    {
    case INT:
        res.result = new_int(left->value.integer - right->value.integer);
        break;

    case FLOAT:
        res.result = new_float(left->value.decimal - right->value.decimal);
        break;

    default:
        char details[MAX_ERR_DET_LEN];
        sprintf(
            details, 
            "No subtraction method defined for type %s", 
            get_type_representation(node->type)
        );
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            details
        );
        break;
    }

    return res;
}

Result mul(const DataType* left, const DataType* right, const ASTNode* node)
{
    Result res;

    switch (node->type)
    {
    case INT:
        res.result = new_int(left->value.integer * right->value.integer);
        break;

    case FLOAT:
        res.result = new_float(left->value.decimal * right->value.decimal);
        break;

    default:
        char details[MAX_ERR_DET_LEN];
        sprintf(
            details, 
            "No multiplication method defined for type %s", 
            get_type_representation(node->type)
        );
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            details
        );
        break;
    }

    return res;
}

Result div_(const DataType* left, const DataType* right, const ASTNode* node)
{
    Result res;

    if (isZero(right))
    {
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            "Division by 0"
        );
        return res;
    }

    switch (node->type)
    {
    case INT:
        res.result = new_int(left->value.integer / right->value.integer);
        break;

    case FLOAT:
        res.result = new_float(left->value.decimal / right->value.decimal);
        break;

    default:
        char details[MAX_ERR_DET_LEN];
        sprintf(
            details, 
            "No division method defined for type %s", 
            get_type_representation(node->type)
        );
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            details
        );
        break;
    }

    return res;
}

Result mod(const DataType* left, const DataType* right, const ASTNode* node)
{
    Result res;

    if (isZero(right))
    {
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            "Division by 0"
        );
        return res;
    }

    switch (node->type)
    {
    case INT:
        res.result = new_int(left->value.integer % right->value.integer);
        break;

    case FLOAT:
        res.result = new_float(remainder(left->value.decimal, right->value.decimal));
        break;

    default:
        char details[MAX_ERR_DET_LEN];
        sprintf(
            details, 
            "No module method defined for type %s", 
            get_type_representation(node->type)
        );
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            details
        );
        break;
    }

    return res;
}

Result pow_(const DataType* left, const DataType* right, const ASTNode* node)
{
    Result res;

    switch (node->type)
    {
    case INT:
        res.result = new_int(pow(left->value.integer, right->value.integer));
        break;

    case FLOAT:
        res.result = new_float(pow(left->value.decimal, right->value.decimal));
        break;

    default:
        char details[MAX_ERR_DET_LEN];
        sprintf(
            details, 
            "No power method defined for type %s", 
            get_type_representation(node->type)
        );
        res.result = NULL;
        res.err = new_error(
            RuntimeError,
            node->pos,
            details
        );
        break;
    }

    return res;
}
