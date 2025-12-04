# Language

## About
I am writing a small programming language for fun, with the idea of both being a learning experience and a potentially useful tool in the future

## The language
Two equivalent implementations, in C and Python, are provided. Python serves as a pseudocode/brainstorm tool, whereas C should offer a better performance in more complex programs.

## Syntax
The current syntax as a context-free grammar can be found in grammar.txt. At its current state, the language supports real math, with int and float data types supported

## Project structure
Regardless of the implementation, the structure follows a similar pattern:

- **Base:** Common data types and definitions required between modules.
- **Lexer:** Receives the code in the implemented language and performs the lexical analysis, detecting each token supported by the language and returning a list of tokens as a result.
- **Parser:** Receives the list of tokens from the previous step and performs the syntactical analysis, based on the syntax defined as a CFG. Returns an Abstract Syntax Tree (AST).
- **Interpreter:** Receives the AST of a program and evaluates each node until a final expression is obtained. It is implemented directly in the target language (Python or C).
- **Console:** Offers a console interface to be able to use the language from command line.

## Future work
- **Compiler:** It is possible to generate Assembly code from the AST in a similar structure to the interpreter's. A compiled language usually offers a better performance.
- **Transpilers:** Similarly, code in any other language can be generated from the AST to obtain a portable, cross-platform program.
- **Custom Assembly Language:** I consider defining a custom machine code set, with a potential transpilation process to obtain platform-specific instructions.
