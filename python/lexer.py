
from typing import List, Tuple
from base import *
    

# ----- LEXER -----

class Lexer:
    def __init__(self, text: str) -> None:
        self.text = text
        self.pos = -1
        self.row = 1
        self.col = 0
        self.current_char = None
        self.advance()

    def advance(self) -> None:
        self.pos += 1
        if self.current_char == '\n':
            self.row += 1
            self.col = 0
        self.col += 1

        self.current_char = self.text[self.pos] if self.pos < len(self.text) else None

    def get_current_pos(self) -> Tuple[int, int]:
        return (self.row, self.col)

    def get_number(self) -> Token:
        value = ''
        dot_count = 0

        while self.current_char != None and self.current_char in DIGITS + '.':
            if self.current_char == '.':
                dot_count += 1
            value += self.current_char
            self.advance()

        if dot_count == 0:
            return Token(self.get_current_pos(), TT_INT, value)
        elif dot_count == 1:
            return Token(self.get_current_pos(), TT_FLT, value)
        else:
            return None

    def tokenize(self) -> Tuple[List[Token], Error]:
        tokens = []

        while self.current_char != None:

            # Ignore whitespace
            if self.current_char in ' \t':
                self.advance()

            # Update row and column on newline
            elif self.current_char == '\n':
                self.advance()

            # Single-character tokens

            elif self.current_char == '+':
                tokens.append(Token(self.get_current_pos(), TT_ADD))
                self.advance()

            elif self.current_char == '-':
                tokens.append(Token(self.get_current_pos(), TT_SUB))
                self.advance()

            elif self.current_char == '*':
                tokens.append(Token(self.get_current_pos(), TT_MUL))
                self.advance()

            elif self.current_char == '/':
                tokens.append(Token(self.get_current_pos(), TT_DIV))
                self.advance()

            elif self.current_char == '%':
                tokens.append(Token(self.get_current_pos(), TT_MOD))
                self.advance()

            elif self.current_char == '^':
                tokens.append(Token(self.get_current_pos(), TT_POW))
                self.advance()

            elif self.current_char == '(':
                tokens.append(Token(self.get_current_pos(), TT_LPA))
                self.advance()

            elif self.current_char == ')':
                tokens.append(Token(self.get_current_pos(), TT_RPA))
                self.advance()

            # Literals

            # Numbers
            elif self.current_char in DIGITS:
                tok = self.get_number()
                if not tok:
                    return None, IllegalCharError(self.get_current_pos(), "Not a valid number format")
                tokens.append(tok)

            else:
                return None, IllegalCharError(self.get_current_pos(), f"Invalid character '{self.current_char}'")

        return tokens, None
