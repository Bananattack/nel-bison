%{

#include "common.h"
#include "y.tab.hpp"
#include "lex.yy.h"
#include <fstream>

#ifdef _MSC_VER
// bison generates a switch statement which only has
// a default and no case. MSVC warns. Let's just shut it up.
#pragma warning(disable:4065)
#endif

%}

/* Give verbose error messages */
%error-verbose

%token IDENTIFIER "identifier"
%token NUMBER "number"
%token STRING "string literal"

%token KW_INES "`ines`"
%token KW_DEF "`def`"
%token KW_LET "`let`"
%token KW_GOTO "`goto`"
%token KW_WHEN "`when`"
%token KW_CALL "`call`"
%token KW_RETURN "`return`"
%token KW_RTI "`rti`"
%token KW_NOP "`nop`"
%token KW_GET "`get`"
%token KW_PUT "`put`"
%token KW_ADD "`add`"
%token KW_ADDC "`addc`"
%token KW_SUB "`sub`"
%token KW_SUBC "`subc`"
%token KW_OR "`or`"
%token KW_AND "`and`"
%token KW_XOR "`xor`"
%token KW_COMPARE "`compare`"
%token KW_BIT "`bit`"
%token KW_IS "`is`"
%token KW_SET "`set`"
%token KW_UNSET "`unset`"
%token KW_SIGNED "`signed`"
%token KW_UNSIGNED "`unsigned`"
%token KW_INC "`inc`"
%token KW_DEC "`dec`"
%token KW_NOT "`not`"
%token KW_NEG "`neg`"
%token KW_SHL "`shl`"
%token KW_SHR "`shr`"
%token KW_ROL "`rol`"
%token KW_ROR "`ror`"
%token KW_PUSH "`push`"
%token KW_PULL "`pull`"
%token KW_ROM "`rom`"
%token KW_BANK "`bank`"
%token KW_RAM "`ram`"
%token KW_BYTE "`byte`"
%token KW_WORD "`word`"
%token KW_VAR "`var`"
%token KW_BEGIN "`begin`"
%token KW_END "`end`"
%token KW_EMBED "`embed`"
%token KW_REQUIRE "`require`"
%token KW_PACKAGE "`package`"

%token PUNC_SET "`=`"
%token PUNC_COLON "`:`"
%token PUNC_COMMA "`,`"
%token PUNC_DOT "`.`"
%token PUNC_SEMI "`;`"
%token PUNC_EXCLAIM "`!`"
%token PUNC_HASH "`#`"
%token PUNC_AT "`@`"
%token PUNC_LBRACKET "`[`"
%token PUNC_RBRACKET "`]`"
%token PUNC_LPAREN "`(`"
%token PUNC_RPAREN "`)`"
%token PUNC_LBRACE "`{`"
%token PUNC_RBRACE "`}`"

%token PUNC_EQ "`==`"
%token PUNC_NEQ "`!=`"
%token PUNC_LT "`<`"
%token PUNC_LE "`<=`"
%token PUNC_GT "`>`"
%token PUNC_GE "`>=`"

%token OP_ADD "`+`"
%token OP_SUB "`-`"
%token OP_MUL "`*`"
%token OP_DIV "`/`"
%token OP_MOD "`%`"
%token OP_SHL "`<<`"
%token OP_SHR "`>>`"
%token OP_AND "`&`"
%token OP_XOR "`^`"
%token OP_OR "`|`"

%token END 0 "end-of-file"

%token INVALID_CHAR "invalid character"
%token UNTERMINATED_STRING "unterminated string literal"

/* 
    Destructor rules.
    WHY WHY WHY DO I HAVE TO DEFINE EVERY NODE OH GOD
    
    I recommend doing a Find All for ":" and then writing each nonterminal entry that way,
    better than actually scrolling around. Still godawful.
 */
