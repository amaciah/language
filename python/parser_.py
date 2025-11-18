
from typing import List, Callable
from base import *


# ----- PARSER -----

class Parser:
    def __init__(self, tokens: List[Token]) -> None:
        self.tokens = tokens
        self.idx = -1
        self.current_tok = None
        self.advance()

    def advance(self) -> Token:
        self.idx += 1
        self.current_tok = self.tokens[self.idx] if self.idx < len(self.tokens) else None
        return self.current_tok

    def parse(self) -> Tuple[ASTNode, Error]:
        """
        Parse the program
        """
        return self.prog()
    
    def prog(self) -> Tuple[ASTNode, Error]:
        """
        Consume the program (root of the grammar)

        `prog ::= expr`
        """

        # Consume an expression
        expr, err = self.expr()
        if err:
            return None, err
        
        # End of program reached
        if self.current_tok == None:
            return expr, None
        
        # Unexpected token
        return None, InvalidSyntaxError(
            self.current_tok.pos,
            f"Unexpected token '{self.current_tok}'"
        )
    
    def expr(self) -> Tuple[ASTNode, Error]:
        """
        Consume a math expression

        `expr ::= term { ('+' | '-') term }`
        """
        # Consume binary operation
        return self._bin_op(self.term, (TT_ADD, TT_SUB))
    
    def term(self) -> Tuple[ASTNode, Error]:
        """
        Consume a math term

        `term ::= fact { ('*' | '/' | '%') fact }`
        """
        # Consume binary operation
        return self._bin_op(self.fact, (TT_MUL, TT_DIV, TT_MOD))

    def fact(self) -> Tuple[ASTNode, Error]:
        """
        Consume a math factor

        `fact ::= '(' expr ')' | ( '+' | '-' ) fact | nval`
        """

        # '(' expr ')'
        if self.current_tok.type == TT_LPA:

            # Consume left parenthesis
            pos = self.current_tok.get_next_position()
            if self.advance() == None:
                return None, InvalidSyntaxError(
                    pos,
                    "Expected expression"
                )

            # Consume expression
            expr, err = self.expr()
            if err:
                return None, err
            
            # Consume right parenthesis
            if self.current_tok.type != TT_RPA:
                return None, InvalidSyntaxError(
                    self.current_tok.pos,
                    "Expected ')'"
                )
            self.advance()

            # Correct exit
            return expr, None
        
        # Consume signed value
        if self.current_tok.type in (TT_ADD, TT_SUB):

            # Consume sign
            sign = self.current_tok
            if self.advance() == None:
                return None, InvalidSyntaxError(
                    sign.get_next_position(),
                    "Expected expression"
                )
            
            # Consume factor
            node, err = self.fact()
            if err:
                return None, err
            
            # Correct exit
            return UnOpNode(sign, node), None
        
        # Consume numeric value
        return self.nval()
    
    def nval(self) -> Tuple[NumberNode, Error]:
        """
        Consume a numeric value

        `nval ::= INT | FLT`
        """
        # Consume integer or float token
        if self.current_tok.type in (TT_INT, TT_FLT):
            node = NumberNode(self.current_tok)
            self.advance()

            # Correct exit
            return node, None
        
        # Invalid token
        return None, InvalidSyntaxError(
            self.current_tok.pos,
            "Expected number"
        )


    # Auxiliary methods

    def _bin_op(self, func: Callable, ops: List) -> Tuple[ASTNode, Error]:
        """
        Consume a binary operation

        `bino ::= func { ( op1 | op2 | ... ) func }`
        """
        
        # Consume left node
        left, err = func()
        if err:
            return None, err       

        while self.current_tok and self.current_tok.type in ops:

            # Consume operator
            op = self.current_tok
            if self.advance() == None:
                return None, InvalidSyntaxError(
                    op.get_next_position(),
                    f"Expected another number"
                )

            # Consume right node
            right, err = func()
            if err:
                return None, err
            
            # Build binary node
            left = BinOpNode(op, left, right)

        # Correct exit
        return left, None
