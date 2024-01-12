%{
#include <iostream>
#include <vector>
#include <memory>

#include "io_handler.hpp"
#include "ast_nodes.hpp"
#include "semantic_analyser.hpp"
#include "code_generator.hpp"

extern int yylex();
extern FILE *yyin;
extern int yylineno;
extern char* yytext;

std::vector<std::string> code;
ProgramNode * root;

void yyerror(const char *s) {
    std::cerr << "Error at line " << yylineno << ": " << s << std::endl;
}

%}


%union {
    
    std::string* pidentifier;
    long long num;
    ProgramNode* program_node;
    ProceduresNode* procedures_node;
    MainNode* main_node;
    ProcedureNode* procedure_node;
    ExpressionNode* expression_node;
    ConditionNode* condition_node;
    CommandBlockNode* command_block;
    CommandNode* command_node;
    ProcedureHeadNode* procedure_head;
    ProcedureCallNode* procedure_call;
    ProcedureCallArguments* proc_arg;
    ArgumentsNode* args_node;
    DeclarationsNode* decl_node;
    IdentifierNode* identifier_node;
    void* ptr;
}



%token <pidentifier> pidentifier
%token <num> num
%token PROCEDURE PROGRAM IS IN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL READ WRITE ADD SUB DIV MOD MUL EQ NEQ GT LT LEQ GEQ ASSIGN T ERROR
%type <program_node> program_all
%type <procedures_node> procedures
%type <main_node> main
%type <expression_node> expression value
%type <condition_node> condition
%type <command_block> commands
%type <command_node> command
%type <procedure_head> proc_head
%type <procedure_call> proc_call
%type <proc_arg> args
%type <args_node> args_decl
%type <decl_node> declarations
%type <identifier_node> identifier

%%

program_all: procedures main
            {
                root = new ProgramNode();
                root->addProcedures($1);
                root->setMainBlock($2);
            }
            ;

procedures: /* empty */
            {
                $$ = new ProceduresNode();

            }
          | procedures PROCEDURE proc_head IS declarations IN commands END
            {
                $1->procedures.push_back(new ProcedureNode($3,$5,$7));
                $$ = $1;
            }
          | procedures PROCEDURE proc_head IS IN commands END
            {
                $1->procedures.push_back(new ProcedureNode($3,nullptr,$6));
                $$= $1;
            }
          ;

main: PROGRAM IS declarations IN commands END
      {
          $$ = new MainNode($3,$5);
      }
    | PROGRAM IS IN commands END
      {
          $$ = new MainNode(nullptr,$4);
      }
    ;

commands: commands command
          {   
              $1->body.push_back($2);
              $$ = $1;
          }
        | command
          {
              $$ = new CommandBlockNode();
              $$->body.push_back($1);
          }
        ;

command: identifier ASSIGN expression ';'
         {
            $$ = new AssignmentNode($1,$3);
         }
       | IF condition THEN commands ELSE commands ENDIF
         {
            $$ = new IfNode($2,$4,$6);
         }
       | IF condition THEN commands ENDIF
         {
            $$ = new IfNode($2,$4,nullptr);
         }
       | WHILE condition DO commands ENDWHILE
         {
            $$ = new WhileNode($2,$4);
         }
       | REPEAT commands UNTIL condition ';'
         {
            $$ = new RepeatUntilNode($4,$2);
         }
       | proc_call ';'
         {
            $$ = $1;
         }
       | READ identifier ';'
         {
            $$ = new ReadNode($2);
         }
       | WRITE value ';'
         {
            $$ = new WriteNode($2);
         }
       ;

proc_head: pidentifier '(' args_decl ')'
           {
                $$ = new ProcedureHeadNode($1, $3);
           }
           ;

proc_call: pidentifier '(' args ')'
           {
              $$ = new ProcedureCallNode($1, $3); 
           }
           ;

