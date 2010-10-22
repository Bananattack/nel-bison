%{

#include <string>

#include "common.h"
#include "y.tab.hpp"

#define YY_USER_ACTION currentPosition->incrementColumn(yyleng);

%}

%x multi_comment
%x string_literal

%%

"ines"      return KW_INES;
"def"       return KW_DEF;
"let"       return KW_LET;
"goto"      return KW_GOTO;
"when"      return KW_WHEN;
"call"      return KW_CALL;
"return"    return KW_RETURN;
"rti"       return KW_RTI;
"nop"       return KW_NOP;
"get"       return KW_GET;
"put"       return KW_PUT;
"add"       return KW_ADD;
"addc"      return KW_ADDC;
"sub"       return KW_SUB;
"subc"      return KW_SUBC;
"or"        return KW_OR;
"and"       return KW_AND;
"xor"       return KW_XOR;
"compare"   return KW_COMPARE;
"bit"       return KW_BIT;
"is"        return KW_IS;
"set"       return KW_SET;
"unset"     return KW_UNSET;
"signed"    return KW_SIGNED;
"unsigned"  return KW_UNSIGNED;
"inc"       return KW_INC;
"dec"       return KW_DEC;
"not"       return KW_NOT;
"neg"       return KW_NEG;
"shl"       return KW_SHL;
"shr"       return KW_SHR;
"ror"       return KW_ROR;
"rol"       return KW_ROL;
"push"      return KW_PUSH;
"pull"      return KW_PULL;
"rom"       return KW_ROM;
"bank"      return KW_BANK;
"ram"       return KW_RAM;
"byte"      return KW_BYTE;
"word"      return KW_WORD;
"var"       return KW_VAR;
"begin"     return KW_BEGIN;
"end"       return KW_END;
"embed"     return KW_EMBED;
"require"   return KW_REQUIRE;
"package"   return KW_PACKAGE;

\=          return PUNC_SET;
\:          return PUNC_COLON;
\,          return PUNC_COMMA;
\.          return PUNC_DOT;
\!          return PUNC_EXCLAIM;
\;          return PUNC_SEMI;
\#          return PUNC_HASH;
\@          return PUNC_AT;
\[          return PUNC_LBRACKET;
\]          return PUNC_RBRACKET;
\(          return PUNC_LPAREN;
\)          return PUNC_RPAREN;
\{          return PUNC_LBRACE;
\}          return PUNC_RBRACE;

\+          return OP_ADD;
\-          return OP_SUB;
\*          return OP_MUL;
\/          return OP_DIV;
\%          return OP_MOD;
\<\<        return OP_SHL;
\>\>        return OP_SHR;
\&          return OP_AND;
\^          return OP_XOR;
\|          return OP_OR;

\=\=        return PUNC_EQ;
\!\=        return PUNC_NEQ;
\<          return PUNC_LT;
\<\=        return PUNC_LE;
\>          return PUNC_GT;
\>\=        return PUNC_GE;

[a-zA-Z_][a-zA-Z0-9_]*          {
                                    yylval = new nel::StringNode(std::string(yytext), NEL_GET_SOURCE_POS);
                                    return IDENTIFIER;
                                }

[1-9][0-9]*                     {
                                    yylval = new nel::NumberNode(stringToNumber(yytext, 10), NEL_GET_SOURCE_POS);
                                    return NUMBER;
                                }
0x([0-9a-fA-F]+)                {                        
                                    yylval = new nel::NumberNode(stringToNumber(yytext + 2, 16), NEL_GET_SOURCE_POS);
                                    return NUMBER;
                                }
0b([0-1]+)                      {
                                    yylval = new nel::NumberNode(stringToNumber(yytext + 2, 2), NEL_GET_SOURCE_POS);
                                    return NUMBER;
                                }
0[0-9]*                         {
                                    yylval = new nel::NumberNode(stringToNumber(yytext, 10), NEL_GET_SOURCE_POS);
                                    return NUMBER;
                                }

[\'\"]                          {
                                    stringTerminator = yytext[0];
                                    stringContent = "";
                                    BEGIN(string_literal);
                                }

\n          currentPosition->incrementLine(); /* Increase line count, emit nothing. */
[\r\t ]+    /* ignore whitespace */;
\/\/.*      /* ignore comment. */
"/*"        BEGIN(multi_comment);

<multi_comment>"*/"     BEGIN(INITIAL);
<multi_comment>[^\n*]*  /* gobble characters */
<multi_comment>"*"      /* single star. false alarm! */
<multi_comment>\n       currentPosition->incrementLine(); /* line counting is still significant in comments. */

<string_literal>[\'\"]          {
                                    if(yytext[0] == stringTerminator)
                                    {
                                        BEGIN(INITIAL);
                                        if(stringContent.size() == 1)
                                        {
                                            yylval = new nel::NumberNode((unsigned int) stringContent[0], NEL_GET_SOURCE_POS);
                                            return NUMBER;
                                        }
                                        else
                                        {
                                            yylval = new nel::StringNode(stringContent, NEL_GET_SOURCE_POS);
                                            return STRING;
                                        }
                                    }
                                    else
                                    {
                                        stringContent += yytext;
                                    }
                                }
<string_literal>\\n             { stringContent.append(1, '\n'); }
<string_literal>\\r             { stringContent.append(1, '\r'); }
<string_literal>\\0             { stringContent.append(1, '\0'); }
<string_literal>\\t             { stringContent.append(1, '\t'); }
<string_literal>\\b             { stringContent.append(1, '\b'); }
<string_literal>\\f             { stringContent.append(1, '\f'); }
<string_literal>\\v             { stringContent.append(1, '\v'); }
<string_literal>\\a             { stringContent.append(1, '\a'); }
<string_literal>\\\\            { stringContent.append(1, '\\'); }
<string_literal>\\\"            { stringContent.append(1, '\"'); }
<string_literal>\\\'            { stringContent.append(1, '\''); }
<string_literal>\\.             { yyerror("Invalid escape sequence in string."); }
<string_literal>[^\n\'\"\\]*    { stringContent += yytext; }
<string_literal>\n              { return UNTERMINATED_STRING; }


<<EOF>>                         {
                                    yypop_buffer_state();

                                    if(!YY_CURRENT_BUFFER)
                                    {
                                        yyterminate();
                                    }
                                    else
                                    {
                                        if(!popInputFile())
                                        {
                                            yyerror("internal: include stack underflow");
                                        }
                                    }
                                }

.           return INVALID_CHAR;

%%
