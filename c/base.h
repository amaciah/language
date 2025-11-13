#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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


// ----- TYPES -----

/**
 * Data type and priority for promotion
 */
typedef enum type_priority
{
    INT   = 0,          // Integer value
    FLOAT = 1,          // Decimal value
} TypePriority;

/**
 * Possible values for data
 */
typedef union data_value
{
    int integer;
    double decimal;
} DataValue;

/**
 * Contains information about a type from the language
 */
typedef struct data_type
{
    TypePriority type;  // Type of data
    DataValue value;
} DataType;

/**
 * Creates a new integer value
 * 
 * @param value Data value
 * 
 * @return The new value
 * 
 * @note Remember to call ```free_data``` afterwards
 */
DataType* new_int(int value);

/**
 * Creates a new decimal value
 * 
 * @param value Data value
 * 
 * @return The new value
 * 
 * @note Remember to call ```free_data``` afterwards
 */
DataType* new_float(double value);

/**
 * Promotes a data value to another type
 * 
 * @param data The data value
 * @param type The type to promote to
 * 
 * @return The promoted data value
 * 
 * @note Remember to call ```free_data``` afterwards
 * @note The resulting data value will be of type ```type```
 */
DataType* promote(DataType* data, TypePriority type);

/**
 * Obtains a string representation of a data type
 * 
 * @param type The data type
 * 
 * @return A string representing the type of data
 */
const char* get_type_representation(TypePriority type);

/**
 * Prints the value of a data type to ```stdout```
 * 
 * @param data The data value
 * 
 * @return The number of characters printed
 */
int print_value(const DataType* data);

/**
 * Frees the memory used by a data value
 * 
 * @param data The data value
 */
void free_value(DataType* data);

/**
 * Obtains the data type with higher priority
 * 
 * @param type1 The type of the first value
 * @param type2 The type of the second value
 * 
 * @return The type with higher priority
*/
TypePriority max_priority(TypePriority type1, TypePriority type2);


// ----- NODES -----

/**
 * Types of AST nodes
 */
typedef enum node_class
{
    Number,     // Numeric value
    UnOp,       // Unary operation
    BinOp,      // Binary operation
} NodeClass;

typedef struct ast_node ASTNode;

/**
 * Contains information about a numeric node
 */
typedef struct number_node
{
    const Token* value;
} NumberNode;

/**
 * Contains information about a unary operation node
 */
typedef struct un_op_node
{
    const Token* sign;
    ASTNode* value;
} UnOpNode;

/**
 * Contains information about a binary operation node
 */
typedef struct bin_op_node
{
    const Token* op;
    ASTNode* left;
    ASTNode* right;
} BinOpNode;


/**
 * Possible values for data
 */
typedef union node_data
{
    NumberNode number;
    UnOpNode unary;
    BinOpNode binary;
} NodeData;

/**
 * Base type for an AST node
 */
struct ast_node
{
    NodeClass class;
    TypePriority type;
    Position pos;
    NodeData data;
};

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
 * @param sign Token representing the sign
 * @param value Node containing the value
 * 
 * @return The new node
 * 
 * @note Remember to call ```free_node``` afterwards
 */
ASTNode* new_un_op_node(const Token* sign, ASTNode* value);

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