declarations: declarations ',' pidentifier
              {
                  $1->declarations.push_back(new DeclarationNode(new IdentifierNode($3),-1));
                  $$ = $1;
              }
            | declarations ',' pidentifier '[' num ']'
              { 
                  $1->declarations.push_back(new DeclarationNode(new IdentifierNode($3),$5));
                  $$ = $1;
              }
            | pidentifier
              {
                  $$ = new DeclarationsNode();
                  $$->declarations.push_back(new DeclarationNode(new IdentifierNode($1),-1));
              }
            | pidentifier '[' num ']'
              {
                  $$ = new DeclarationsNode();
                  $$->declarations.push_back(new DeclarationNode(new IdentifierNode($1),$3));
              }
            ;

args_decl: args_decl ',' pidentifier
           {
              $1->arguments.push_back(new ArgumentNode($3,false));
              $$ = $1;
           }
         | args_decl ',' T pidentifier
           {
              $1->arguments.push_back(new ArgumentNode($4,true));
              $$ = $1;
           }
         | pidentifier
           {
              $$ = new ArgumentsNode();
              $$->arguments.push_back(new ArgumentNode($1,false));
           }
         | T pidentifier
           {
              $$ = new ArgumentsNode();
              $$->arguments.push_back(new ArgumentNode($2,true));
           }
         ;

args: args ',' pidentifier
      {
          $1->args.push_back(new IdentifierNode($3));
          $$ = $1;
      }
    | pidentifier
      {
          $$ = new ProcedureCallArguments();
          $$->args.push_back(new IdentifierNode($1));
      }
    ;

expression: value
            {
                $$ = $1;
            }
          | value ADD value
            {
                $$ = new BinaryExpressionNode($1,$3,"+");
            }
          | value SUB value
            {
                $$ = new BinaryExpressionNode($1,$3,"-");
            }
          | value MUL value
            {
                $$ = new BinaryExpressionNode($1,$3,"*");
            }
          | value DIV value
            {
                $$ = new BinaryExpressionNode($1,$3,"/");
            }
          | value MOD value
            {
                $$ = new BinaryExpressionNode($1,$3,"%");
            }
          ;

condition: value EQ value
           {
              $$ = new ConditionNode($1,$3,"=");
           }
         | value NEQ value
           {
              $$ = new ConditionNode($1,$3,"!=");
           }
         | value GT value
           {
              $$ = new ConditionNode($1,$3,">");
           }
         | value LT value
           {
              $$ = new ConditionNode($1,$3,"<");
           }
         | value GEQ value
           {
              $$ = new ConditionNode($1,$3,">=");
           }
         | value LEQ value
           {
              $$ = new ConditionNode($1,$3,"<=");
           }
         ;

value: num
       {
          $$ = new ValueNode($1);
       }
     | identifier
       {
          $$ = $1;
       }
     ;

identifier: pidentifier
            {
                $$ = new IdentifierNode($1);
            }
          | pidentifier '[' num ']'
            {
                $$ = new IdentifierNode($1, new ValueNode($3));
            }
          | pidentifier '[' pidentifier ']'
            {
                $$ = new IdentifierNode($1, new IdentifierNode($3));
            }
          ;

%%

int main(int argc, char** argv) {
    std::shared_ptr<io_handler> io = std::make_shared<io_handler>(argc, argv);
    yyin = io->read_code();

    if(yyin == nullptr) {
            return -1;
    }

    std::cout << "### \x1b[34mCompiler is running...\x1b[0m ###" << std::endl;

    yyparse();
    SemanticAnalyser* visitor = new SemanticAnalyser();
    root->accept(visitor);
    visitor->intermediate_form->print(0);
    CodeGenerator* cg = new CodeGenerator(visitor->intermediate_form);
    cg->visit(cg->itermediate_form);
    CodeFormater* cf = new CodeFormater(cg->end_result);
    cf->transform(cf->program);
    io->print_code(cf->end_res);
    return 0;
}