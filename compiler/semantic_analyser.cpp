#include <unordered_map>
#include <string>
#include <iostream>
#include <stdexcept>
#include "ast_nodes.hpp"
#include "semantic_analyser.hpp"

SymbolTable::SymbolTable() {
    nextMemoryLocation = 0; 
    currentScope = "";
}

Variable::Variable(std::string name, int memoryLocation, Variable* offset, bool isValue, bool isMemoryReference, bool isArray) {
    this->name = name;
    this->memoryLocation = memoryLocation;
    this->offset = offset;
    this->isValue = isValue;
    this->isMemoryReference = isMemoryReference;
    this->isArray = isArray;
}

Variable* Variable::getOffset() {
    if(this->offset!=nullptr){
       return offset->getOffset();
    } else {
        return this;
    }
}

SemanticAnalyser::SemanticAnalyser() {
    symbolTable = new SymbolTable();
    intermediate_form = new Intermediate();
}

bool SemanticAnalyser::containsEntryWith(std::string scope, std::string name) {
    for (const auto& pair : symbolTable->symbols) {
        const SymbolTableEntry& entry = pair.second;
        if (entry.scope == scope && entry.name == name) {
            return true;
        }
    }
    for (const auto& pair : symbolTable->arguments) {
        const ArgumentEntry& entry = pair.second;
        if (entry.scope == scope && entry.name == name) {
            return true;
        }
    }
    return false;
}

std::vector<ArgumentEntry> SemanticAnalyser::getProcedureArguments(std::string name){
    std::vector<ArgumentEntry> args;
    for (const auto& pair : symbolTable->arguments) {
        const ArgumentEntry& entry = pair.second;
        if (entry.scope == name) {
            args.push_back(entry);
        }
    }
    return args;
}

void SemanticAnalyser::visit(DeclarationNode* node) {
    std::string varName = *node->identifier->name;
    SymbolTableEntry entry;
    entry.name = varName;
    entry.memoryLocation = symbolTable->nextMemoryLocation;
    entry.scope = symbolTable->currentScope;
    entry.arraySize = node->isArray() ? node->arraySize : -1;
    if(containsEntryWith(entry.scope, entry.name)){
        std::cerr << "Variable redeclaration: " << varName << " in " << entry.scope << std::endl;
        throw std::runtime_error("Variable redeclaration");
    }
    else {
        std::string uniqueKey = entry.scope + "::" + varName;
        symbolTable->symbols.insert({uniqueKey, entry});
        symbolTable->nextMemoryLocation += (entry.arraySize > 0 ? entry.arraySize : 1);
        Variable *var = new Variable(varName, entry.memoryLocation, nullptr, false, false, node->isArray());
        parent->vars.push_back(*var);
    }
}

void SemanticAnalyser::visit(ArgumentNode* node) {
    std::string varName = *node->argumentName;
    ArgumentEntry entry;
    entry.name = varName;
    entry.scope = symbolTable->currentScope;
    entry.isArray = node->isArray;
    entry.memoryLocation = symbolTable->nextMemoryLocation;
    if(containsEntryWith(entry.scope, entry.name)){
        std::cerr << "Variable redeclaration: " << varName << " in " << entry.scope << std::endl;
        throw std::runtime_error("Variable redeclaration");
    }
    else {
        std::string uniqueKey = entry.scope + "::" + varName;
        symbolTable->arguments.insert({uniqueKey, entry});
        symbolTable->nextMemoryLocation += 1;
        Variable *var = new Variable(varName, entry.memoryLocation, nullptr, false, true, entry.isArray);
        parent->vars.push_back(*var);
    }
}

