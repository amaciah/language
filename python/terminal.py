from lexer import Lexer
from parser_ import Parser
from interpreter import Interpreter

while True:
    text = input("mpl > ")
    tokens, err = Lexer(text).tokenize()
    if err:
        print(err)
        continue
    # print(tokens)
    ast, err = Parser(tokens).parse()
    if err:
        print(err)
        continue
    # print(ast)
    result, err = Interpreter(ast).interpret()
    print(err if err else result)
