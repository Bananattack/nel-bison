#pragma once

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

#include "../ast/error.h"
#include "../ast/rom_generator.h"
#include "../ast/ast.h"

/**
 * The type returned by each terminal and rule.
 */
#define YYSTYPE nel::Node*

/**
 * yyin is the file stream which yacc and lex use.
 */
extern FILE* yyin;

/**
 * The current position in source. Used by lex and yacc.
 */
extern nel::SourcePosition* currentPosition;

/**
 * The stack of all included files.
 */
extern std::vector<nel::SourcePosition*> includeStack;
const unsigned int INCLUDE_STACK_MAX = 15; // Used by above as hard-coded upper limit in file include depth.

/**
 * The start node of the program. Set on a successful parse.
 */
extern nel::BlockStatement* startNode;

/**
 * Contains the content of a string literal being parsed.
 * Used by lex.
 */
extern std::string stringContent;

/**
 * The character used to terminate an active string literal.
 * Used by lex.
 */
extern char stringTerminator;

extern "C"
{
    /**
     * Lexically scans yyin.
     * Provided here so that yacc has this prototype handy.
     */
    int yylex();
    
    /**
     * lex and yacc use this for error reporting.
     */
    void yyerror(const char*);
}

/**
 * Performs a cast of x into type T.
 * In debug mode this is an error-checked cast.
 * In release builds this may later be replaced with an unchecked C-style cast.
 */
#define NEL_CAST(T, x)          NEL__cast<T>(x, __FILE__, __LINE__)

/**
 * Returns a copy of the current source position.
 */
#define NEL_GET_SOURCE_POS      new nel::SourcePosition(currentPosition)

/**
 * A function that converts a node into another type, and errors noisily if the cast fails.
 * Null values are simply passed through.
 */
template <typename T>
T NEL__cast(nel::Node* node, const char* file, int line)
{
    if(node == 0) return 0;
    else
    {
        T result = dynamic_cast<T>(node);
        if(!result)
        {
            char message[4096];
            sprintf(message, "Failed to cast '%s' node to '%s' in %s at line %d (%s:%d).",
                typeid(*node).name(), typeid(T).name(), file, line,
                currentPosition->getSourceFile()->getFilename().c_str(),
                currentPosition->getLine()
            );
            throw std::runtime_error(message); 
        }
        else
        {
            return result;
        }
    }
}

/**
 * Converts a string value with a given radix into an unsigned integer.
 * Errors if this exceeds the maximum word value.
 */
inline unsigned int stringToNumber(const char* text, unsigned int radix)
{
    long value = strtol(text, 0, radix);
    
    if(errno == ERANGE || (unsigned long) value > nel::Expression::MAX_VALUE)
    {
        std::ostringstream os;
        os << "Value " << text << " outside of representable range of 0..65535.";
        error(os.str(), currentPosition);
        return 0;
    }
    return (unsigned int) value;
}

bool pushInputFile(const char* filename);
bool popInputFile();

#ifdef _MSC_VER
#define YY_NO_UNISTD_H 1
#define isatty(X) (0)
#endif
