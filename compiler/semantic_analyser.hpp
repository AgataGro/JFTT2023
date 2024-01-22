#ifndef SEMANTIC_ANALYSER_HPP
#define SEMANTIC_ANALYSER_HPP

#include <unordered_map>
#include <string>
#include "ast_nodes.hpp"

struct SymbolTableEntry {
    std::string name;
    long long memoryLocation;
    std::string scope;
    long long arraySize; // Use -1 for non-array variables
    bool initialized = false;
};

struct ArgumentEntry
{
    std::string name;
    std::string scope;
    long long memoryLocation;
    long long isArray;
};

struct InitializedArgument
{
    ArgumentEntry a;
    SymbolTableEntry s;
};

struct InitializedVariable {
    std::string name;
    std::string scope;
};

class SymbolTable {
public:
    std::unordered_map<std::string, SymbolTableEntry> symbols;
    std::unordered_map<std::string, ArgumentEntry> arguments;
    std::unordered_map<std::string, InitializedArgument> initArguments;
    std::unordered_map<std::string, InitializedVariable> initVariables;
    std::string currentScope;
    long long nextMemoryLocation;
    SymbolTable();
    // Methods to add, find, and remove symbols, manage scope, etc.
};

class Variable {
public:
    std::string name;
    long long memoryLocation;
    Variable* offset;
    bool isValue;
    bool isMemoryReference;
    bool isArray;
    Variable(std::string name, long long memoryLocation, Variable* offset, bool isValue, bool isMemoryReference, bool isArray);
    ~Variable() = default;
    Variable* getOffset();
};

class BlockVisitor;

class Block {
public:
    Block() = default;
    virtual ~Block() = default;
    virtual void print(int level) = 0;
    virtual void accept(BlockVisitor *visitor) = 0;
};

class ParentBlock;

