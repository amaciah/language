
from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Any, Tuple, Union

# ----- TOKENS -----

DIGITS = '0123456789'

TT_INT = 'INT'
TT_FLT = 'FLOAT'
TT_ADD = 'PLUS'
TT_SUB = 'MINUS'
TT_MUL = 'STAR'
TT_DIV = 'SLASH'
TT_MOD = 'PERCENT'
TT_POW = 'CARET'
TT_LPA = 'LPAREN'
TT_RPA = 'RPAREN'
TT_EOF = 'EOF'

class Token:
    def __init__(self, position: Tuple[int, int], tok_type: str, value: str = None) -> None:
        self.pos = position
        self.type = tok_type
        self.value = value

    def get_next_position(self) -> Tuple[int, int]:
        line, col = self.pos
        col += len(self.value) if self.value else 1
        return line, col

    def __repr__(self) -> str:
        if self.value:
            return f"{self.type}:{self.value}"
        return f"{self.type}"
    

# ----- TYPES -----

@dataclass
class DataType(ABC):
    type: str
    value: Any

    def __repr__(self) -> str:
        return f"{self.value}"

    @abstractmethod
    def promote(self, type: str) -> Union["DataType", None]:
        pass

class Int(DataType):
    def __init__(self, value: int) -> None:
        super().__init__(type=TT_INT, value=value)

    def promote(self, type: str) -> DataType | None:
        if type == TT_FLT:
            return Float(float(self.value))
        
        return None

class Float(DataType):
    def __init__(self, value: float) -> None:
        super().__init__(type=TT_FLT, value=value)

    def promote(self, type: str) -> DataType | None:
        if type == TT_FLT:
            return self
        
        return None

class TypePromotion:
    priorities = [
        TT_INT,
        TT_FLT,
    ]

    @classmethod
    def max(cls, type1: str, type2: str) -> str:
        return cls.priorities[max(cls.priorities.index(type1), cls.priorities.index(type2))]
    

# ----- NODES -----

class ASTNode:
    def __init__(self, node_type: str, position: Tuple[int, int]) -> None:
        self.type = node_type
        self.pos = position

    def __repr__(self) -> str:
        pass

class NumberNode(ASTNode):
    def __init__(self, number: Token) -> None:
        super().__init__(number.type, number.pos)
        self.number = number

    def __repr__(self) -> str:
        return f"{self.number}"
    
class UnOpNode(ASTNode):
    def __init__(self, sign: Token, value: ASTNode) -> None:
        super().__init__(value.type, sign.pos)
        self.sign = sign
        self.value = value

    def __repr__(self) -> str:
        return f"(SIGN:{self.sign}, {self.value})"

class BinOpNode(ASTNode):
    def __init__(self, op: Token, left: ASTNode, right: ASTNode) -> None:
        super().__init__(left.type, left.pos)
        self.op = op
        self.left = left
        self.right = right
        self.infer_type()

    def __repr__(self) -> str:
        return f"{self.op}({self.left}, {self.right})"
    
    def infer_type(self) -> None:
        if self.left.type != self.right.type:
            self.type = TypePromotion.max(self.left.type, self.right.type)
        if self.op.type == TT_DIV:
            self.type = TT_FLT


# ----- ERRORS -----

class Error:
    def __init__(self, position: Tuple[int, int], details: str) -> None:
        self.type = "Error"
        self.pos = position
        self.details = details

    def __repr__(self) -> str:
        return f"{self.type} at line {self.pos[0]}, column {self.pos[1]}: {self.details}"
    
class IllegalCharError(Error):
    def __init__(self, position: Tuple[int, int], details: str) -> None:
        super().__init__(position, details)
        self.type = "Illegal character"

class InvalidSyntaxError(Error):
    def __init__(self, position: Tuple[int, int], details: str) -> None:
        super().__init__(position, details)
        self.type = "Invalid syntax"

class RuntimeError(Error):
    def __init__(self, position: Tuple[int, int], details: str) -> None:
        super().__init__(position, details)
        self.type = "Runtime error"
