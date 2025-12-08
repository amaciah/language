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
                f"Unable to interpret node: Type {type(node)} unknown"
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
        
        if node.sign.type == TT_ADD:
            return self.pos(value, node)
        if node.sign.type == TT_SUB:
            return self.neg(value, node)

        return value, None
    
    def visit_BinOpNode(self, node: BinOpNode) -> Tuple[DataType, Error]:
        left, err = self.visit_node_with_promotion(node.left, node.type)
        if err:
            return None, err

        right, err = self.visit_node_with_promotion(node.right, node.type)
        if err:
            return None, err
        
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
        if node.op.type == TT_POW:
            return self.pow(left, right, node)
        
        return None, RuntimeError(
            node.pos,
            f"Unknown operator: {node.op.type}"
        )
        
    def pos(self, value: DataType, node: UnOpNode) -> Tuple[DataType, Error]:
        if node.type in (TT_INT, TT_FLT):
            value.value = +value.value
            return value, None
        
        return None, RuntimeError(
            node.pos,
            f"No positive method defined for type {node.type}"
        )
    
    def neg(self, value: DataType, node: UnOpNode) -> Tuple[DataType, Error]:
        if node.type in (TT_INT, TT_FLT):
            value.value = -value.value
            return value, None

        return None, RuntimeError(
            node.pos,
            f"No negative method defined for type {node.type}"
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
    
    def pow(self, left: DataType, right: DataType, node: BinOpNode) -> Tuple[DataType, Error]:
        if node.type == TT_INT:
            return Int(left.value ** right.value), None
        if node.type == TT_FLT:
            return Float(left.value ** right.value), None
        
        return None, RuntimeError(
            node.pos,
            f"No power method defined for type {node.type}"
        )

    def isZero(self, value: DataType) -> bool:
        return value.value == 0
    
    def visit_node_with_promotion(self, node: ASTNode, type: str) -> Tuple[DataType, Error]:
        value, err = self.visit(node)
        if err:
            return None, err
        
        if node.type != type:
            value = value.promote(type)
            if value is None:
                return None, RuntimeError(
                    node.pos,
                    f"Unable to convert from {node.type} to {type}"
                )
        
        return value, None
        