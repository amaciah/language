from base import *

# ----- INTERPRETER -----

class Interpreter:
    def __init__(self, ast: ASTNode) -> None:
        self.ast = ast

    def interpret(self) -> Tuple[DataType, Error]:
        """
        Interpret the AST
        """
        return self.visit(self.ast)

    def visit(self, node: ASTNode) -> Tuple[DataType, Error]:
        if type(node) is NumberNode:
            return self.visit_NumberNode(node)
        elif type(node) is UnOpNode:
            return self.visit_UnOpNode(node)
        elif type(node) is BinOpNode:
            return self.visit_BinOpNode(node)
        else:
            return None, RuntimeError(
                node.pos,
                f"No visit method defined for node of type {node.type}"
            )
        
    def visit_NumberNode(self, node: NumberNode) -> Tuple[DataType, Error]:
        if node.number.type == TT_INT:
            return Int(int(node.number.value)), None
        elif node.number.type == TT_FLT:
            return Float(float(node.number.value)), None
        else:
            return None, RuntimeError(
                node.pos,
                f"{node.number.type} is not a valid number type"
            )
        
    def visit_UnOpNode(self, node: UnOpNode) -> Tuple[DataType, Error]:
        value, err = self.visit(node.value)
        if err:
            return None, err
        
        if node.sign.type == TT_SUB:
            value.value = -value.value

        return value, None
    
    def visit_BinOpNode(self, node: BinOpNode) -> Tuple[DataType, Error]:

        left, err = self.visit(node.left)
        if err:
            return None, err
        
        if node.left.type != node.type:
            left = left.promote(node.type)
            if left is None:
                return None, RuntimeError(
                    node.pos,
                    f"Unable to convert from {node.left.type} to {node.type}"
                )

        right, err = self.visit(node.right)
        if err:
            return None, err
        
        if node.right.type != node.type:
            right = right.promote(node.type)
            if right is None:
                return None, RuntimeError(
                    node.pos,
                    f"Unable to convert from {node.right.type} to {node.type}"
                )
        
        if node.op.type == TT_ADD:
            return self.add(left, right, node)
        if node.op.type == TT_SUB:
            return self.sub(left, right, node)
        if node.op.type == TT_MUL:
            return self.mul(left, right, node)
        if node.op.type == TT_DIV:
            return self.div(left, right, node)
        if node.op.type == TT_MOD:
            return self.mod(left, right, node)
        
        return None, RuntimeError(
            node.pos,
            f"Unknown operator: {node.op.type}"
        )
        
    def add(self, left: DataType, right: DataType, node: BinOpNode) -> Tuple[DataType, Error]:
        if node.type == TT_INT:
            return Int(left.value + right.value), None
        if node.type == TT_FLT:
            return Float(left.value + right.value), None
        
        return None, RuntimeError(
            node.pos,
            f"No addition method defined for type {node.type}"
        )
        
    def sub(self, left: DataType, right: DataType, node: BinOpNode) -> Tuple[DataType, Error]:
        if node.type == TT_INT:
            return Int(left.value - right.value), None
        if node.type == TT_FLT:
            return Float(left.value - right.value), None
        
        return None, RuntimeError(
            node.pos,
            f"No subtraction method defined for type {node.type}"
        )
        
    def mul(self, left: DataType, right: DataType, node: BinOpNode) -> Tuple[DataType, Error]:
        if node.type == TT_INT:
            return Int(left.value * right.value), None
        if node.type == TT_FLT:
            return Float(left.value * right.value), None
        
        return None, RuntimeError(
            node.pos,
            f"No multiplication method defined for type {node.type}"
        )
        
    def div(self, left: DataType, right: DataType, node: BinOpNode) -> Tuple[DataType, Error]:
        if self.isZero(right):
            return None, RuntimeError(
                node.pos,
                "Division by 0"
            )

        if node.type == TT_INT:
            return Int(round(left.value / right.value)), None
        if node.type == TT_FLT:
            return Float(left.value / right.value), None
        
        return None, RuntimeError(
            node.pos,
            f"No division method defined for type {node.type}"
        )
    
    def mod(self, left: DataType, right: DataType, node: BinOpNode) -> Tuple[DataType, Error]:
        if self.isZero(right):
            return None, RuntimeError(
                node.pos,
                "Division by 0"
            )

        if node.type == TT_INT:
            return Int(left.value % right.value), None
        if node.type == TT_FLT:
            return Float(left.value % right.value), None
        
        return None, RuntimeError(
            node.pos,
            f"No module method defined for type {node.type}"
        )

    def isZero(self, value: DataType) -> bool:
        return value.value == 0
