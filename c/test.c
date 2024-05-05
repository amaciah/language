#include "lexer.h"
#include "parser.h"

int main()
{
    char text[100];
    while (1)
    {
        printf("mpl > ");
        fgets(text, sizeof(text), stdin);
        text[strlen(text) - 1] = '\0';

        Lexer l = new_lexer(text);
        LexerResult lr = tokenize(&l);

        if (lr.tokens == NULL)
        {
            print_error(l.err);
        }

        else
        {
            Parser p = new_parser(lr);
            ParserResult pr = parse(&p);

            if (pr.root == NULL)
            {
                print_error(pr.err);
            }

            else
            {
                print_node(pr.root);
                printf("\n");
                free_node(pr.root);
            }
        }

        free_lexer_result(&lr);
    }
}