void SemanticAnalyser::visit(ValueNode* node) {
    Variable *var = new Variable("value", node->value, nullptr, true, false, false);
    ValueCommand *block = new ValueCommand(var);
    ready_var = var;
    currentBlock = block;
}
void SemanticAnalyser::visit(IdentifierNode* node) {
    std::string varName = *node->name;
    ValueCommand *block;
    if(containsEntryWith(symbolTable->currentScope, varName)){
        std::string uniqueKey = symbolTable->currentScope + "::" + varName;
        auto search = symbolTable->symbols.find(uniqueKey);
        auto searchA = symbolTable->arguments.find(uniqueKey);
        Variable *var;
        if(search!=symbolTable->symbols.end()){
            Variable *access = nullptr;
            SymbolTableEntry s = search->second;
            if ( node->isArrayAccess() ) {
                currentBlock = nullptr;
                node->index->accept(this);
                access = ready_var->getOffset();

            }
            for (const auto& c : parent->vars) {
                if(c.name==varName){
                    var = new Variable(c.name, c.memoryLocation, access, c.isValue, c.isMemoryReference, c.isArray);
                    break;
                }
            }
            block = new ValueCommand(var);
            currentBlock = block;
            ready_var = var;
        } else if(searchA!=symbolTable->arguments.end()) {
            Variable *access = nullptr;
            ArgumentEntry a = searchA->second;
            if ( node->isArrayAccess() ) {
                currentBlock = nullptr;
                node->index->accept(this);
                access = ready_var->getOffset();
            }

            for (const auto& c : parent->vars) {
                if(c.name==varName){
                    var = new Variable(c.name, c.memoryLocation, access, c.isValue, c.isMemoryReference, c.isArray);
                    break;
                }
            }
            block = new ValueCommand(var);
            currentBlock = block;
            ready_var = var;
        } else {
            std::cerr << "Undeclared variable: " << varName << " in " << symbolTable->currentScope << std::endl;
            throw std::runtime_error("Undeclared variable");
        }
        
    } else {
        std::cerr << "Undeclared variable: " << varName << " in " << symbolTable->currentScope << std::endl;
        throw std::runtime_error("Undeclared variable");
    }
}
void SemanticAnalyser::visit(BinaryExpressionNode* node) {
    node->leftOperand->accept(this);
    Variable* left=ready_var;
    
    if (left->isArray && left->offset==nullptr){
        std::cerr << "Incorrect array access: " << " in " << symbolTable->currentScope << std::endl;
        throw std::runtime_error("Incorrect array access");
    }
    node->rightOperand->accept(this);
    Variable* right=ready_var;
    if (right->isArray && right->offset==nullptr){
        std::cerr << "Incorrect array access: " << " in " << symbolTable->currentScope << std::endl;
        throw std::runtime_error("Incorrect array access");
    }
    std::string op = node->opr;
    if (op == "+"){
        AddCommand *block = new AddCommand(left, right);
        currentBlock = block; 
    } else if (op == "-"){
        SubCommand *block = new SubCommand(left, right);
        currentBlock = block; 
    } else if (op == "*"){
        MulCommand *block = new MulCommand(left, right);
        currentBlock = block; 
    } else if (op == "/"){
        DivCommand *block = new DivCommand(left, right);
        currentBlock = block; 
    } else {
        ModCommand *block = new ModCommand(left, right);
        currentBlock = block; 
    }
}
void SemanticAnalyser::visit(ConditionNode* node) {
    node->leftOperand->accept(this);
    Variable* left=ready_var;
    
    if (left->isArray && left->offset==nullptr){
        std::cerr << "Incorrect array access: " << " in " << symbolTable->currentScope << std::endl;
        throw std::runtime_error("Incorrect array access");
    }
    node->rightOperand->accept(this);
    Variable* right=ready_var;
    if (right->isArray && right->offset==nullptr){
        std::cerr << "Incorrect array access: " << " in " << symbolTable->currentScope << std::endl;
        throw std::runtime_error("Incorrect array access");
    }
    
    std::string op = node->opr;
    if (op == "="){
        EQCondition *block = new EQCondition(left, right);
        currentBlock = block; 
    } else if (op == "!="){
        NEQCondition *block = new NEQCondition(left, right);
        currentBlock = block; 
    } else if (op == ">="){
        GEQCondition *block = new GEQCondition(left, right);
        currentBlock = block; 
    } else if (op == "<="){
        LEQCondition *block = new LEQCondition(left, right);
        currentBlock = block; 
    } else if (op == ">"){
        GTCondition *block = new GTCondition(left, right);
        currentBlock = block; 
    } else {
        LTCondition *block = new LTCondition(left, right);
        currentBlock = block; 
    }
}
void SemanticAnalyser::visit(CommandBlockNode* node) {
    for (const auto& c : node->body) {
        c->accept(this);
    } 
}
void SemanticAnalyser::visit(ReadNode* node) {
    node->identifier->accept(this);
    Variable* left=ready_var;
    
    if (left->isArray && left->offset==nullptr){
        std::cerr << "Incorrect array access: " << " in " << symbolTable->currentScope << std::endl;
        throw std::runtime_error("Incorrect array access");
    }
    ReadCommand *block = new ReadCommand(left);
    currentBlock = block;
}
void SemanticAnalyser::visit(WriteNode* node) {
    node->expression->accept(this);
    Variable* left=ready_var;
    
    if (left->isArray && left->offset==nullptr){
        std::cerr << "Incorrect array access: " << " in " << symbolTable->currentScope << std::endl;
        throw std::runtime_error("Incorrect array access");
    }
    WriteCommand *block = new WriteCommand(left);
    currentBlock = block;
}
void SemanticAnalyser::visit(ProcedureCallArguments* node) {
    
    CallCommand* derivedPtr = dynamic_cast<CallCommand*>(currentBlock);
    if (derivedPtr==nullptr){
        std::cerr << "Tree build error: " << checkedProcedure << std::endl;
        throw std::runtime_error("Tree build error");
    } else {
        std::vector<Variable> arguments;

        for (const auto& v:derivedPtr->parent->vars){
            if(v.name!="return index" && v.isMemoryReference==true) {
                arguments.push_back(v);
            }
        }
        if(arguments.size()!=node->args.size()){
            std::cerr << "Wrong number of arguments: " << checkedProcedure << " " <<arguments.size() <<" "<< node->args.size() << std::endl;
            throw std::runtime_error("Wrong number of arguments");
        } else {
            for (long unsigned int i = 0; i < arguments.size();i++){
                node->args[i]->accept(this);

                if (ready_var->isArray==false && arguments[i].isArray) {
                    std::cerr << "Type mismatch: " << checkedProcedure <<" "<< std::endl;
                    throw std::runtime_error("Type mismatch");
                } else if (ready_var->isArray==true && !arguments[i].isArray) {
                    std::cerr << "Type mismatch: " << checkedProcedure << std::endl;
                    throw std::runtime_error("Type mismatch");
                } else {
                    if(ready_var!=nullptr){
                        std::cout << "Add arg: " << ready_var->name << " as " << arguments[i].name << std::endl;
                        Variable var = *ready_var;
                        derivedPtr->addArg(var);
                    }
                }
            }
        }
    }
    currentBlock = nullptr;
}
void SemanticAnalyser::visit(ProcedureCallNode* node) {
    std::string name = *node->procedureName;
    if(name == symbolTable->currentScope){
        std::cerr << "Cannot call procedures recursively: " << name << std::endl;
        throw std::runtime_error("Cannot call procedures recursively");
    } else {
        bool found = false;
        for (const auto& p : intermediate_form->blocks) {
            if(p->name == name && !found){
                CallCommand *block = new CallCommand(name);
                currentBlock = block;
                block->parent = p;
                checkedProcedure = name;
                node->arguments->accept(this);
                found = true;
                currentBlock = block;
            }
        }
        if (!found) {
            std::cerr << "Undefined procedure: " << name << std::endl;
            throw std::runtime_error("Undefined procedure");
        }
    }
}
void SemanticAnalyser::visit(RepeatUntilNode* node) {
    RepeatBlock *block = new RepeatBlock();
    node->condition->accept(this);
    block->addCondition(currentBlock);
    currentBlock = block;
    for (const auto& c : node->commands->body) {
        currentBlock = nullptr;
        c->accept(this);
        if(currentBlock!=nullptr) block->addBlock(currentBlock);
        currentBlock = block;
    }
    currentBlock = block;
}
void SemanticAnalyser::visit(WhileNode* node) {
    WhileBlock *block = new WhileBlock();
    node->condition->accept(this);
    block->addCondition(currentBlock);
    currentBlock = block;
    for (const auto& c : node->commands->body) {
        currentBlock = nullptr;
        c->accept(this);
        if(currentBlock!=nullptr) block->addBlock(currentBlock);
        currentBlock = block;
    }
    currentBlock = block;   
}
void SemanticAnalyser::visit(IfNode* node) {
    IfBlock *block = new IfBlock();
    currentBlock = block;
    node->condition->accept(this);
    block->addCondition(currentBlock);
    currentBlock = block;
    for (const auto& c : node->thenCommands->body) {
        currentBlock = nullptr;
        c->accept(this);
        if(currentBlock!=nullptr) block->addThenBlock(currentBlock);
        currentBlock = block;
    } 
    if(node->elseCommands!=nullptr) {
        for (const auto& c : node->elseCommands->body) {
            currentBlock = nullptr;
            c->accept(this);
            if(currentBlock!=nullptr) block->addElseBlock(currentBlock);
            currentBlock = block;
        } 
    }
    currentBlock = block;
}
void SemanticAnalyser::visit(AssignmentNode* node) {
    AssignCommand *block;
    node->identifier->accept(this);
    Variable* left=ready_var;
    
    if (left->isArray && left->offset==nullptr){
        std::cerr << "Incorrect array access: " << " in " << symbolTable->currentScope << std::endl;
        throw std::runtime_error("Incorrect array access");
    }
    currentBlock = nullptr;
    node->expression->accept(this);
    block = new AssignCommand(left, currentBlock);
    currentBlock = block;
}
void SemanticAnalyser::visit(DeclarationsNode* node) {
    for (const auto& c : node->declarations) {
        c->accept(this);
    }
}
void SemanticAnalyser::visit(MainNode* node) {
    symbolTable->currentScope = "main";
    ParentBlock* p = new ParentBlock("main");
    parent = p;
    intermediate_form->addBlock(p);
    node->decl->accept(this);
    ContentBlock *body = new ContentBlock(p);
    for (const auto& c : node->body->body) {
        c->accept(this);
        if(currentBlock!=nullptr) body->addBlock(currentBlock);
        currentBlock = nullptr;
    }
    p->addContent(body);
}
void SemanticAnalyser::visit(ArgumentsNode* node) {
    for (const auto& c : node->arguments) {
        c->accept(this);
    }
}
void SemanticAnalyser::visit(ProcedureHeadNode* node) {
    symbolTable->currentScope = *node->name;
    node->arguments->accept(this);
}
void SemanticAnalyser::visit(ProcedureNode* node) {
    ParentBlock *p;
    if (onlyGatherProcedureNames) {
        p = new ParentBlock(*node->arguments->name);
        Variable *v = new Variable("return index", symbolTable->nextMemoryLocation, new Variable("value", 0, nullptr, true, false, false), true, false, false);
        p->vars.push_back(*v);
        symbolTable->nextMemoryLocation += 1;
        parent = p;
        intermediate_form->addBlock(p);
        node->arguments->accept(this);
        node->declarations->accept(this);
    }
    if(!onlyGatherProcedureNames){
        symbolTable->currentScope = *node->arguments->name;
        for (const auto& c : intermediate_form->blocks) {
            if(c->name==*node->arguments->name){
                p = c;
                parent = p;
                break;
            }
        }
        ContentBlock *body = new ContentBlock(p);
        for (const auto& c : node->commands->body) {
            c->accept(this);
            if(currentBlock!=nullptr) body->addBlock(currentBlock);
            currentBlock = nullptr;
        } 
        p->addContent(body);
    }
}
void SemanticAnalyser::visit(ProceduresNode* node) {
    for (const auto& c : node->procedures) {
        if (onlyGatherProcedureNames) procedures.push_back(c);
        c->accept(this);
    }
}
void SemanticAnalyser::visit(ProgramNode* node) {
    if(node->procedures!=nullptr) node->procedures->accept(this);
    node->mainBlock->accept(this);
    onlyGatherProcedureNames = false;
    if(node->procedures!=nullptr) node->procedures->accept(this);
}

