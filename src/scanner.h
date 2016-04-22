/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       scanner.h
 * Autor(i):    Dusan Madarka
 *
 * Hlavickovy subor lexikalneho analyzatora.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_SCANNER_H_
#define IFJ11_SCANNER_H_

#include "str.h"

/**
 * typy tokenov
 */
enum token_id
{
    TKN_ID = 0,
    TKN_NUMBER,
    TKN_STRING,

    TKN_BRACKET_L,
    TKN_BRACKET_R,

    TKN_COMMA,
    TKN_SEMICOL,

    TKN_OP_ASSIGNMENT,
    TKN_OP_POWER,
    TKN_OP_MUL,
    TKN_OP_DIV,
    TKN_OP_ADD,
    TKN_OP_SUB,
    TKN_OP_CONC,
    TKN_OP_LESS,
    TKN_OP_GREATER,
    TKN_OP_LESS_EQUAL,
    TKN_OP_GREATER_EQUAL,
    TKN_OP_NOT_EQUAL,
    TKN_OP_EQUAL,

        /* rozsirenia */
        TKN_OP_MOD,
        TKN_OP_STRLEN,

    TKN_KW_DO,
    TKN_KW_IF,
    TKN_KW_THEN,
    TKN_KW_ELSE,
    TKN_KW_LOCAL,
    TKN_KW_TRUE,
    TKN_KW_END,
    TKN_KW_NIL,
    TKN_KW_WHILE,
    TKN_KW_FALSE,
    TKN_KW_READ,
    TKN_KW_WRITE,
    TKN_KW_FUNCTION,
    TKN_KW_RETURN,

    TKN_RW_AND,
    TKN_RW_BREAK,
    TKN_RW_ELSEIF,
    TKN_RW_FOR,
    TKN_RW_IN,
    TKN_RW_NOT,
    TKN_RW_OR,
    TKN_RW_REPEAT,
    TKN_RW_UNTIL,

    TKN_BI_TYPE,
    TKN_BI_SUBSTR,
    TKN_BI_FIND,
    TKN_BI_SORT,

    TKN_EOF,
};

/**
 * Union pre token
 */
typedef union token_value
{
    double num;
    char * str;
} Ttoken_value;

/**
 * Struktura pre token
 */
typedef struct Ttoken
{
    enum token_id id;
    Ttoken_value val;
} Ttoken;

bool scanner_next_token(Ttoken *token);
void scanner_special_words(Ttoken *token, Tstring *id);

#endif /* IFJ11_SCANNER_H_ */
