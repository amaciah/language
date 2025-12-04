#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "base.h"

// ----- INTERPRETER -----

/**
 * Contains information for the interpretation of an Abstract Syntax Tree
 */
typedef struct interpreter
{
    const ASTNode* ast;
} Interpreter;

/**
 * Contains the result of the interpretation of an AST
 */
typedef struct interpreter_result
{
    DataType* result;
    Error err;
} Result;

/**
 * Creates and initializes an interpreter for an AST
 * 
 * @param ast The root of the AST
 * 
 * @return The new interpreter
*/
Interpreter new_interpreter(const ASTNode* ast);

/**
 * Interprets the AST
 * 
 * @param i The interpreter
 * 
 * @return The result of the interpretation
 * 
 * @note In case of error, the ```result``` field is ```NULL```
 * and the ```err``` field contains the error
 */
Result interpret(Interpreter* i);

/**
 * Interprets an AST node
 * 
 * @return The result of the interpretation
 * 
 * 
 * @note In case of error, the ```result``` field is ```NULL```
 * and the ```err``` field contains the error
 */
Result visit(const ASTNode* node);

#endif  // INTERPRETER_H