ValueCommand::ValueCommand(Variable* val) {
    this->val = val;
}

void ValueCommand::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Variable: " << val->name << " memory location: " << val->memoryLocation << " is Value? "
              << val->isValue << " is Memory Reference? " << val->isMemoryReference << " is Array? " << val->isArray << std::endl;
}

void ValueCommand::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

AddCommand::AddCommand(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}

void AddCommand::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Add: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

void AddCommand::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

SubCommand::SubCommand(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}
void SubCommand::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Sub: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

void SubCommand::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

MulCommand::MulCommand(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}

void MulCommand::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

void MulCommand::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Mul: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

DivCommand::DivCommand(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}

void DivCommand::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Div: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

void DivCommand::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

ModCommand::ModCommand(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}

void ModCommand::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Mod: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

void ModCommand::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}


EQCondition::EQCondition(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}

void EQCondition::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "EQ: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

void EQCondition::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

NEQCondition::NEQCondition(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}

void NEQCondition::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "NEQ: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

void NEQCondition::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

GEQCondition::GEQCondition(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}

void GEQCondition::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "GEQ: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

void GEQCondition::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

LEQCondition::LEQCondition(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}

void LEQCondition::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "LEQ: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

void LEQCondition::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}


GTCondition::GTCondition(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}

void GTCondition::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "GT: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

