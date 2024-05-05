#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Represents a position (row, column) on a file
 */
typedef struct position
{
    int row;
    int col;
} Position;


// ----- TOKENS -----

// Maximum lenght of token value
#define MAX_TOK_VAL_LEN 32

/**
 * Types of tokens in the language
 */
typedef enum token_type
{

    // Number literals
    TT_INT,     // INT
    TT_FLT,     // FLOAT

    // Operators
    TT_ADD,     // '+'
    TT_SUB,     // '-'
    TT_MUL,     // '*'
    TT_DIV,     // '/'
    TT_MOD,     // '%'
    TT_POW,     // '^'

    // Parentheses
    TT_LPA,     // '('
    TT_RPA,     // ')'

} TokenType;

/**
 * Contains information about a token from the language
 */
typedef struct token
{
    Position pos;
    TokenType type;
    char value[MAX_TOK_VAL_LEN];
} Token;

/**
 * Creates a new token
 * 
 * @param pos Position of the token
 * @param type Type of token
 * @param value Value of the token (for tokens that require it).
 * Set to ```NULL``` if not required
 * 
 * @return The new token
 * 
 * @note Remember to call ```free_token``` afterwards
 */
const Token* new_token(Position pos, TokenType type, const char* value);

/**
 * Prints the information of a token to ```stdout```
 * 
 * @param t The token
 * 
 * @return The number of characters printed
 */
int print_token(const Token* t);

/**
 * Obtains the position of the next token
 * 
 * @param t The current token
 * 
 * @return The position of the next token in the file
 */
Position get_next_position(const Token* t);

/**
 * Frees the memory used by a token
 * 
 * @param t The token
 */
void free_token(Token* t);


// ----- NODES -----

/**
 * Types of AST nodes
 */
typedef enum node_type {
    Number,     // Numeric value
    UnOp,       // Unary operation
    BinOp,      // Binary operation
} NodeType;

/**
 * Base type for an AST node
 */
typedef struct ast_node
{
    NodeType type;
} ASTNode;

/**
 * Contains information about a numeric node
 */
typedef struct number_node
{
    ASTNode base;
    const Token* number;
} NumberNode;

/**
 * Contains information about a unary operation node
 */
typedef struct un_op_node
{
    ASTNode base;
    const Token* sign;
    ASTNode* value;
} UnOpNode;

/**
 * Contains information about a binary operation node
 */
typedef struct bin_op_node
{
    ASTNode base;
    const Token* op;
    ASTNode* left;
    ASTNode* right;
} BinOpNode;

/**
 * Creates a new numeric node
 * 
 * @param number Token representing the number
 * 
 * @return The new node
 * 
 * @note Remember to call ```free_node``` afterwards
 */
ASTNode* new_number_node(const Token* number);

/**
 * Creates a new unary operation node
 * 
 * @param value Node containing the value
 * @param sign Token representing the sign
 * 
 * @return The new node
 * 
 * @note Remember to call ```free_node``` afterwards
 */
ASTNode* new_un_op_node(ASTNode* value, const Token* sign);

/**
 * Creates a new binary operation node
 * 
 * @param op Token representing the operation
 * @param left Node containing the left operand
 * @param right Node containing the right operand
 * 
 * @return The new node
 * 
 * @note Remember to call ```free_node``` afterwards
 */
ASTNode* new_bin_op_node(const Token* op, ASTNode* left, ASTNode* right);

/**
 * Prints the information of a node to ```stdout```
 * 
 * @param node The node
 * 
 * @return The number of characters printed
 */
int print_node(const ASTNode* node);

/**
 * Frees the memory used by a node
 * 
 * @param node The node
 */
void free_node(ASTNode* node);


// ----- ERRORS -----

/**
 * Types of errors
 */
typedef enum error_type
{
    IllegalCharError,
    InvalidSyntaxError,
    RuntimeError
} ErrorType;

// Maximum lenght of error details
#define MAX_ERR_DET_LEN 128

/**
 * Contains details of an error (type, position and details)
 */
typedef struct error
{
    ErrorType type;
    Position pos;
    char details[MAX_ERR_DET_LEN];
} Error;

/**
 * Creates a new error with the given parameters
 * 
 * @param type Type of error
 * @param pos Position where the error is found
 * @param details Details of the error
 * 
 * @return The new error
 */
const Error new_error(ErrorType type, Position pos, const char* details);

/**
 * Prints the information of an error to ```stdout```
 * 
 * @param e The error
 * 
 * @return The number of characters printed
 */
int print_error(const Error e);


#endif  // BASE_H