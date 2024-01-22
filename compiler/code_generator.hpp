#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <unordered_map>
#include <string>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include "ast_nodes.hpp"
#include "semantic_analyser.hpp"

class CodeFormater;

class CodeBlock {
public:
    std::string name="";
    long long start_index = 0;
    long long exit_index = 0;
    std::vector<CodeBlock *> body;
    CodeBlock() = default;
    virtual ~CodeBlock() = default;
    void addCodeBlock(CodeBlock *block);
    void addFront(CodeBlock *block);
};

class CodeSimple : public CodeBlock{
public:
    long long* jump;
    std::string command;
    CodeSimple(std::string c, long long *j);
    ~CodeSimple() = default;
};

class CodeGenerator : public BlockVisitor {
public:
    Intermediate *itermediate_form;
    CodeBlock *end_result = new CodeBlock();
    CodeBlock *currentBlock;
    ParentBlock *parent = nullptr;
    std::string usable_reg = "";
    long long k=1;
    bool checkmain = true;
    CodeGenerator(Intermediate* i);
    ~CodeGenerator() = default;
    virtual void visit(ValueCommand* node) override;
    virtual void visit(AddCommand* node) override;
    virtual void visit(SubCommand* node) override;
    virtual void visit(MulCommand* node) override;
    virtual void visit(DivCommand* node) override;
    virtual void visit(ModCommand* node) override;
    virtual void visit(EQCondition* node) override;
    virtual void visit(NEQCondition* node) override;
    virtual void visit(GEQCondition* node) override;
    virtual void visit(LEQCondition* node) override;
    virtual void visit(GTCondition* node) override;
    virtual void visit(LTCondition* node) override;
    virtual void visit(AssignCommand* node) override;
    virtual void visit(ReadCommand* node) override;
    virtual void visit(WriteCommand* node) override;
    virtual void visit(CallCommand* node) override;
    virtual void visit(IfBlock* node) override;
    virtual void visit(RepeatBlock* node) override;
    virtual void visit(WhileBlock* node) override;
    virtual void visit(ParentBlock* node) override;
    virtual void visit(ContentBlock* node) override;
    virtual void visit(Intermediate* node) override;
    void setLocation(Variable *var, std::string reg);
    void setARGLocation(Variable *var, std::string reg);
};

class CodeFormater {
public:
    std::vector<std::string> end_res;
    CodeBlock *program;
    CodeFormater(CodeBlock *code);
    void transform(CodeBlock *code);
};
#endif