class ValueCommand : public Block {
public:
    Variable* val;
    ValueCommand(Variable *val);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class AddCommand : public Block {
public:
    Variable* left, *right;
    AddCommand(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class SubCommand : public Block {
public:
    Variable *left, *right;
    SubCommand(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class MulCommand : public Block {
public:
    Variable *left, *right;
    MulCommand(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class DivCommand : public Block {
public:
    Variable *left, *right;
    DivCommand(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class ModCommand : public Block {
public:
    Variable *left, *right;
    ModCommand(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class EQCondition : public Block {
public:
    Variable *left, *right;
    EQCondition(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class NEQCondition : public Block {
public:
    Variable *left, *right;
    NEQCondition(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class GEQCondition : public Block {
public:
    Variable *left, *right;
    GEQCondition(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class LEQCondition : public Block {
public:
    Variable *left, *right;
    LEQCondition(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class GTCondition : public Block {
public:
    Variable *left, *right;
    GTCondition(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class LTCondition : public Block {
public:
    Variable *left, *right;
    LTCondition(Variable *left, Variable *right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class AssignCommand : public Block {
public:
    Variable *left;
    Block* right;
    AssignCommand(Variable *left, Block* right);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class ReadCommand : public Block {
public:
    Variable* val;
    ReadCommand(Variable *val);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class WriteCommand : public Block {
public:
    Variable* val;
    WriteCommand(Variable* val);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class CallCommand : public Block {
public:
    ParentBlock *parent;
    std::string procedure_name;
    std::vector<Variable> vals;
    CallCommand(std::string name);
    void addArg(Variable val);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class IfBlock : public Block {
public:
    Block* cond;
    std::vector<Block*> thenBlock;
    std::vector<Block*> elseBlock;
    IfBlock() = default;
    void addThenBlock(Block* b);
    void addElseBlock(Block* b);
    void addCondition(Block* c);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class RepeatBlock : public Block {
public:
    ParentBlock *parent;
    Block* cond;
    std::vector<Block*> body;
    RepeatBlock() = default;
    void addBlock(Block* b);
    void addCondition(Block* c);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class WhileBlock : public Block {
public:
    ParentBlock *parent;
    Block* cond;
    std::vector<Block*> body;
    WhileBlock() = default;
    void addBlock(Block* b);
    void addCondition(Block* c);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class ContentBlock : public Block {
public:
    ParentBlock *parent;
    std::vector<Block *> body;
    ContentBlock(ParentBlock *parent);
    void addBlock(Block *b);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class ParentBlock : public Block {
public:
    std::string name;
    std::vector<Variable> vars;
    ContentBlock *content;
    ParentBlock(std::string name);
    void addContent(ContentBlock *b);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class Intermediate {
public:
    std::vector<ParentBlock *> blocks;
    Intermediate() = default;
    ~Intermediate() = default;
    void addBlock(ParentBlock *b);
    void print(int level);
    void accept(BlockVisitor *visitor);
};

class SemanticAnalyser : public AstNodeVisitor {
public: 
    SymbolTable* symbolTable;
    std::vector<ProcedureNode *> procedures;
    std::string checkedProcedure = "";
    Intermediate *intermediate_form;
    ParentBlock *parent;
    Block *currentBlock;
    Variable* ready_var;
    bool onlyGatherProcedureNames = true;
    SemanticAnalyser();
    virtual void visit(ValueNode* node) override;
    virtual void visit(IdentifierNode* node) override;
    virtual void visit(BinaryExpressionNode* node) override;
    virtual void visit(ConditionNode* node) override;
    virtual void visit(DeclarationNode* node) override;
    virtual void visit(CommandBlockNode* node) override;
    virtual void visit(ReadNode* node) override;
    virtual void visit(WriteNode* node) override;
    virtual void visit(ProcedureCallArguments* node) override;
    virtual void visit(ProcedureCallNode* node) override;
    virtual void visit(RepeatUntilNode* node) override;
    virtual void visit(WhileNode* node) override;
    virtual void visit(IfNode* node) override;
    virtual void visit(AssignmentNode* node) override;
    virtual void visit(DeclarationsNode* node) override;
    virtual void visit(MainNode* node) override;
    virtual void visit(ArgumentNode* node) override;
    virtual void visit(ArgumentsNode* node) override;
    virtual void visit(ProcedureHeadNode* node) override;
    virtual void visit(ProcedureNode* node) override;
    virtual void visit(ProceduresNode* node) override;
    virtual void visit(ProgramNode* node) override;
    bool containsEntryWith(std::string scope, std::string name);
    std::vector<ArgumentEntry> getProcedureArguments(std::string name);
};

class BlockVisitor {
public:
    Intermediate *itermediate_form;
    BlockVisitor() = default;
    ~BlockVisitor() = default;
    virtual void visit(ValueCommand* node) = 0;
    virtual void visit(AddCommand* node) = 0;
    virtual void visit(SubCommand* node) = 0;
    virtual void visit(MulCommand* node) = 0;
    virtual void visit(DivCommand* node) = 0;
    virtual void visit(ModCommand* node) = 0;
    virtual void visit(EQCondition* node) = 0;
    virtual void visit(NEQCondition* node) = 0;
    virtual void visit(GEQCondition* node) = 0;
    virtual void visit(LEQCondition* node) = 0;
    virtual void visit(GTCondition* node) = 0;
    virtual void visit(LTCondition* node) = 0;
    virtual void visit(AssignCommand* node) = 0;
    virtual void visit(ReadCommand* node) = 0;
    virtual void visit(WriteCommand* node) = 0;
    virtual void visit(CallCommand* node) = 0;
    virtual void visit(IfBlock* node) = 0;
    virtual void visit(RepeatBlock* node) = 0;
    virtual void visit(WhileBlock* node) = 0;
    virtual void visit(ParentBlock* node) = 0;
    virtual void visit(ContentBlock* node) = 0;
    virtual void visit(Intermediate* node) = 0;
};

#endif // SEMANTIC_ANALYSER_HPP
