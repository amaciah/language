#include "parser.h"

// ----- PARSER -----

// Auxiliary functions

/**
 * Consumes a binary operation:
 * 
 * ```bino ::= func { ( op1 | op2 | ... ) func }```
 * 
 * @param p The parser
 * @param func The rule function of the operands
 * @param ops The valid operators for the rule
 * @param n_ops The number of valid operators
 * 
 * @return The result of parsing the rule
 * 
 * @note In case of error, the ```root``` field is ```NULL```
 * and the ```err``` field contains the error
*/
ParserResult bin_op(Parser* p, ParserResult (*func)(Parser*), TokenType ops[], int n_ops)
{
    ParserResult res;
    int f_current_tok_in_ops = 1;   // The current token is a valid operator

    // Consume left node
    res = func(p);
    if (res.root == NULL)
        return res;

    while (p->current && f_current_tok_in_ops)
    {
        // Reset flag
        f_current_tok_in_ops = 0;

        for (int i = 0; i < n_ops; i++)
        {
            if (p->current->type == ops[i])
            {
                // Set flag
                f_current_tok_in_ops = 1;

                // Consume operator
                const Token* op = p->current;
                if (advance_parser(p) == NULL)
                {
                    free_node(res.root);
                    res.root = NULL;
                    res.err = new_error(
                        InvalidSyntaxError,
                        get_next_position(op),
                        "Expected another number"
                    );
                    return res;
                }

                // Consume right node
                ParserResult right = func(p);
                if (right.root == NULL)
                {
                    free_node(res.root);
                    return right;
                }

                // Build binary node
                res.root = new_bin_op_node(op, res.root, right.root);
                break;
            }
        }
    }
    
    // Correct exit
    return res;
}


// Public functions

Parser new_parser(const LexerResult res)
{
    Parser p = {res.tokens, res.size, -1, NULL};
    advance_parser(&p);
    return p;
}

const Token* advance_parser(Parser* p)
{
    (p->idx)++;
    p->current = (p->idx < p->tok_count) ? p->tok_list[p->idx] : NULL;
    return p->current;
}

ParserResult parse(Parser* p)
{
    return prog(p);
}

ParserResult prog(Parser* p)
{
    // Consume an expression
    ParserResult res = expr(p);
    if (res.root == NULL)
        return res;

    // End of program reached
    if (p->current == NULL)
        return res;

    // Unexpected token
    free_node(res.root);
    res.root = NULL;
    res.err = new_error(
        InvalidSyntaxError,
        p->current->pos,
        "Unexpected token"
    );
}

ParserResult expr(Parser* p)
{
    // Consume binary operation
    TokenType ops[] = {TT_ADD, TT_SUB};
    return bin_op(p, term, ops, 2);
}

ParserResult term(Parser* p)
{
    // Consume binary operation
    TokenType ops[] = {TT_MUL, TT_DIV, TT_MOD};
    return bin_op(p, fact, ops, 3);
}

ParserResult fact(Parser* p)
{
    // '(' expr ')'
    if (p->current->type == TT_LPA)
    {
        ParserResult res;

        // Consume left parenthesis
        Position pos = get_next_position(p->current);
        if (advance_parser(p) == NULL)
        {
            res.root = NULL;
            res.err = new_error(
                InvalidSyntaxError,
                pos,
                "Expected expression"
            );
            return res;
        }

        // Consume expression
        res = expr(p);
        if (res.root == NULL)
            return res;

        // Consume right parenthesis
        if (p->current->type != TT_RPA)
        {
            free_node(res.root);
            res.root = NULL;
            res.err = new_error(
                InvalidSyntaxError,
                p->current->pos,
                "Expected ')'"
            );
            return res;
        }
        advance_parser(p);

        // Correct exit
        return res;
    }

    // Consume signed value
    if (p->current->type == TT_ADD || p->current->type == TT_SUB)
    {
        // Consume sign
        const Token* sign = NULL;
        if (p->current->type == TT_ADD || p->current->type == TT_SUB)
        {
            sign = p->current;
            if (advance_parser(p) == NULL)
            {
                res.root = NULL;
                res.err = new_error(
                    InvalidSyntaxError,
                    get_next_position(sign),
                    "Expected number"
                );
                return res;
            }
        }
    }
}

ParserResult sval(Parser* p)
{
    ParserResult res;

    // Consume sign
    const Token* sign = NULL;
    if (p->current->type == TT_ADD || p->current->type == TT_SUB)
    {
        sign = p->current;
        if (advance_parser(p) == NULL)
        {
            res.root = NULL;
            res.err = new_error(
                InvalidSyntaxError,
                get_next_position(sign),
                "Expected number"
            );
            return res;
        }
    }

    // Consume numeric value
    res = nval(p);
    if (res.root == NULL)
        return res;

    // Correct exit
    if (sign) {
        NumberNode* nn = (NumberNode*) res.root;
        nn->sign = sign;
    }
    return res;
}

ParserResult nval(Parser* p)
{
    ParserResult res;

    // Consume integer or float token
    if (p->current->type == TT_INT || p->current->type == TT_FLT)
    {
        ASTNode* node = new_number_node(p->current, NULL);
        advance_parser(p);

        res.root = node;
        return res;
    }

    // Invalid token
    res.root = NULL;
    res.err = new_error(
        InvalidSyntaxError,
        p->current->pos,
        "Expected number"
    );
    return res;
}
