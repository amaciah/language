from lexer import Lexer
from parser_ import Parser
from interpreter import Interpreter

print("Type 'q' or 'Quit' to quit.")
while True:
    text = input("mc > ")
    if text.strip().lower() in ('q', 'quit'):
        break
    tokens, err = Lexer(text).tokenize()
    if err:
        print(err)
        continue
    # print("Tokens:", tokens)
    ast, err = Parser(tokens).parse()
    if err:
        print(err)
        continue
    # print("Abstract Syntax Tree:", ast)
    result, err = Interpreter(ast).interpret()
    print(err if err else result)
