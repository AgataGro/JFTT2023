#ifndef AST_NODES_HPP
#define AST_NODES_HPP

#include <memory>
#include <vector>
#include <string>

class AstNodeVisitor;

// Base class for all AST nodes
class AstNode {
public:
    virtual ~AstNode() = default;
    virtual void print(int level = 0) const = 0;
    virtual void accept(AstNodeVisitor* visitor)=0;
};

class ExpressionNode : public AstNode {
public:
    virtual ~ExpressionNode() = default;
};

class ValueNode : public ExpressionNode {
public:
    long long value;  // Assuming the value is a long long. Adjust the type as needed.

    // Constructor
    ValueNode(long long val);

    virtual ~ValueNode() = default;

    // Implement any virtual methods from ExpressionNode, such as a print method.
    virtual void print(int level) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class IdentifierNode : public ExpressionNode {
public:
    std::string* name;
    ExpressionNode * index;

    IdentifierNode(std::string* idName);
    IdentifierNode(std::string* idName, ExpressionNode * idx);
    virtual void print(int level = 0) const override;
    virtual ~IdentifierNode() = default;

    bool isArrayAccess() const;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class BinaryExpressionNode : public ExpressionNode {
public:
    ExpressionNode * leftOperand;
    ExpressionNode * rightOperand;
    std::string opr;

    BinaryExpressionNode(ExpressionNode * left, ExpressionNode * right, std::string op);
    virtual ~BinaryExpressionNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class ConditionNode : public AstNode {
public:
    ExpressionNode * leftOperand;
    ExpressionNode * rightOperand;
    std::string opr;

    ConditionNode(ExpressionNode * left, ExpressionNode * right, std::string op);
    virtual ~ConditionNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};
class DeclarationNode : public AstNode {
public:
    IdentifierNode * identifier;
    long long arraySize;

    DeclarationNode(IdentifierNode * id, long long size = -1);
    virtual ~DeclarationNode() = default;
    virtual void print(int level = 0) const override;

    bool isArray() const;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class CommandNode : public AstNode {
public:
    virtual ~CommandNode() = default;
};

class CommandBlockNode :  public CommandNode{
public:
    std::vector<CommandNode *> body;
    CommandBlockNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class ReadNode : public CommandNode {
public:
    IdentifierNode * identifier;

    ReadNode(IdentifierNode * id);
    virtual ~ReadNode() = default;

    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class WriteNode : public CommandNode {
public:
    ExpressionNode * expression;

    WriteNode(ExpressionNode * expr);
    virtual ~WriteNode() = default;

    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class ProcedureCallArguments : public AstNode{
public:
    std::vector<ExpressionNode *> args;
    ProcedureCallArguments() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class ProcedureCallNode : public CommandNode {
public:
    std::string* procedureName; // Name of the procedure being called
    ProcedureCallArguments* arguments; // Arguments passed to the procedure

    ProcedureCallNode(std::string* procName, ProcedureCallArguments* args);
    virtual ~ProcedureCallNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class RepeatUntilNode : public CommandNode {
public:
    CommandBlockNode* commands; // Commands to execute in the loop
    ConditionNode * condition; // The condition that ends the loop

    RepeatUntilNode(ConditionNode * cond, CommandBlockNode* body);
    virtual ~RepeatUntilNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class WhileNode : public CommandNode {
public:
    ConditionNode * condition; // The condition for the while loop
    CommandBlockNode* commands; // Commands to execute in the loop

    WhileNode(ConditionNode * cond, CommandBlockNode* body);
    virtual ~WhileNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class IfNode : public CommandNode {
public:
    ConditionNode * condition; // Adjusted to use ConditionNode
    CommandBlockNode* thenCommands;
    CommandBlockNode* elseCommands;

    IfNode(ConditionNode * cond, CommandBlockNode* ifTrue, CommandBlockNode* ifFalse);
    virtual ~IfNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class AssignmentNode : public CommandNode {
public:
    IdentifierNode * identifier;  // Variable to which value is assigned
    ExpressionNode * expression;  // Expression being assigned

    AssignmentNode(IdentifierNode * id, ExpressionNode * expr);
    virtual ~AssignmentNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class DeclarationsNode : public AstNode {
public:
    std::vector<DeclarationNode *> declarations;
    DeclarationsNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class MainNode : public AstNode {
public:
    DeclarationsNode *decl;
    CommandBlockNode *body;

    MainNode(DeclarationsNode * decl_node, CommandBlockNode * cmd);
    virtual ~MainNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class ArgumentNode : public AstNode {
public:
    std::string* argumentName;
    bool isArray;

    ArgumentNode(std::string* argName, bool arrayFlag = false);
    virtual ~ArgumentNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class ArgumentsNode : public AstNode {
public:
    std::vector<ArgumentNode *> arguments;
    ArgumentsNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class ProcedureHeadNode : public AstNode{
public:
    std::string* name;
    ArgumentsNode* arguments;
    ProcedureHeadNode(std::string* proc_name, ArgumentsNode *args);
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class ProcedureNode : public AstNode {
public:
    ProcedureHeadNode *arguments;
    DeclarationsNode* declarations;
    CommandBlockNode* commands;

    ProcedureNode(ProcedureHeadNode* args,DeclarationsNode* decl,CommandBlockNode* body);
    virtual ~ProcedureNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class ProceduresNode : public AstNode {
public:
    std::vector<ProcedureNode *> procedures;

    ProceduresNode() = default;
    virtual ~ProceduresNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;
};

class ProgramNode : public AstNode {
public:
    ProceduresNode * procedures;
    MainNode * mainBlock;

    ProgramNode();
    virtual ~ProgramNode() = default;
    virtual void print(int level = 0) const override;
    virtual void accept(AstNodeVisitor* visitor) override;

    void addProcedures(ProceduresNode * proc);
    void setMainBlock(MainNode * main);
};

class AstNodeVisitor {
public:
    virtual void visit(ValueNode* node) = 0;
    virtual void visit(IdentifierNode* node) = 0;
    virtual void visit(BinaryExpressionNode* node) = 0;
    virtual void visit(ConditionNode* node) = 0;
    virtual void visit(DeclarationNode* node) = 0;
    virtual void visit(CommandBlockNode* node) = 0;
    virtual void visit(ReadNode* node) = 0;
    virtual void visit(WriteNode* node) = 0;
    virtual void visit(ProcedureCallArguments* node) = 0;
    virtual void visit(ProcedureCallNode* node) = 0;
    virtual void visit(RepeatUntilNode* node) = 0;
    virtual void visit(WhileNode* node) = 0;
    virtual void visit(IfNode* node) = 0;
    virtual void visit(AssignmentNode* node) = 0;
    virtual void visit(DeclarationsNode* node) = 0;
    virtual void visit(MainNode* node) = 0;
    virtual void visit(ArgumentNode* node) = 0;
    virtual void visit(ArgumentsNode* node) = 0;
    virtual void visit(ProcedureHeadNode* node) = 0;
    virtual void visit(ProcedureNode* node) = 0;
    virtual void visit(ProceduresNode* node) = 0;
    virtual void visit(ProgramNode* node) = 0;
};

#endif // AST_NODES_HPP
