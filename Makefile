CC = g++
LEX = flex 
YACC = yacc

CC_FLAGS = -g -Wall -Wno-switch
LD_FLAGS = -g -Wall -Wno-switch
CXX_FLAGS = -g -Wall -Wno-switch
LEX_FLAGS =  
YACC_FLAGS = -d --debug --verbose

# Information specific to the lex file
LEX_FILES = grammar/nel.l 
LEX_OUTPUT = grammar/lex.yy.c
LEX_HEADER = grammar/lex.yy.h

# Information specific to the yacc file
YACC_FILES = grammar/nel.y
YACC_OUTPUT = grammar/y.tab.cpp

# Information common to both lex and yacc
COMMON_HEADER = grammar/common.h

# AST related.
# Information specific to AST source code.
AST_HEADERS = \
	ast/argument.h \
	ast/ast.h \
	ast/attribute.h \
	ast/block_statement.h \
	ast/branch_condition.h \
	ast/branch_statement.h \
	ast/command.h \
	ast/command_statement.h \
	ast/constant_declaration.h \
	ast/constant_definition.h \
	ast/data_item.h \
	ast/data_statement.h \
	ast/definition.h \
	ast/embed_statement.h \
	ast/error.h \
	ast/expression.h \
	ast/header_setting.h \
	ast/header_statement.h \
	ast/label_declaration.h \
	ast/label_definition.h \
	ast/list_node.h \
	ast/map.h \
	ast/node.h \
	ast/number_node.h \
	ast/operation.h \
	ast/path.h \
	ast/package_definition.h \
	ast/relocation_statement.h \
	ast/rom_bank.h \
	ast/rom_generator.h \
	ast/set.h \
	ast/source_file.h \
	ast/source_position.h \
	ast/statement.h \
	ast/string_node.h \
	ast/symbol_table.h \
	ast/variable_declaration.h \
	ast/variable_definition.h
	
AST_OBJS = \
	ast/argument.o \
	ast/attribute.o \
	ast/block_statement.o \
	ast/branch_condition.o \
	ast/branch_statement.o \
	ast/command.o \
	ast/command_statement.o \
	ast/constant_declaration.o \
	ast/constant_definition.o \
	ast/data_item.o \
	ast/data_statement.o \
	ast/embed_statement.o \
	ast/error.o \
	ast/expression.o \
	ast/header_setting.o \
	ast/header_statement.o \
	ast/label_declaration.o \
	ast/label_definition.o \
	ast/operation.o \
	ast/path.o \
	ast/package_definition.o \
	ast/relocation_statement.o \
	ast/rom_bank.o \
	ast/rom_generator.o \
	ast/source_file.o \
	ast/source_position.o \
	ast/symbol_table.o \
	ast/variable_declaration.o \
	ast/variable_definition.o


# Extra files which get created during the process
EXTRA_FILES = grammar/y.output grammar/y.tab.hpp

# What the final application should be called
PARSER_OUTPUT = nel

# Makes sure all of the options are created
all: $(PARSER_OUTPUT)

%.o:%.cpp
	$(CC) $(CXX_FLAGS) $< -c -o $@

$(LEX_OUTPUT): $(LEX_FILES) $(AST_HEADERS) $(COMMON_HEADER) $(AST_OBJS)
	$(LEX) -o $(LEX_OUTPUT) $(LEX_FLAGS) $(LEX_FILES)
	
$(LEX_HEADER): $(LEX_FILES) $(AST_HEADERS) $(COMMON_HEADER) $(AST_OBJS)
	$(LEX) --header-file=$(LEX_HEADER) $(LEX_FLAGS) $(LEX_FILES)

$(YACC_OUTPUT): $(LEX_HEADER) $(YACC_FILES) $(AST_HEADERS) $(COMMON_HEADER) $(AST_OBJS)
	$(YACC) $(YACC_FLAGS) -o $(YACC_OUTPUT) $(YACC_FILES)

$(PARSER_OUTPUT): $(LEX_OUTPUT) $(YACC_OUTPUT) $(AST_OBJS)
	$(CC) $(LEX_OUTPUT) $(YACC_OUTPUT) $(AST_OBJS) $(CC_FLAGS) -lfl -o $(PARSER_OUTPUT) -Wno-sign-compare -Wno-unused-function

# Clean up the directory
clean:
	rm -rf $(LEX_OUTPUT) $(YACC_OUTPUT) $(PARSER_OUTPUT) $(AST_OBJS) $(EXTRA_FILES)

