#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

char* strip(char* str)
{
    int i = 0, j = 0;

    while (str[i] == ' ' || str[i] == '\t')
        i++;
    
    while (str[i] != ' ' && str[i] != '\t' && str[i] != '\0')
        str[j++] = str[i++];

    str[j] = '\0';

    return str;
}

char* lower(char* str)
{
    int i;

    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 'a' - 'A';
    }

    return str;
}

int main()
{
    char text[100], aux[100];
    printf("Type 'q' or 'Quit' to quit.\n");
    while (1)
    {
        printf("mc > ");
        fgets(text, sizeof(text), stdin);
        text[strlen(text) - 1] = '\0';

        lower(strip(strcpy(aux, text)));
        if (strcmp(aux, "q") == 0 || strcmp(aux, "quit") == 0)
            break;

        Lexer l = new_lexer(text);
        LexerResult lr = tokenize(&l);

        if (lr.tokens == NULL)
        {
            print_error(l.err);
            free_lexer_result(&lr);
            continue;
        }

        Parser p = new_parser(lr);
        ParserResult pr = parse(&p);

        if (pr.root == NULL)
        {
            print_error(pr.err);
            free_lexer_result(&lr);
            continue;
        }

        Interpreter i = new_interpreter(pr.root);
        Result r = interpret(&i);

        if (r.result == NULL)
        {
            print_error(r.err);
            free_lexer_result(&lr);
            free_node(pr.root);
            continue;
        }

        print_value(r.result);
        printf("\n");

        free_lexer_result(&lr);
        free_node(pr.root);
    }
}
