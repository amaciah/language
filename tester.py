from python.lexer import Lexer
from python.parser_ import Parser
from python.interpreter import Interpreter


with open('tests.txt', 'r') as file:
    lines = file.readlines()
    
fails = []
for line in lines:
    line = line.strip()
    if line:
        if line.startswith('//'):
            if 'test_count' in locals() and 'pass_count' in locals():
                print(f"\n{pass_count}/{test_count} tests passed.")
                if fails:
                    print("Failed tests:")
                    for entry, expected, result in fails:
                        print(f"[X] {entry} -> {result} (Expected: {expected})")
            fails = []
            test_count = pass_count = 0
            print(f"\n[Test] {line[2:].strip()}")
            
        else:
            entry, expected = [value.strip() for value in line.split('->')]
            
            if expected.startswith('[ERR]'):
                expected_error = expected[5:].strip()
                if ':' in expected_error:
                    expected_error = expected_error.split(':')[1].strip()
                
            tokens, err = Lexer(entry).tokenize()
            if not err:
                ast, err = Parser(tokens).parse()
                if not err:
                    result, err = Interpreter(ast).interpret()
            
            test_count += 1
            if (err and expected_error in err.__repr__()) or (not err and str(result) == expected):
                pass_count += 1
                print('.', end='')
            else:
                print('x', end='')
                fails.append((entry, expected, result if not err else err))