%destructor {
                // Only do cleanup here if we didn't successfully parse. Otherwise, let me do the cleanup.
                if(!startNode)
                {
                    // delete $$;
                }
            }
            // TODO: majorly fix memory cleanup problems.
            program statement_list statement label_declaration constant_declaration var_declaration
            opt_size goto_statement goto_term relocate_statement data_statement data_list data_term command_statement
            when_condition condition command_list command
            argument numeric_term expr opt_register_indexing
            IDENTIFIER NUMBER STRING

/* Start node */
%start program

%%

program:
    statement_list
        {
            startNode = new nel::BlockStatement(nel::BlockStatement::MAIN, NEL_CAST(nel::ListNode<nel::Statement*>*, $1), NEL_GET_SOURCE_POS);
            $$ = startNode;
            YYACCEPT;
        }
    ;
    
statement_list:
    statement_list statement
        {
            // Ignore any empty statements.
            if($2)
            {
                nel::ListNode<nel::Statement*>* list = NEL_CAST(nel::ListNode<nel::Statement*>*, $1);
                list->getList().push_back(NEL_CAST(nel::Statement*, $2));
            }
            $$ = $1;
        }
    | /* empty */
        {
            $$ = new nel::ListNode<nel::Statement*>(NEL_GET_SOURCE_POS);
        }     
    ;

statement:
    block_statement
        {
            $$ = $1;
        }
    | require_statement
        {
            $$ = $1;
        }
    | embed_statement
        {
            $$ = $1;
        }
    | header_statement
        {
            $$ = $1;
        }
    | label_declaration
        {
            $$ = $1;
        }
    | constant_declaration
        {
            $$ = $1;
        }
    | var_declaration
        {
            $$ = $1;
        }
    | goto_statement
        {
            $$ = $1;
        }
    | relocate_statement
        {
            $$ = $1; 
        }
    | command_statement
        {
            $$ = $1;
        }
    | data_statement
        { 
            $$ = $1;
        }
    | error
        {
            $$ = 0;
        }
    | PUNC_SEMI
        {
            // Semi-colons are just for syntax comfort to some people.
            // However, these are actually empty statements that take no ROM space.
            $$ = 0; 
        }
    ;

block_statement:
    KW_BEGIN statement_list KW_END
        {
            $$ = new nel::BlockStatement(nel::BlockStatement::SCOPE, NEL_CAST(nel::ListNode<nel::Statement*>*, $2), NEL_GET_SOURCE_POS);
        }
    | KW_PACKAGE IDENTIFIER statement_list KW_END
        {
			$$ = new nel::BlockStatement(nel::BlockStatement::SCOPE, NEL_CAST(nel::StringNode*, $2), NEL_CAST(nel::ListNode<nel::Statement*>*, $3), NEL_GET_SOURCE_POS);
        }
    ;

require_statement:
    KW_REQUIRE STRING
        {
            $$ = 0;
            // Lookup the relative path to the required input file based on the current source, and add it to the input stack.
            pushInputFile(std::string(nel::getDirectory(currentPosition->getSourceFile()->getFilename()) + NEL_CAST(nel::StringNode*, $2)->getValue()).c_str());
        }
    ;

embed_statement:
    KW_EMBED STRING
        {
            $$ = new nel::EmbedStatement(NEL_CAST(nel::StringNode*, $2), NEL_GET_SOURCE_POS);
        }
    ;

header_statement:
    KW_INES PUNC_COLON header_segment_list
        {
            $$ = new nel::HeaderStatement(NEL_CAST(nel::ListNode<nel::HeaderSetting*>*, $3), NEL_GET_SOURCE_POS);
        }
    ;
    
header_segment_list:
    header_segment_list PUNC_COMMA header_segment
        {
            nel::ListNode<nel::HeaderSetting*>* list = NEL_CAST(nel::ListNode<nel::HeaderSetting*>*, $1);
            list->getList().push_back(NEL_CAST(nel::HeaderSetting*, $3));
            $$ = $1;
        }
    | header_segment
        {
            $$ = new nel::ListNode<nel::HeaderSetting*>(NEL_CAST(nel::HeaderSetting*, $1), NEL_GET_SOURCE_POS);
        }
    ;