void GTCondition::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

LTCondition::LTCondition(Variable* left, Variable* right) {
    this->left = left;
    this->right = right;
}

void LTCondition::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "LT: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray 
              << " Variable: " << right->name << " memory location: " << right->memoryLocation << " is Value? "
              << right->isValue << " is Memory Reference? " << right->isMemoryReference << " is Array? " << right->isArray <<std::endl;
}

void LTCondition::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

AssignCommand::AssignCommand(Variable* left, Block* right) {
    this->left = left;
    this->right = right;
}

void Block::print(int level) {
    std::cout << "empty\n";
}

void AssignCommand::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Assign: Variable: " << left->name << " memory location: " << left->memoryLocation << " is Value? "
              << left->isValue << " is Memory Reference? " << left->isMemoryReference << " is Array? " << left->isArray << ":=";
    right->print(level + 1);
}

void AssignCommand::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

ReadCommand::ReadCommand(Variable* val) {
    this->val = val;
}

void ReadCommand::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Read Variable: " << val->name << " memory location: " << val->memoryLocation << " is Value? "
              << val->isValue << " is Memory Reference? " << val->isMemoryReference << " is Array? " << val->isArray << std::endl;
}

void ReadCommand::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

WriteCommand::WriteCommand(Variable* val) {
    this->val = val;
}

