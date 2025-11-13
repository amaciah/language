#include "lexer.h"

// ----- LEXER -----

// Auxiliary functions

/**
 * Checks if a character is a digit
 * 
 * @param c The character
 * 
 * @return Boolean-like value
 */
int is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

/**
 * Checks whether a character is alphabetic
 * 
 * @param c The character
 * 
 * @return Boolean-like value
 */
int is_alpha(char c)
{
    return (c >= 'A' && c <= 'z');
}


// Public functions

Lexer new_lexer(const char* text)
{
    Lexer l = { .text = text, .pos = -1, .row = 1, .col = 0, .current = '\0', };
    advance_lexer(&l);
    l.err = new_error(
        IllegalCharError,
        get_current_pos(&l),
        "Unexpected end of file"
    );
    return l;
}

LexerResult new_lexer_result(Lexer l)
{
    LexerResult r;
    int size = strlen(l.text);
    r.tokens = (const Token**) malloc(size * sizeof(const Token*));
    for (int i = 0; i < size; i++)
        r.tokens[i] = NULL;
    r.current = 0;
    r.size = size;
    return r;
}

LexerResult error_lexer_result()
{
    LexerResult e = { .tokens = NULL, .current = -1, .size = -1 };
    return e;
}

void trim_lexer_result(LexerResult* r)
{
    r->tokens = (const Token**) realloc(r->tokens, r->current * sizeof(Token*));
    r->size = r->current;
}

void append_token_to_result(LexerResult* r, const Token* t)
{
    r->tokens[r->current] = t;
    (r->current)++;
}

void free_lexer_result(LexerResult* r)
{
    for (int i = 0; i < r->size; i++)
        free_token((Token*) r->tokens[i]);
    free(r->tokens);
    r->tokens = NULL;
    r->size = -1;
}

void advance_lexer(Lexer* l)
{
    (l->pos)++;
    if (l->current == '\n')
    {
        (l->row)++;
        l->col = 0;
    }
    (l->col)++;

    l->current = l->text[l->pos];
}

Position get_current_pos(const Lexer* l)
{
    Position res = { l->row, l->col };
    return res;
}

const Token* get_number(Lexer* l)
{
    char value[16];
    int dot_count = 0, i;
    Error e = new_error(
        IllegalCharError,
        get_current_pos(l),
        "Not a valid number format"
    );

    for (i = 0; is_digit(l->current) || l->current == '.'; i++)
    {
        value[i] = '\0';

        if (l->current == '.')
            dot_count++;

        value[i] = l->current;
        advance_lexer(l);
    }

    value[i] = '\0';
    
    switch (dot_count)
    {
    case 0:
        return new_token(get_current_pos(l), TT_INT, value);
        break;

    case 1:
        return new_token(get_current_pos(l), TT_FLT, value);
        break;
    
    default:
        l->err = e;
        return NULL;
        break;
    }
}

LexerResult tokenize(Lexer* l)
{
    LexerResult res = new_lexer_result(*l);

    while (l->current != '\0')
    {
        switch (l->current)
        {
        // Ignore whitespace
        case ' ':
        case '\t':
        // Update row and column on newline
        case '\n':
            advance_lexer(l);
            break;

        // Single-character tokens

        case '+':
            append_token_to_result(&res, new_token(get_current_pos(l), TT_ADD, NULL));
            advance_lexer(l);
            break;

        case '-':
            append_token_to_result(&res, new_token(get_current_pos(l), TT_SUB, NULL));
            advance_lexer(l);
            break;

        case '*':
            append_token_to_result(&res, new_token(get_current_pos(l), TT_MUL, NULL));
            advance_lexer(l);
            break;

        case '/':
            append_token_to_result(&res, new_token(get_current_pos(l), TT_DIV, NULL));
            advance_lexer(l);
            break;

        case '%':
            append_token_to_result(&res, new_token(get_current_pos(l), TT_MOD, NULL));
            advance_lexer(l);
            break;

        case '^':
            append_token_to_result(&res, new_token(get_current_pos(l), TT_POW, NULL));
            advance_lexer(l);
            break;

        case '(':
            append_token_to_result(&res, new_token(get_current_pos(l), TT_LPA, NULL));
            advance_lexer(l);
            break;

        case ')':
            append_token_to_result(&res, new_token(get_current_pos(l), TT_RPA, NULL));
            advance_lexer(l);
            break;

        // Complex tokens
        default:

            // Literals

            // Numbers
            if (is_digit(l->current))
            {
                const Token* t = get_number(l);
                if (!t)
                {
                    free_lexer_result(&res);
                    return error_lexer_result();
                }
                append_token_to_result(&res, t);
            }

            // Illegal character
            else
            {
                char details[MAX_ERR_DET_LEN];
                sprintf(details, "Invalid character '%c'", l->current);
                Error e = new_error(
                    IllegalCharError,
                    get_current_pos(l),
                    details
                );
                l->err = e;
                free_lexer_result(&res);
                return error_lexer_result();
            }

            break;
        }
    }

    trim_lexer_result(&res);
    return res;
}
