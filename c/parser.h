#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// ----- PARSER -----

/**
 * Contains information for the parsing of a list of tokens
 */
typedef struct parser
{
    const Token** tok_list;
    int tok_count;
    int idx;
    const Token* current;
} Parser;

/**
 * Contains the result of a syntax analysis
*/
typedef struct parser_result
{
    ASTNode* root;
    Error err;
} ParserResult;

/**
 * Creates and initializes a parser with the list of tokens to parse
 * 
 * @param res The result obtained from a lexical analysis
 * 
 * @return The new parser
*/
Parser new_parser(const LexerResult res);

/**
 * Consumes a token from the list
 * 
 * @param p The parser
 */
const Token* advance_parser(Parser* p);

/**
 * Performs a syntax analysis of the parser's list of tokens
 * 
 * @param p The parser
 * 
 * @return The result of the analysis
 * 
 * @note In case of error, the ```root``` field is ```NULL```
 * and the ```err``` field contains the error
 */
ParserResult parse(Parser* p);

#endif  // PARSER_H
