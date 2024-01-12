#include "ast_nodes.hpp"
#include <string>
#include <iostream>
#include <iterator>

std::string createIndent(int level) {
    return std::string(level * 4, ' '); // Four spaces per level of indentation
}

void AstNode::print(int level) const {
    std::cout << "empty\n";
}

ValueNode::ValueNode(long long val) : value(val) {}
void ValueNode::print(int level) const {
        std::string indent(level * 4, ' ');  // Four spaces per level of indentation
        std::cout << indent << "Value: " << value << std::endl;
    }

// IdentifierNode definitions
IdentifierNode::IdentifierNode(std::string* idName) 
    : name(idName), index(nullptr) {}

IdentifierNode::IdentifierNode(std::string* idName, ExpressionNode * idx) 
    : name(idName), index(idx) {}

bool IdentifierNode::isArrayAccess() const {
    return index != nullptr;
}

void IdentifierNode::print(int level) const {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Identifier: " << *name;
    if (isArrayAccess()) {
        std::cout << "[";
        index->print(0); // Assuming index is an ExpressionNode
        std::cout << "]";
    }
    std::cout << std::endl;
}

// BinaryExpressionNode definitions
BinaryExpressionNode::BinaryExpressionNode(ExpressionNode * left, ExpressionNode * right, std::string op)
    : leftOperand(left), rightOperand(right), opr(op) {}

void BinaryExpressionNode::print(int level) const {
    std::cout << createIndent(level) << "Binary Expression: Operator " << opr << std::endl;
    std::cout << createIndent(level + 1) << "Left:" << std::endl;
    leftOperand->print(level + 2);
    std::cout << createIndent(level + 1) << "Right:" << std::endl;
    rightOperand->print(level + 2);
}

// ConditionNode definitions
ConditionNode::ConditionNode(ExpressionNode * left, ExpressionNode * right, std::string op)
    : leftOperand(left), rightOperand(right), opr(op) {}

void ConditionNode::print(int level) const {
    std::cout << createIndent(level) << "Condition: Operator " << opr << std::endl;
    std::cout << createIndent(level + 1) << "Left:" << std::endl;
    leftOperand->print(level + 2);
    std::cout << createIndent(level + 1) << "Right:" << std::endl;
    rightOperand->print(level + 2);
}

void DeclarationsNode::print(int level) const {
    std::cout << createIndent(level) << "Declarations: " << std::endl;
    for (const auto& d : declarations) {
        d->print(level + 1);
    }
}

// DeclarationNode definitions
DeclarationNode::DeclarationNode(IdentifierNode * id, long long size)
    : identifier(id), arraySize(size) {}

bool DeclarationNode::isArray() const {
    return arraySize != -1;
}

void DeclarationNode::print(int level) const {
    std::cout << createIndent(level) << "Declaration:" << std::endl;
    identifier->print(level + 1);
    if (arraySize != -1) {
        std::cout << createIndent(level + 1) << "Array Size: " << arraySize << std::endl;
    }
}

ReadNode::ReadNode(IdentifierNode * id)
    : identifier(id) {}

void ReadNode::print(int level) const {
    std::cout << createIndent(level) << "Read:" << std::endl;
    identifier->print(level + 1);
} 

WriteNode::WriteNode(ExpressionNode * expr)
    : expression(expr) {}

void WriteNode::print(int level) const {
    std::cout << createIndent(level) << "Write:" << std::endl;
    expression->print(level + 1);
} 

void ProcedureCallArguments::print(int level) const {
    std::cout << createIndent(level) << "ProcedureCallArguments:" << std::endl;
    for (const auto& c : args) {
         c->print(level + 1);
    }
} 

void ProcedureHeadNode::print(int level) const {
    std::cout << createIndent(level) << "ProcedureHeadNode: " << *name << std::endl;
    arguments->print(level + 1);
} 

// ProcedureCallNode definitions
ProcedureCallNode::ProcedureCallNode(std::string* procName, ProcedureCallArguments* args)
    : procedureName(procName), arguments(args) {}

void ProcedureCallNode::print(int level) const {
    std::cout << createIndent(level) << "Procedure Call: " << *procedureName << std::endl;
    arguments->print(level + 1);
}

void CommandBlockNode::print(int level) const {
    std::cout << createIndent(level) << "CommandBlock:" << std::endl;
    for (const auto& c : body) {
         c->print(level + 1);
    }
}

// RepeatUntilNode definitions
RepeatUntilNode::RepeatUntilNode(ConditionNode * cond, CommandBlockNode* body)
    : condition(cond), commands(body) {}

