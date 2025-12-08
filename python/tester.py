import math
from typing import List
from lexer import Lexer
from parser_ import Parser
from interpreter import Interpreter


TEST_FILE = '../tests.txt'


def print_test_header(name: str) -> None:
    print(f"\n[Test] {name}")
    
def print_test_summary(pass_count: int, test_count: int, fails: List) -> None:
    print(f"\n{pass_count}/{test_count} tests passed.")
    if fails:
        print("Failed tests:")
        for entry, expected, result in fails:
            print(f"[X] {entry} -> {result} (Expected: {expected})")


with open(TEST_FILE, 'r') as file:
    lines = file.readlines()
    
fails = []
for line in lines:
    line = line.strip()
    if line:
        if line.startswith('//'):
            if 'test_count' in locals() and 'pass_count' in locals():
                print_test_summary(pass_count, test_count, fails)
            fails = []
            test_count = pass_count = 0
            print_test_header(line[2:].strip())
            
        else:
            entry, expected = [value.strip() for value in line.split('->')]
            
            if expected.startswith('[ERR]'):
                expected_error = expected[5:].strip()
                if ':' in expected_error:
                    expected_error = expected_error.split(':')[1].strip()
                    
            if expected.startswith('~'):
                expected = expected[1:].strip()
                
            tokens, err = Lexer(entry).tokenize()
            if not err:
                ast, err = Parser(tokens).parse()
                if not err:
                    result, err = Interpreter(ast).interpret()
            
            test_count += 1
            if (err and 'expected_error' in locals() and expected_error in err.__repr__()) \
            or (not err and math.isclose(float(result.value), float(expected), rel_tol=1e-6)):
                pass_count += 1
                print('.', end='')
            else:
                print('x', end='')
                fails.append((entry, expected, result if not err else err))
                
print_test_summary(pass_count, test_count, fails)
