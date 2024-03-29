#include <unordered_map>
#include <string>
#include <iostream>
#include <stdexcept>
#include "ast_nodes.hpp"
#include "semantic_analyser.hpp"
#include "code_generator.hpp"

int level=0;

CodeSimple::CodeSimple(std::string c, long long* j) {
    command = c;
    jump = j;
} 

void CodeBlock::addCodeBlock(CodeBlock* block){
    body.push_back(block);
}

void CodeBlock::addFront(CodeBlock* block){
    body.insert(body.begin(), block);
}

CodeGenerator::CodeGenerator(Intermediate* i) {
    this->itermediate_form = i;
}

void CodeGenerator::setLocation(Variable* var, std::string reg) {
    long long value = 0;
    long long stored_value = 0;
    if (var->isMemoryReference==false){
        if(var->offset!=nullptr) {
            if(var->offset->isValue==true) {
                long long mem = var->memoryLocation + var->offset->memoryLocation;
                currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                value = 0;
                k++;
                if(mem != 0) {
                    currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                    value = 1;
                    k++;
                }
                while ((value+stored_value)!=mem) {
                    if((value*2+stored_value)<=mem) {
                        currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                        value = value * 2;
                        k++;
                    } else if (value + stored_value <= mem) {
                        currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                        k++;
                        if (stored_value!=0){
                            currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                            k++;
                        }
                        currentBlock->addCodeBlock(new CodeSimple("PUT g", nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                        k++;
                        stored_value += value;
                        value = 1;
                    }
                }
                if(stored_value!=0) {
                    currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                    k++;
                }
            } else {
                long long mem = var->memoryLocation;
                currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                value = 0;
                k++;
                if(mem != 0) {
                    currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                    value = 1;
                    k++;
                }
                while ((value+stored_value)!=mem) {
                    if((value*2+stored_value)<=mem) {
                        currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                        value = value * 2;
                        k++;
                    } else if (value + stored_value <= mem) {
                        currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                        k++;
                        if (stored_value!=0){
                            currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                            k++;
                        }
                        currentBlock->addCodeBlock(new CodeSimple("PUT g", nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                        k++;
                        stored_value += value;
                        value = 1;
                    }
                }
                if(stored_value!=0) {
                    currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                    k++;
                }
                currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("PUT f", nullptr));
                k++;
                if(var->offset->isMemoryReference==true){
                    setLocation(var->offset, reg);
                } else {
                    mem = var->offset->memoryLocation;
                    currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                    value = 0;
                    stored_value = 0;
                    k++;
                    if(mem != 0) {
                        currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                        value = 1;
                        k++;
                    }
                    while ((value+stored_value)!=mem) {
                        if((value*2+stored_value)<=mem) {
                            currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                            value = value * 2;
                            k++;
                        } else if (value + stored_value <= mem) {
                            currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                            k++;
                            if (stored_value!=0){
                                currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                                k++;
                            }
                            currentBlock->addCodeBlock(new CodeSimple("PUT g", nullptr));
                            k++;
                            currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                            k++;
                            currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                            k++;
                            stored_value += value;
                            value = 1;
                        }
                    }
                    if(stored_value!=0) {
                        currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                        k++;
                    }
                }
                
                currentBlock->addCodeBlock(new CodeSimple("LOAD " + reg, nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("ADD f ", nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                k++;
            }
        } else {
            long long mem = var->memoryLocation;
            currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
            value = 0;
            k++;
            currentBlock->addCodeBlock(new CodeSimple("RST g", nullptr));
            k++;
            if(mem != 0) {
                currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                value = 1;
                k++;
            }
            while ((value+stored_value)!=mem) {
                if((value*2+stored_value)<=mem) {
                    currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                    value = value * 2;
                    k++;
                } else if (value + stored_value <= mem) {
                    currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                    k++;
                    if (stored_value!=0){
                        currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                        k++;
                    }
                    currentBlock->addCodeBlock(new CodeSimple("PUT g", nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                    k++;
                    stored_value += value;
                    value = 1;
                }
            }
            if(stored_value!=0) {
                currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                k++;
            }
        }
    } else {
        if(var->offset!=nullptr) {
            if(var->offset->isValue==true) {
                long long mem = var->memoryLocation;
                currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                value = 0;
                k++;
                if(mem != 0) {
                    currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                    value = 1;
                    k++;
                }
                while ((value+stored_value)!=mem) {
                    if((value*2+stored_value)<=mem) {
                        currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                        value = value * 2;
                        k++;
                    } else if (value + stored_value <= mem) {
                        currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                        k++;
                        if (stored_value!=0){
                            currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                            k++;
                        }
                        currentBlock->addCodeBlock(new CodeSimple("PUT g", nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                        k++;
                        stored_value += value;
                        value = 1;
                    }
                }
                if(stored_value!=0) {
                    currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                    k++;
                }
                currentBlock->addCodeBlock(new CodeSimple("LOAD " + reg, nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("PUT f", nullptr));
                k++;
                mem = var->offset->memoryLocation;
                currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                value = 0;
                stored_value = 0;
                k++;
                if(mem != 0) {
                    currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                    value = 1;
                    k++;
                }
                while ((value+stored_value)!=mem) {
                    if((value*2+stored_value)<=mem) {
                        currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                        value = value * 2;
                        k++;
                    } else if (value + stored_value <= mem) {
                        currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                        k++;
                        if (stored_value!=0){
                            currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                            k++;
                        }
                        currentBlock->addCodeBlock(new CodeSimple("PUT g", nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                        k++;
                        stored_value += value;
                        value = 1;
                    }
                }
                if(stored_value!=0) {
                    currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                    k++;
                }
                currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("ADD f ", nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                k++;
            } else {
                long long mem = var->memoryLocation;
                currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                value = 0;
                k++;
                if(mem != 0) {
                    currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                    value = 1;
                    k++;
                }
                while ((value+stored_value)!=mem) {
                    if((value*2+stored_value)<=mem) {
                        currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                        value = value * 2;
                        k++;
                    } else if (value + stored_value <= mem) {
                        currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                        k++;
                        if (stored_value!=0){
                            currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                            k++;
                        }
                        currentBlock->addCodeBlock(new CodeSimple("PUT g", nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                        k++;
                        stored_value += value;
                        value = 1;
                    }
                }
                if(stored_value!=0) {
                    currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                    k++;
                }
                currentBlock->addCodeBlock(new CodeSimple("LOAD " + reg, nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("PUT f", nullptr));
                k++;
                if(var->offset->isMemoryReference==true){
                    setLocation(var->offset, reg);
                } else {
                    mem = var->offset->memoryLocation;
                    currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                    value = 0;
                    stored_value = 0;
                    k++;
                    if(mem != 0) {
                        currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                        value = 1;
                        k++;
                    }
                    while ((value+stored_value)!=mem) {
                        if((value*2+stored_value)<=mem) {
                            currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                            value = value * 2;
                            k++;
                        } else if (value + stored_value <= mem) {
                            currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                            k++;
                            if (stored_value!=0){
                                currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                                k++;
                            }
                            currentBlock->addCodeBlock(new CodeSimple("PUT g", nullptr));
                            k++;
                            currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                            k++;
                            currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                            k++;
                            stored_value += value;
                            value = 1;
                        }
                    }
                    if(stored_value!=0) {
                        currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                        k++;
                        currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                        k++;
                    }
                }
                
                currentBlock->addCodeBlock(new CodeSimple("LOAD " + reg, nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("ADD f ", nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                k++;
            }
        } else {
            long long mem = var->memoryLocation;
            currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
            value = 0;
            k++;
            if(mem != 0) {
                currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                value = 1;
                k++;
            }
            while ((value+stored_value)!=mem) {
                if((value*2+stored_value)<=mem) {
                    currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                    value = value * 2;
                    k++;
                } else if (value + stored_value <= mem) {
                    currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                    k++;
                    if (stored_value!=0){
                        currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                        k++;
                    }
                    currentBlock->addCodeBlock(new CodeSimple("PUT g", nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
                    k++;
                    currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                    k++;
                    stored_value += value;
                    value = 1;
                }
            }
            if(stored_value!=0) {
                currentBlock->addCodeBlock(new CodeSimple("GET " + reg, nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("ADD g", nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("PUT " + reg, nullptr));
                k++;
            }
            currentBlock->addCodeBlock(new CodeSimple("LOAD " + reg, nullptr));
            k++;
            currentBlock->addCodeBlock(new CodeSimple("PUT "+ reg, nullptr));
            k++;
        }
    }

}

void CodeGenerator::setARGLocation(Variable* var, std::string reg) {
    long long value = 0;
    
    if(var->offset!=nullptr) {
        if(var->offset->isValue==true) {
            long long mem = var->memoryLocation + var->offset->memoryLocation;
            currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
            value = 0;
            k++;
            if(mem != 0) {
                currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                value = 1;
                k++;
            }
            while (value!=mem) {
                if(value*2<=mem) {
                    currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                    value = value * 2;
                    k++;
                } else if (value + 1 <= mem) {
                    currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                    value += 1;
                    k++;
                }
            }
        } else {
            long long mem = var->offset->memoryLocation;
            currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
            value = 0;
            k++;
            if(mem != 0) {
                currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                value = 1;
                k++;
            }
            while (value!=mem) {
                if(value*2<=mem) {
                    currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                    value = value * 2;
                    k++;
                } else if (value + 1 <= mem) {
                    currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                    value += 1;
                    k++;
                }
            }
            currentBlock->addCodeBlock(new CodeSimple("LOAD " + reg, nullptr));
            k++;
            mem = var->memoryLocation;
            currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
            value = 0;
            k++;
            if(mem != 0) {
                currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                value = 1;
                k++;
            }
            while (value!=mem) {
                if(value*2<=mem) {
                    currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                    value = value * 2;
                    k++;
                } else if (value + 1 <= mem) {
                    currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                    value += 1;
                    k++;
                }
            }
            currentBlock->addCodeBlock(new CodeSimple("ADD " + reg, nullptr));
            k++;
            currentBlock->addCodeBlock(new CodeSimple("PUT "+ reg, nullptr));
            k++;
        }
    } else {
        long long mem = var->memoryLocation;
        currentBlock->addCodeBlock(new CodeSimple("RST " + reg, nullptr));
        value = 0;
        k++;
        if(mem != 0) {
            currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
            value = 1;
            k++;
        }
        while (value!=mem) {
            if(value*2<=mem) {
                currentBlock->addCodeBlock(new CodeSimple("SHL " + reg, nullptr));
                value = value * 2;
                k++;
            } else if (value + 1 <= mem) {
                currentBlock->addCodeBlock(new CodeSimple("INC " + reg, nullptr));
                value += 1;
                k++;
            }
        }
    }

}

void CodeGenerator::visit(ValueCommand* node) {
    setLocation(node->val, usable_reg);
    if (node->val->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD " + usable_reg, nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT "+ usable_reg, nullptr));
        k++;
    }  
    std::string indent(level * 4, ' ');  
}

void CodeGenerator::visit(AddCommand* node) {
    std::string indent(level * 4, ' ');
    
    setLocation(node->right, "c");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD c", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
        k++;
    }
    setLocation(node->left, "b");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
        k++;
    }
    currentBlock->addCodeBlock(new CodeSimple("ADD c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
    k++;

}

void CodeGenerator::visit(SubCommand* node) {
    std::string indent(level * 4, ' ');
    
    setLocation(node->right, "c");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD c", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
        k++;
    }
    setLocation(node->left, "b");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
        k++;
    }
    currentBlock->addCodeBlock(new CodeSimple("SUB c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
    k++;

}

void CodeGenerator::visit(MulCommand* node) {
    std::string indent(level * 4, ' ');
    
    setLocation(node->left, "b");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT b", nullptr));
        k++;
    }
    setLocation(node->right, "d");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD d", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT d", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT e", nullptr));
        k++;
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET d", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT e", nullptr));
        k++;
    }
    
    currentBlock->addCodeBlock(new CodeSimple("RST c", nullptr));
    k++;
    long long ret = k;
    currentBlock->addCodeBlock(new CodeSimple("GET d", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JZERO "+std::to_string(k+12), nullptr));//end
    k++;
    currentBlock->addCodeBlock(new CodeSimple("PUT e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SHR e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SHL e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JZERO "+std::to_string(k+4), nullptr));//check
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("ADD c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
    k++;
    //check
    currentBlock->addCodeBlock(new CodeSimple("SHL b", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SHR d", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JUMP "+std::to_string(ret), nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET c", nullptr));
    k++;

}

void CodeGenerator::visit(DivCommand* node) {
    std::string indent(level * 4, ' ');

    setLocation(node->left, "b");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT b", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
        k++;
    }
    setLocation(node->right, "d");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD d", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT d", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT e", nullptr));
        k++;
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET d", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT e", nullptr));
        k++;
    }
    currentBlock->addCodeBlock(new CodeSimple("RST f", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("RST g", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET d", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JZERO "+std::to_string(k+32), nullptr));//end dzielenie przez 0
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("INC a", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB d", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JZERO "+std::to_string(k+28), nullptr));//check
    k++;
    long long ret = k;
    currentBlock->addCodeBlock(new CodeSimple("GET d", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("PUT e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("RST g", nullptr));
    k++;
    long long ret2 = k;
    currentBlock->addCodeBlock(new CodeSimple("SHL e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("INC g", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("INC a", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS "+std::to_string(ret2), nullptr));//check
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SHR e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("DEC g", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("RST h", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("INC h", nullptr));
    k++;
    long long q = k;
    currentBlock->addCodeBlock(new CodeSimple("GET g", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JZERO "+std::to_string(k+4), nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SHL h", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("DEC g", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JUMP "+std::to_string(q), nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET f", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("ADD h", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("PUT f", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
    k++;
    //check
    currentBlock->addCodeBlock(new CodeSimple("INC a", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB d", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS "+std::to_string(ret), nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET f", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
    k++;

}

void CodeGenerator::visit(ModCommand* node) {
    std::string indent(level * 4, ' ');

    setLocation(node->left, "b");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT b", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
        k++;
    }
    setLocation(node->right, "d");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD d", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT d", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT e", nullptr));
        k++;
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET d", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT e", nullptr));
        k++;
    }
    
    currentBlock->addCodeBlock(new CodeSimple("GET d", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JZERO "+std::to_string(k+19), nullptr));//end dzielenie przez 0
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("INC a", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB d", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JZERO "+std::to_string(k+15), nullptr));//check
    k++;
    long long ret = k;
    currentBlock->addCodeBlock(new CodeSimple("GET d", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("PUT e", nullptr));
    k++;
    long long ret2 = k;
    currentBlock->addCodeBlock(new CodeSimple("SHL e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("INC a", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS "+std::to_string(ret2), nullptr));//check
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SHR e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB e", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
    k++;
    //check
    currentBlock->addCodeBlock(new CodeSimple("INC a", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB d", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS "+std::to_string(ret), nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET c", nullptr));
    k++;
}

void CodeGenerator::visit(EQCondition* node) {
    std::string indent(level * 4, ' ');
    
    setLocation(node->right, "c");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD c", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
        k++;
    }
    setLocation(node->left, "b");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT b", nullptr));
        k++;
        
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
        k++;
    }
    currentBlock->addCodeBlock(new CodeSimple("SUB c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS ", &currentBlock->exit_index));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB b", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS ", &currentBlock->exit_index));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JUMP ", &currentBlock->start_index));
    k++;
}

void CodeGenerator::visit(NEQCondition* node) {
    std::string indent(level * 4, ' ');
    setLocation(node->right, "c");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD c", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
        k++;
    }
    setLocation(node->left, "b");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT b", nullptr));
        k++;
        
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
        k++;
    }
    currentBlock->addCodeBlock(new CodeSimple("SUB c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS ", &currentBlock->start_index));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("GET c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB b", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS ", &currentBlock->start_index));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JUMP ", &currentBlock->exit_index));
    k++;
}

void CodeGenerator::visit(GEQCondition* node) {
    std::string indent(level * 4, ' ');

    setLocation(node->right, "c");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD c", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
        k++;
    }
    setLocation(node->left, "b");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
        k++;
    }
    currentBlock->addCodeBlock(new CodeSimple("INC a", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS ", &currentBlock->start_index));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JUMP ", &currentBlock->exit_index));
    k++;
}

void CodeGenerator::visit(LEQCondition* node) {
    std::string indent(level * 4, ' ');
    
    setLocation(node->left, "c");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD c", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
        k++;
    }
    setLocation(node->right, "b");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
        k++;
    }
    currentBlock->addCodeBlock(new CodeSimple("INC a", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("SUB c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS ", &currentBlock->start_index));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JUMP ", &currentBlock->exit_index));
    k++;
}

void CodeGenerator::visit(GTCondition* node) {
    std::string indent(level * 4, ' ');
    
    setLocation(node->right, "c");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD c", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
        k++;
    }
    setLocation(node->left, "b");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
        k++;
    }
    currentBlock->addCodeBlock(new CodeSimple("SUB c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS ", &currentBlock->start_index));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JUMP ", &currentBlock->exit_index));
    k++;
}

void CodeGenerator::visit(LTCondition* node) {
    std::string indent(level * 4, ' ');
    setLocation(node->left, "c");
    if (node->left->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD c", nullptr));
        k++;
        currentBlock->addCodeBlock(new CodeSimple("PUT c", nullptr));
        k++;
    }
    setLocation(node->right, "b");
    if (node->right->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
        
    } else {
        currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
        k++;
    }
    currentBlock->addCodeBlock(new CodeSimple("SUB c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JPOS ", &currentBlock->start_index));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("JUMP ", &currentBlock->exit_index));
    k++;
}

void CodeGenerator::visit(AssignCommand* node) {
    std::string indent(level * 4, ' ');
    usable_reg = "c";
    node->right->accept(this);

    setLocation(node->left, "b");
    currentBlock->addCodeBlock(new CodeSimple("GET c", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("STORE b", nullptr));
    k++;
}

void CodeGenerator::visit(ReadCommand* node) {
    std::string indent(level * 4, ' ');

    setLocation(node->val, "b");
    currentBlock->addCodeBlock(new CodeSimple("READ", nullptr));
    k++;
    currentBlock->addCodeBlock(new CodeSimple("STORE b", nullptr));
    k++;

}

void CodeGenerator::visit(WriteCommand* node) {
    std::string indent(level * 4, ' ');
    setLocation(node->val, "b");
    if (node->val->isValue==false){
        currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
        k++;
    }  else {
        currentBlock->addCodeBlock(new CodeSimple("GET b", nullptr));
        k++;
    }
    currentBlock->addCodeBlock(new CodeSimple("WRITE", nullptr));
    k++;
}

void CodeGenerator::visit(CallCommand* node) {
    std::string indent(level * 4, ' ');
    std::string indent2((level + 1) * 4, ' ');
    std::vector<Variable> p_vars = node->parent->vars;
    long long skipped = 0;
    for (long unsigned int i = 0; i < p_vars.size();i++)
    {
        if (p_vars[i].name!="return index" && p_vars[i].isMemoryReference){
            setARGLocation(&p_vars[i], "b");
            setLocation(&node->vals[i-skipped], "c");
            currentBlock->addCodeBlock(new CodeSimple("GET c", nullptr));
            k++;
            currentBlock->addCodeBlock(new CodeSimple("STORE b", nullptr));
            k++;

        } else {
            skipped++;
        }
        
    }
    for (long unsigned int i = 0; i < p_vars.size();i++)
    {
        if(p_vars[i].name=="return index"){
            setLocation(&p_vars[i], "b");
            currentBlock->addCodeBlock(new CodeSimple("RST c", nullptr));
            k++;
            currentBlock->addCodeBlock(new CodeSimple("INC c", nullptr));
            k++;
            currentBlock->addCodeBlock(new CodeSimple("SHL c", nullptr));
            k++;
            currentBlock->addCodeBlock(new CodeSimple("SHL c", nullptr));
            k++;
            currentBlock->addCodeBlock(new CodeSimple("STRK a", nullptr));
            k++;
            currentBlock->addCodeBlock(new CodeSimple("ADD c", nullptr));
            k++;
            currentBlock->addCodeBlock(new CodeSimple("STORE b", nullptr));
            k++;

            break;
        }
       if(i==p_vars.size()-1)
           std::cout << "ERROR\n";
    }
    for (const auto &v: end_result->body) {
        if(v!=nullptr) {
            if(v->name==node->parent->name) {
                currentBlock->addCodeBlock(new CodeSimple("JUMP ", &v->start_index));
                k++;
            }
        }
    }
      
}

void CodeGenerator::visit(IfBlock* node) {
    CodeBlock *block = new CodeBlock();
    long long out=k;
    std::string indent(level * 4, ' ');
    currentBlock = block;

    node->cond->accept(this);
    block->start_index = k;
    int current_level = level;
    level++;

    for(const auto& v:node->thenBlock) {
        currentBlock=block;
        v->accept(this);
        if(block!=currentBlock)block->addCodeBlock(currentBlock);
    }
    block->addCodeBlock(new CodeSimple("JUMP ", &out));
    k++;
    block->exit_index = k;

    if(node->elseBlock.empty()==false){
        for(const auto& v:node->elseBlock) {
            currentBlock=block;
            if(v !=nullptr)v->accept(this);
            if(currentBlock!=block)block->addCodeBlock(currentBlock);
        }
        
    }
    out = k;
    for (const auto &v:block->body){
        CodeSimple* derivedPtr = dynamic_cast<CodeSimple*>(v);
        if (derivedPtr!=nullptr) {
            if(derivedPtr->jump==&out){
                derivedPtr->command = derivedPtr->command + std::to_string(out);
                derivedPtr->jump = nullptr;
            }
        } 
    }
    
    level = current_level;
    currentBlock = block;
}

void CodeGenerator::visit(RepeatBlock* node) {
    CodeBlock *block = new CodeBlock();
    block->exit_index = k;
    std::string indent(level * 4, ' ');
    currentBlock = block;
    
    int current_level = level;
    level++;
    for(const auto& v:node->body) {
        currentBlock = block;
        v->accept(this);
        if(block!=currentBlock)block->addCodeBlock(currentBlock);
    }
    level = current_level;
    node->cond->accept(this);
    block->start_index = k;
    currentBlock = block;
}

void CodeGenerator::visit(WhileBlock* node) {
    CodeBlock *block = new CodeBlock();
    std::string indent(level * 4, ' ');
    currentBlock = block;
    long long cond_indx = k;
    node->cond->accept(this);
    block->start_index = k;
    int current_level = level;
    level++;
    for(const auto& v:node->body) {
        currentBlock = block;
        if(v!=nullptr) v->accept(this);
        if(block!=currentBlock)block->addCodeBlock(currentBlock);
    }
    block->addCodeBlock(new CodeSimple("JUMP "+std::to_string(cond_indx), nullptr));
    k++;
    block->exit_index = k;
    level = current_level;
    currentBlock = block;
}

void CodeGenerator::visit(ContentBlock* node) {
    CodeBlock *block = currentBlock;
    for (const auto& v: node->body) {
        if(v!=nullptr) {
            currentBlock = block;
            v->accept(this);
            if (currentBlock!=block) {
                block->addCodeBlock(currentBlock);
            }
        }
    }
    currentBlock = block;
}

void CodeGenerator::visit(ParentBlock* node) {
    CodeBlock *block = new CodeBlock();
    block->name = node->name;
    block->start_index = k;
    currentBlock = block;
    level = 0;
    parent = node;
    std::string indent(level * 4, ' ');
    level++;
    std::string indent2(level * 4, ' ');

    level++;
    node->content->accept(this);
    block->exit_index = k;
    currentBlock = block;
    if(node->name=="main"){
        end_result->addFront(new CodeSimple("JUMP ", &block->start_index));
        currentBlock->addCodeBlock(new CodeSimple("HALT", nullptr));
    } else {
        for (auto& v:node->vars) {
            if (v.name=="return index") {
                setLocation(&v, "b");
                currentBlock->addCodeBlock(new CodeSimple("LOAD b", nullptr));
                k++;
                currentBlock->addCodeBlock(new CodeSimple("JUMPR a", nullptr));
                k++;
            }
        }
    }
}

void CodeGenerator::visit(Intermediate* node) {
    end_result->start_index = 0;
    for (const auto &v: node->blocks) {
        if(v!=nullptr) {
            v->accept(this);
            end_result->addCodeBlock(currentBlock);
        }
    }
}

CodeFormater::CodeFormater(CodeBlock* code) {
    program = code;
}

void CodeFormater::transform(CodeBlock * code) {

    for (const auto &v:code->body){
        CodeSimple* derivedPtr = dynamic_cast<CodeSimple*>(v);
        if (derivedPtr!=nullptr) {
            if(derivedPtr->jump==nullptr){
                end_res.push_back(derivedPtr->command);
            } else {
                std::string c = derivedPtr->command + std::to_string(*derivedPtr->jump);
                end_res.push_back(c);
            }
        } else {
            transform(v);
        }
    }
}