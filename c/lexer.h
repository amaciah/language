#ifndef LEXER_H
#define LEXER_H

#include "base.h"

// ----- LEXER -----

/**
 * Contains information for the lexical analysis of a text
 */
typedef struct lexer
{
    const char* text;
    int pos;
    int row;
    int col;
    char current;
    Error err;
} Lexer;

/**
 * Contains the result of a lexical analysis
 */
typedef struct lexer_result
{
    const Token** tokens;
    int current;
    int size;
} LexerResult;

/**
 * Creates and initializes a lexer with the text to analyze
 * 
 * @param text The text to analyze
 * 
 * @return The new lexer
 */
Lexer new_lexer(const char* text);

/**
 * Reserves memory for a lexer result of a given size
 * 
 * @param size The maximum number of tokens of the result
 * 
 * @return The new lexer result
 * 
 * @note Remember to call ```free_lexer_result()``` afterwards
 */
LexerResult new_lexer_result(int size);

/**
 * Returns an "error result" containing no tokens
 * 
 * @return The error lexer result
 */
LexerResult error_lexer_result();

/**
 * Finalizes a lexer result, removing unused memory
 * 
 * @param r The lexer result to trim
 * 
 * @note Tokens can no longer be appended to the result
 */
void trim_lexer_result(LexerResult* r);

/**
 * Appends a token to a lexer result
 * 
 * @param r The lexer result
 * @param t The token to append
 */
void append_token_to_result(LexerResult* r, const Token* t);

/**
 * Frees the memory used by a lexer result
 * 
 * @param r The lexer result
 */
void free_lexer_result(LexerResult* r);

/**
 * Prepares the lexer to process the next character from the text
 * 
 * @param l The lexer
 */
void advance_lexer(Lexer* l);

/**
 * Obtains the position of the current character being processed by the lexer
 * 
 * @param l The lexer
 * 
 * @return The character's position (row, column)
 */
Position get_current_pos(const Lexer* l);

/**
 * Obtains a number token starting from the current position of the text
 * 
 * @param l The lexer
 * 
 * @return The number token on success, or ```NULL``` in case of error
 * 
 * @note Sets ```l.err``` in case of error
 */
const Token* get_number(Lexer* l);

/**
 * Performs a lexical analysis of the lexer's text
 * 
 * @param l The lexer
 * 
 * @return The result of the analysis, or an "error result" in case of error
 * 
 * @note An error result's ```tokens``` field is ```NULL```
 * @note Sets ```l.err``` in case of error
 */
LexerResult tokenize(Lexer* l);

#endif  // LEXER_H