header_segment:
    IDENTIFIER PUNC_SET expr
        {
            $$ = new nel::HeaderSetting(NEL_CAST(nel::StringNode*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS);
        }
    ;
    

label_declaration:
    KW_DEF IDENTIFIER PUNC_COLON
        {
            $$ = new nel::LabelDeclaration(NEL_CAST(nel::StringNode*, $2), NEL_GET_SOURCE_POS);
        }
    ;

constant_declaration:
    KW_LET IDENTIFIER PUNC_SET expr
        {
            $$ = new nel::ConstantDeclaration(NEL_CAST(nel::StringNode*, $2), NEL_CAST(nel::Expression*, $4), NEL_GET_SOURCE_POS);
        }
    ;

var_declaration:
    KW_VAR identifier_list PUNC_COLON KW_BYTE opt_size
        {
            $$ = new nel::VariableDeclaration(nel::VariableDeclaration::BYTE, NEL_CAST(nel::ListNode<nel::StringNode*>*, $2), NEL_CAST(nel::Expression*, $5), NEL_GET_SOURCE_POS);
        }
    | KW_VAR identifier_list PUNC_COLON KW_WORD opt_size
        {
            $$ = new nel::VariableDeclaration(nel::VariableDeclaration::WORD, NEL_CAST(nel::ListNode<nel::StringNode*>*, $2), NEL_CAST(nel::Expression*, $5), NEL_GET_SOURCE_POS);
        }
    ;
    
identifier_list:
    identifier_list PUNC_COMMA IDENTIFIER
        {
            nel::ListNode<nel::StringNode*>* list = NEL_CAST(nel::ListNode<nel::StringNode*>*, $1);
            list->getList().push_back(NEL_CAST(nel::StringNode*, $3));
            $$ = $1;
        }
    | IDENTIFIER
        {
            $$ = new nel::ListNode<nel::StringNode*>(NEL_CAST(nel::StringNode*, $1), NEL_GET_SOURCE_POS);
        }
    ;

opt_size:
    PUNC_LBRACKET expr PUNC_RBRACKET
        {
            $$ = NEL_CAST(nel::Expression*, $2);
        }
    | /* empty */
        {
            $$ = 0;
        }
    ;

goto_statement:
    KW_GOTO goto_term when_condition
        {
            $$ = new nel::BranchStatement(nel::BranchStatement::GOTO, NEL_CAST(nel::Argument*, $2), NEL_CAST(nel::BranchCondition*, $3), NEL_GET_SOURCE_POS);
        }
    | KW_CALL goto_term
        {
            $$ = new nel::BranchStatement(nel::BranchStatement::CALL, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS);
        }
    | KW_RETURN
        {
            $$ = new nel::BranchStatement(nel::BranchStatement::RETURN, NEL_GET_SOURCE_POS);
        }
    | KW_RTI
        {
            $$ = new nel::BranchStatement(nel::BranchStatement::RTI, NEL_GET_SOURCE_POS);
        }
    | KW_NOP
        {
            $$ = new nel::BranchStatement(nel::BranchStatement::NOP, NEL_GET_SOURCE_POS);
        }
    ;

goto_term:
    expr
        {
            $$ = new nel::Argument(nel::Argument::LABEL, NEL_CAST(nel::Expression*, $1), NEL_GET_SOURCE_POS);
        }
    | PUNC_LBRACKET expr PUNC_RBRACKET
        {
            $$ = new nel::Argument(nel::Argument::INDIRECT_LABEL, NEL_CAST(nel::Expression*, $2), NEL_GET_SOURCE_POS);
        }
    ;

relocate_statement:
    KW_ROM KW_BANK expr bank_optional_origin PUNC_COLON
        {
            $$ = new nel::RelocationStatement(nel::RelocationStatement::ROM, NEL_CAST(nel::Expression*, $3), NEL_CAST(nel::Expression*, $4), NEL_GET_SOURCE_POS);
        }
    | KW_ROM expr PUNC_COLON
        {
            $$ = new nel::RelocationStatement(nel::RelocationStatement::ROM, 0, NEL_CAST(nel::Expression*, $2), NEL_GET_SOURCE_POS);
        }
    | KW_RAM expr PUNC_COLON
        {
            $$ = new nel::RelocationStatement(nel::RelocationStatement::RAM, 0, NEL_CAST(nel::Expression*, $2), NEL_GET_SOURCE_POS);
        }
    ;
    
bank_optional_origin:
    PUNC_COMMA expr
        {
            $$ = $2;
        }
    | /* empty */
        {
            $$ = 0;
        }

data_statement:
    /* Take data list, where element is a single byte (allowing values of up to 256). */
    KW_BYTE PUNC_COLON data_list
        {
            $$ = new nel::DataStatement(nel::DataStatement::BYTE, NEL_CAST(nel::ListNode<nel::DataItem*>*, $3), NEL_GET_SOURCE_POS);
        }
    /* Take data list, like above, but everything takes 2 bytes (allowing values of up to 65536). */
    | KW_WORD PUNC_COLON data_list
        {
            $$ = new nel::DataStatement(nel::DataStatement::WORD, NEL_CAST(nel::ListNode<nel::DataItem*>*, $3), NEL_GET_SOURCE_POS);
        }
    ;

data_list:
    data_list PUNC_COMMA data_term
        {
            nel::ListNode<nel::DataItem*>* list = NEL_CAST(nel::ListNode<nel::DataItem*>*, $1);
            list->getList().push_back(NEL_CAST(nel::DataItem*, $3));
            $$ = $1;
        }
    | data_term
        {
            $$ = new nel::ListNode<nel::DataItem*>(NEL_CAST(nel::DataItem*, $1), NEL_GET_SOURCE_POS);
        }
    ;

data_term:
    STRING
        {
            $$ = new nel::DataItem(NEL_CAST(nel::StringNode*, $1), NEL_GET_SOURCE_POS);
        }
    | expr
        {
            $$ = new nel::DataItem(NEL_CAST(nel::Expression*, $1), NEL_GET_SOURCE_POS);
        }
    ;

when_condition:
    KW_WHEN condition
        {
            $$ = $2;
        }
    | /* empty */
        {
            $$ = 0;
        }
    ;

condition:
    IDENTIFIER
        {
            nel::StringNode* id = NEL_CAST(nel::StringNode*, $1);
            nel::Argument::ArgumentType argType = nel::Argument::resolveUnprefixedBuiltinType(id->getValue());
            if(argType == nel::Argument::INVALID)
            {
                std::ostringstream os;
                os << "expected a p-flag, not `" << id->getValue() << "`.";
                nel::error(os.str(), currentPosition);
            }
            nel::Argument* arg = new nel::Argument(argType, NEL_GET_SOURCE_POS);
            $$ = new nel::BranchCondition(nel::BranchCondition::CONDITION_SET, arg, NEL_GET_SOURCE_POS);
        }
    | KW_NOT IDENTIFIER
        {
            nel::StringNode* id = NEL_CAST(nel::StringNode*, $2);
            nel::Argument::ArgumentType argType = nel::Argument::resolveUnprefixedBuiltinType(id->getValue());
            if(argType == nel::Argument::INVALID)
            {
                std::ostringstream os;
                os << "expected a p-flag, not `" << id->getValue() << "`.";
                nel::error(os.str(), currentPosition);
            }
            nel::Argument* arg = new nel::Argument(argType, NEL_GET_SOURCE_POS);
            $$ = new nel::BranchCondition(nel::BranchCondition::CONDITION_UNSET, arg, NEL_GET_SOURCE_POS);
        }
    ;
    
command_statement:
    argument PUNC_COLON command_list
        {
            $$ = new nel::CommandStatement(NEL_CAST(nel::Argument*, $1), NEL_CAST(nel::ListNode<nel::Command*>*, $3), NEL_GET_SOURCE_POS);
        }
    ;

command_list:
    command_list PUNC_COMMA command
        {
            nel::ListNode<nel::Command*>* list = NEL_CAST(nel::ListNode<nel::Command*>*, $1);
            list->getList().push_back(NEL_CAST(nel::Command*, $3));
            $$ = $1;
        }
    | command
        {
            $$ = new nel::ListNode<nel::Command*>(NEL_CAST(nel::Command*, $1), NEL_GET_SOURCE_POS);
        }
    ;

command:
    /* These require an argument (error if there is none) */
    KW_GET argument          { $$ = new nel::Command(nel::Command::GET, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_PUT argument        { $$ = new nel::Command(nel::Command::PUT, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_ADD argument        { $$ = new nel::Command(nel::Command::ADD, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_ADDC argument       { $$ = new nel::Command(nel::Command::ADDC, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_SUB argument        { $$ = new nel::Command(nel::Command::SUB, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_SUBC argument       { $$ = new nel::Command(nel::Command::SUBC, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_OR argument         { $$ = new nel::Command(nel::Command::BITWISE_OR, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_AND argument        { $$ = new nel::Command(nel::Command::BITWISE_AND, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_XOR argument        { $$ = new nel::Command(nel::Command::BITWISE_XOR, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_COMPARE argument    { $$ = new nel::Command(nel::Command::CMP, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_BIT argument        { $$ = new nel::Command(nel::Command::BIT, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    /* These require a p-flag (error if there is none) */
    | KW_SET argument        { $$ = new nel::Command(nel::Command::SET, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    | KW_UNSET argument      { $$ = new nel::Command(nel::Command::UNSET, NEL_CAST(nel::Argument*, $2), NEL_GET_SOURCE_POS); }
    /* These require no term (error if there is one) */
    | KW_INC    { $$ = new nel::Command(nel::Command::INC, NEL_GET_SOURCE_POS); }
    | KW_DEC    { $$ = new nel::Command(nel::Command::DEC, NEL_GET_SOURCE_POS); }
    | KW_NOT    { $$ = new nel::Command(nel::Command::NOT, NEL_GET_SOURCE_POS); }
    | KW_NEG    { $$ = new nel::Command(nel::Command::NEG, NEL_GET_SOURCE_POS); }
    | KW_SHL    { $$ = new nel::Command(nel::Command::SHL, NEL_GET_SOURCE_POS); }
    | KW_SHR    { $$ = new nel::Command(nel::Command::SHR, NEL_GET_SOURCE_POS); }
    | KW_ROL    { $$ = new nel::Command(nel::Command::ROL, NEL_GET_SOURCE_POS); }
    | KW_ROR    { $$ = new nel::Command(nel::Command::ROR, NEL_GET_SOURCE_POS); }
    | KW_PUSH   { $$ = new nel::Command(nel::Command::PUSH, NEL_GET_SOURCE_POS); }
    | KW_PULL   { $$ = new nel::Command(nel::Command::PULL, NEL_GET_SOURCE_POS); }
    | IDENTIFIER
        {
            nel::StringNode* id = NEL_CAST(nel::StringNode*, $1);
            
            std::ostringstream os;
            os << "expected a command, not `" << id->getValue() << "`.";
            
            $$ = new nel::Command(nel::Command::INVALID, NEL_GET_SOURCE_POS);
            
            nel::error(os.str(), currentPosition);
        }
    ;
    
argument:
    /* A register or p-flag */
    IDENTIFIER
        {
            nel::StringNode* id = NEL_CAST(nel::StringNode*, $1);
            nel::Argument::ArgumentType argType = nel::Argument::resolveUnprefixedBuiltinType(id->getValue());
            if(argType == nel::Argument::INVALID)
            {
                std::ostringstream os;
                os << "expected a register or p-flag, not `" << id->getValue() << "`. Did you forget an @ or # sign?";
                nel::error(os.str(), currentPosition);
            }
            $$ = new nel::Argument(argType, NEL_GET_SOURCE_POS);
        }
    /* An immediate value */
    | PUNC_HASH expr
        {
            $$ = new nel::Argument(nel::Argument::IMMEDIATE, NEL_CAST(nel::Expression*, $2), NEL_GET_SOURCE_POS);
        }
    /* A direct memory value, possibly indexed */
    | PUNC_AT expr opt_register_indexing
        {
            if($3)
            {
                nel::StringNode* index = NEL_CAST(nel::StringNode*, $3);
                nel::Argument::ArgumentType indexType = nel::Argument::resolveIndexedType(index->getValue());
                if(indexType == nel::Argument::INVALID)
                {
                    std::ostringstream os;
                    os << "term may only be indexed by x or y, not `" << index->getValue() << "`.";
                    nel::error(os.str(), currentPosition);
                }
                $$ = new nel::Argument(indexType, NEL_CAST(nel::Expression*, $2), NEL_GET_SOURCE_POS);
            }
            else
            {
                $$ = new nel::Argument(nel::Argument::DIRECT, NEL_CAST(nel::Expression*, $2), NEL_GET_SOURCE_POS);
            }
        }
    /* An indirect memory value, possibly indexed. (must be indexed appropriately for load/store type commands) */
    | PUNC_AT PUNC_LBRACKET expr opt_register_indexing PUNC_RBRACKET opt_register_indexing
        {
            if($4 && $6)
            {
                nel::error("an indirected term cannot be indexed both before and after indirection.", currentPosition);
                $$ = 0;
            }
            else if(!$4 && !$6)
            {
                nel::error("an indirected term must be indexed in some manner, either before indirection by x or after indirection by y.", currentPosition);
                $$ = 0;
            }
            else if($4)
            {
                nel::StringNode* preIndex = NEL_CAST(nel::StringNode*, $4);
                nel::Argument::ArgumentType indexType = nel::Argument::resolveIndexedType(preIndex->getValue());
                
                if(indexType == nel::Argument::INDEXED_BY_X)
                {
                    $$ = new nel::Argument(nel::Argument::ZP_INDEXED_INDIRECT, NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS);
                } 
                else
                {
                    std::ostringstream os;
                    os << "an indirected term may only be indexed by x before indirection, not by `" << preIndex->getValue() << "`.";
                    nel::error(os.str(), currentPosition);
                    $$ = 0;
                }
            }
            else if($6)
            {
                nel::StringNode* postIndex = NEL_CAST(nel::StringNode*, $6);
                nel::Argument::ArgumentType indexType = nel::Argument::resolveIndexedType(postIndex->getValue());
                
                if(indexType == nel::Argument::INDEXED_BY_Y)
                {
                    $$ = new nel::Argument(nel::Argument::ZP_INDIRECT_INDEXED, NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS);
                }
                else
                {
                    std::ostringstream os;
                    os << "an indirected term may only be indexed by y after indirection, not by `" << postIndex->getValue() << "`.";
                    nel::error(os.str(), currentPosition);
                    $$ = 0;
                }
            }
        }
    ;

numeric_term:
    /* A label name */
    attribute
        {
            $$ = new nel::Expression(new nel::Attribute(NEL_CAST(nel::ListNode<nel::StringNode*>*, $1), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS);
        }
    /* A literal number */
    | NUMBER
        {
            $$ = new nel::Expression(NEL_CAST(nel::NumberNode*, $1), NEL_GET_SOURCE_POS);
        }
    /* An arithmetic expression */
    | PUNC_LPAREN expr PUNC_RPAREN
        {
            $$ = NEL_CAST(nel::Expression*, $2);
        }
    ;

attribute:
    attribute PUNC_DOT IDENTIFIER
        {
            nel::ListNode<nel::StringNode*>* list = NEL_CAST(nel::ListNode<nel::StringNode*>*, $1);
            list->getList().push_back(NEL_CAST(nel::StringNode*, $3));
            $$ = $1;
        }
    | IDENTIFIER
        {
            $$ = new nel::ListNode<nel::StringNode*>(NEL_CAST(nel::StringNode*, $1), NEL_GET_SOURCE_POS);
        }
    ;
    
/* TODO: Constant folding and label arithmetic and other fun. */
expr:
    bitwise_or_expr { $$ = $1; }
    ;

bitwise_or_expr:
    bitwise_or_expr OP_OR bitwise_xor_expr { $$ = new nel::Expression(new nel::Operation(nel::Operation::BITWISE_OR, NEL_CAST(nel::Expression*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS); }
    | bitwise_xor_expr { $$ = $1; }
    ;

bitwise_xor_expr:
    bitwise_xor_expr OP_XOR bitwise_and_expr { $$ = new nel::Expression(new nel::Operation(nel::Operation::BITWISE_XOR, NEL_CAST(nel::Expression*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS); }
    | bitwise_and_expr { $$ = $1; }
    ;
    
bitwise_and_expr:
    bitwise_and_expr OP_AND shift_expr { $$ = new nel::Expression(new nel::Operation(nel::Operation::BITWISE_AND, NEL_CAST(nel::Expression*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS); }
    | shift_expr { $$ = $1; }
    ;
    
shift_expr:
    shift_expr OP_SHL additive_expr { $$ = new nel::Expression(new nel::Operation(nel::Operation::SHL, NEL_CAST(nel::Expression*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS); }
    | shift_expr OP_SHR additive_expr { $$ = new nel::Expression(new nel::Operation(nel::Operation::SHR, NEL_CAST(nel::Expression*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS); }
    | additive_expr { $$ = $1; }
    ;

additive_expr:
    additive_expr OP_ADD multiplicative_expr { $$ = new nel::Expression(new nel::Operation(nel::Operation::ADD, NEL_CAST(nel::Expression*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS); }
    | additive_expr OP_SUB multiplicative_expr { $$ = new nel::Expression(new nel::Operation(nel::Operation::SUB, NEL_CAST(nel::Expression*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS); }
    | multiplicative_expr { $$ = $1; }
    ;
multiplicative_expr:
    multiplicative_expr OP_MUL numeric_term { $$ = new nel::Expression(new nel::Operation(nel::Operation::MUL, NEL_CAST(nel::Expression*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS); }
    | multiplicative_expr OP_DIV numeric_term { $$ = new nel::Expression(new nel::Operation(nel::Operation::DIV, NEL_CAST(nel::Expression*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS); }
    | multiplicative_expr OP_MOD numeric_term { $$ = new nel::Expression(new nel::Operation(nel::Operation::MOD, NEL_CAST(nel::Expression*, $1), NEL_CAST(nel::Expression*, $3), NEL_GET_SOURCE_POS), NEL_GET_SOURCE_POS); }
    | numeric_term { $$ = $1; }
    ;
    
opt_register_indexing:
    /* Indexed by register x or y */
    PUNC_LBRACKET IDENTIFIER PUNC_RBRACKET
        {
            $$ = $2;
        }
    | /* empty */
        {
            $$ = 0;
        }
    ;

%%

nel::SourcePosition* currentPosition = 0;
std::vector<nel::SourcePosition*> includeStack;
nel::BlockStatement* startNode = 0;
std::string stringContent;
char stringTerminator;

void yyerror(const char* message)
{
    nel::error(message, currentPosition);
}

bool aggregate()
{
    std::cerr << "- first pass (aggregation)..." << std::endl;
    startNode->aggregate();
    return !nel::errorCount;
}

bool validate()
{
    std::cerr << "- second pass (validation)..." << std::endl;
    startNode->validate();
    return !nel::errorCount;
}

bool generate()
{
    std::cerr << "- third pass (generation)..." << std::endl;
    nel::romGenerator->resetRomPosition();
    startNode->generate();
    return !nel::errorCount;
}

void printUsage(const char* msg = 0)
{
    if(msg)
    {
        std::cerr << "* " << nel::PROGRAM_NAME << ": " << msg << std::endl << std::endl;
    }

    std::cerr << "usage: " << nel::PROGRAM_NAME << " filename" << std::endl;
    std::cerr << "  where `filename` is a nel source file to compile." << std::endl;
}

bool pushInputFile(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    
    nel::SourcePosition* includePoint = currentPosition;
    
    if(f)
    {   
        // Modify the source position info.
        if(includePoint)
        {
            // Save position on stack
            includeStack.push_back(includePoint);
            
            // Set up new position in included file.
            currentPosition = new nel::SourcePosition(new nel::SourceFile(filename, new nel::SourcePosition(includePoint)));
            
            // Too many includes? error.
            if(includeStack.size() > INCLUDE_STACK_MAX)
            {
                std::ostringstream os;
                os << "too many nested requires (exceeded max depth " <<
                    INCLUDE_STACK_MAX << "). are there mutually-dependent source files?";
                for(size_t i = 0; i < includeStack.size(); i++)
                {
                    nel::SourcePosition* entry = includeStack[i];
                    os << std::endl << "    at "; 
                    entry->print(os, true);
                }
                {
                    nel::SourcePosition* entry = currentPosition;
                    os << std::endl << "    at "; 
                    entry->print(os, true);
                }
                nel::error(os.str(), includePoint, true);
            }
        }
        else
        {
            // First file, probably.
            currentPosition = new nel::SourcePosition(new nel::SourceFile(filename));
        }
        
        // Switch the input stream for the lexer.
        yyin = f;
        yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
        
        return true;
    }
    else
    {
        // If there is at least one file opened, we should use typical error reporting.
        // Otherwise, the main program should use the return value to determine the outcome.
        if(currentPosition)
        {
            std::ostringstream os;
            os << "could not open file '" << filename << "' which was included here.";
            nel::error(os.str(), currentPosition);
        }
        return false;
    }
}

bool popInputFile()
{
    // No source information left to pop.
    if(!currentPosition || includeStack.empty())
    {
        return false;
    }
    else
    {
        // Delete current source position from stack.
        delete currentPosition;
        
        // Pop back to previous position.
        currentPosition = includeStack.back();
        includeStack.pop_back();
        
        return true;
    }
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printUsage("insufficient arguments");
        return 1;
    }
    
    std::cerr << "* " << nel::PROGRAM_NAME << ": compiling..." << std::endl;
    if(!pushInputFile(argv[1]))
    {
        std::cerr << "* " << nel::PROGRAM_NAME << ": fatal: could not open file '" << argv[1] << "' which was provided on command line." << std::endl;
        return 1;
    }

    nel::errorCount = 0;
    currentPosition = new nel::SourcePosition(new nel::SourceFile(argv[1]));
    stringContent = "";
    stringTerminator = 0;
    startNode = 0;

    
    int result = yyparse();
    if(result || nel::errorCount)
    {
        std::cerr << "* " << nel::PROGRAM_NAME << ": failed compilation with " << nel::errorCount << " error(s)." << std::endl;
    }
    else
    {
        bool success = aggregate() && validate() && generate();
        if(success)
        {
            const char* const FILENAME = "out.nes";
        
            std::cerr << "* saving ROM..." << std::endl;
            std::ofstream file(FILENAME, std::ios::out | std::ios::binary);
            if(file.is_open())
            {
                nel::romGenerator->outputToStream(file);
                std::cerr << "* " << nel::PROGRAM_NAME << ": wrote to '" << FILENAME << "'." << std::endl;
                file.close();
            }
            else
            {
                std::cerr << "* " << nel::PROGRAM_NAME << ": failed to open '" << FILENAME << "' for writing." << std::endl;
                exit(1);
            }
            
            std::cerr << "* " << nel::PROGRAM_NAME << ": compilation complete." << std::endl;
        }
        else
        {
            nel::failAndExit();
        }
    }

    return 0;
}