void WriteCommand::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Write Variable: " << val->name << " memory location: " << val->memoryLocation << " is Value? "
              << val->isValue << " is Memory Reference? " << val->isMemoryReference << " is Array? " << val->isArray << std::endl;
}

void WriteCommand::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

CallCommand::CallCommand(std::string name) {
    this->procedure_name = name;
}
void CallCommand::addArg(Variable val) {
    vals.push_back(val);
}

void CallCommand::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

void CallCommand::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << procedure_name << " commands: " << std::endl;
    std::string indent2((level + 1) * 4, ' ');
    std::vector<Variable> p_vars = parent->vars;
    int skipped = 0;
    for (long unsigned int i = 0; i < p_vars.size();i++)
    {
        if (p_vars[i].name!="return index" && p_vars[i].isMemoryReference){
        std::cout << indent2 << "Variable: " << p_vars[i].name << " memory location: " << p_vars[i].memoryLocation << " is Value? "
                  << p_vars[i].isValue << " is Memory Reference? " << p_vars[i].isMemoryReference << " is Array? " << p_vars[i].isArray << " Assign "
                  << "Variable: " << vals[i-skipped].name << " memory location: " << vals[i-skipped].memoryLocation << " is Value? "
                  << vals[i-skipped].isValue << " is Memory Reference? " << vals[i-skipped].isMemoryReference << " is Array? " << vals[i-skipped].isArray << std::endl;
        } else {
            skipped++;
        }
    }
}