void RepeatUntilNode::print(int level) const {
    std::cout << createIndent(level) << "Repeat-Until Loop:" << std::endl;
    std::cout << createIndent(level + 1) << "Commands:" << std::endl;
    commands->print(level + 2);
    std::cout << createIndent(level + 1) << "Until Condition:" << std::endl;
    condition->print(level + 2);
}

// WhileNode definitions
WhileNode::WhileNode(ConditionNode * cond, CommandBlockNode* body)
    : condition(cond), commands(body) {}

void WhileNode::print(int level) const {
    std::cout << createIndent(level) << "While Loop:" << std::endl;
    std::cout << createIndent(level + 1) << "Condition:" << std::endl;
    condition->print(level + 2);

    std::cout << createIndent(level + 1) << "Commands:" << std::endl;
    commands->print(level + 2);
}

// IfNode definitions
IfNode::IfNode(ConditionNode * cond, CommandBlockNode* ifTrue, CommandBlockNode* ifFalse)
    : condition(cond), thenCommands(ifTrue), elseCommands(ifFalse) {}

void IfNode::print(int level) const {
    std::cout << createIndent(level) << "If Statement:" << std::endl;
    std::cout << createIndent(level + 1) << "Condition:" << std::endl;
    condition->print(level + 2);
    std::cout << createIndent(level + 1) << "Then Commands:" << std::endl;
    thenCommands->print(level + 2);
    if (elseCommands != nullptr) {
        std::cout << createIndent(level + 1) << "Else Commands:" << std::endl;
        elseCommands->print(level + 2);
    }
}

// AssignmentNode definitions
AssignmentNode::AssignmentNode(IdentifierNode * id, ExpressionNode * expr)
    : identifier(id), expression(expr) {}

void AssignmentNode::print(int level) const {
    std::cout << createIndent(level) << "Assignment:" << std::endl;
    std::cout << createIndent(level + 1) << "Variable:" << std::endl;
    identifier->print(level + 2);
    std::cout << createIndent(level + 1) << "Expression:" << std::endl;
    expression->print(level + 2);
}

// MainNode definitions
MainNode::MainNode(DeclarationsNode * decl_node, CommandBlockNode * cmd)
    : decl(decl_node), body(cmd) {}

void MainNode::print(int level) const {
    std::cout << createIndent(level) << "Main Node:" << std::endl;
    decl->print(level + 1);
    body->print(level + 1);
}

ProcedureHeadNode::ProcedureHeadNode(std::string* proc_name, ArgumentsNode *args)
    : name(proc_name), arguments(args) {}

ProcedureNode::ProcedureNode(ProcedureHeadNode* args, DeclarationsNode* decl, CommandBlockNode* body)
    : arguments(args), declarations(decl), commands(body) {}

void ProcedureNode::print(int level) const {
    std::cout << createIndent(level) << "Procedure: " << std::endl;
    arguments->print(level + 1);
    declarations->print(level + 1);
    commands->print(level + 1);

}

void ProceduresNode::print(int level) const { 
    std::cout << createIndent(level) << "Procedures Node:" << std::endl;
    for (const auto& pr : procedures) {
        pr->print(level + 1);
    }
}

void ArgumentsNode::print(int level) const { 
    std::cout << createIndent(level) << "Arguments Node:" << std::endl;
    for (const auto& a : arguments) {
        a->print(level + 1);
    }
}

// ArgumentNode definitions
ArgumentNode::ArgumentNode(std::string* argName, bool arrayFlag)
    : argumentName(argName), isArray(arrayFlag) {}

void ArgumentNode::print(int level) const {
    std::cout << createIndent(level) << "Argument: " << *argumentName;
    if (isArray) {
        std::cout << " (Array)";
    }
    std::cout << std::endl;
}

// ProgramNode definitions
ProgramNode::ProgramNode() {}

void ProgramNode::addProcedures(ProceduresNode * proc) {
    procedures = proc;
}

void ProgramNode::setMainBlock(MainNode * main) {
    mainBlock = main;
}

void ProgramNode::print(int level) const {
    std::cout << createIndent(level) << "Program Node:" << std::endl;
    if (procedures) {
        procedures->print(level + 1);
    }
    if (mainBlock) {
        mainBlock->print(level + 1);
    }
}

void ProgramNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void ValueNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void IdentifierNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void BinaryExpressionNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void ConditionNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void DeclarationNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void CommandBlockNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void ReadNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void WriteNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void ProcedureCallArguments::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void ProcedureCallNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void ProcedureHeadNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void WhileNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void RepeatUntilNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void IfNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void AssignmentNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void DeclarationsNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void MainNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void ArgumentNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void ArgumentsNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void ProcedureNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

void ProceduresNode::accept(AstNodeVisitor *visitor) {
    visitor->visit(this);
}