void IfBlock::addCondition(Block* c) {
    this->cond = c;
}

void IfBlock::addThenBlock(Block* b) {
    thenBlock.push_back(b);
}

void IfBlock::addElseBlock(Block* b) {
    elseBlock.push_back(b);
}

void IfBlock::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "If block: " << std::endl;
    std::string indent2((level + 1) * 4, ' ');
    cond->print(level + 1);
    std::cout << indent2 << "Then block: \n";

    if (thenBlock.empty() == false){
        for (const auto& v : thenBlock) {
                if(v!=nullptr) v->print(level + 2);
            }
    }
    
    if (elseBlock.empty() == false){
        std::cout << indent2 << "Else block: \n";
        for (const auto& v : elseBlock) {
            v->print(level + 2);
        }
    }
}

void IfBlock::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

void WhileBlock::addCondition(Block* c) {
    this->cond = c;
}

void WhileBlock::addBlock(Block* b) {
    body.push_back(b);
}

void WhileBlock::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "While block: " << std::endl;
    std::string indent2((level + 1) * 4, ' ');
    cond->print(level + 1);
    std::cout << indent2 << "Body: \n";
    for (const auto& v : body) {
        if(v!=nullptr) v->print(level + 2);
    }
}

void WhileBlock::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

void RepeatBlock::addCondition(Block* c) {
    this->cond = c;
}

void RepeatBlock::addBlock(Block* b) {
    body.push_back(b);
}

void RepeatBlock::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Repeat block: " << std::endl;
    std::string indent2((level + 1) * 4, ' ');
    cond->print(level + 1);
    std::cout << indent2 << "Body: \n";
    for (const auto& v : body) {
        if(v!=nullptr) v->print(level + 2);
    }
}

void RepeatBlock::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

ContentBlock::ContentBlock(ParentBlock* parent) {
    this->parent = parent;
}

void ContentBlock::addBlock(Block* b){
    body.push_back(b);
}

void ContentBlock::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Content block: parent name: "<<parent->name << std::endl;
    for (const auto& v : body) {
        if(v!=nullptr) v->print(level + 1);
    }
}

void ContentBlock::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

ParentBlock::ParentBlock(std::string name) {
    this->name = name;
}

void ParentBlock::addContent(ContentBlock* b) {
    content = b;
}

void ParentBlock::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Parent block: "<<name << std::endl;
    std::string indent2((level + 1) * 4, ' ');
    for (const auto& v : vars) {
        std::cout << indent2 << "Variable: " << v.name << " memory location: " << v.memoryLocation << " is Value? "
              << v.isValue << " is Memory Reference? " << v.isMemoryReference << " is Array? " << v.isArray << std::endl;
    }
    content->print(level+2);
}

void ParentBlock::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

void Intermediate::addBlock(ParentBlock* b) {
    blocks.push_back(b);
}

void Intermediate::print(int level) {
    std::string indent(level * 4, ' ');
    std::cout << indent << "Intermediate representation: " << std::endl;
    std::string indent2((level + 1) * 4, ' ');
    for (const auto& v : blocks) {
        if(v!=nullptr) v->print(level + 1);
    }
}

void Intermediate::accept(BlockVisitor* visitor) {
    visitor->visit(this);
}

