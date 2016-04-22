/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       scanner.c
 * Autor(i):    Dusan Madarka
 *
 * Implementacia lexikalneho analyzatora.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "ifj11.h"
#include "debug.h"
#include "scanner.h"
#include "str.h"
#include "fbuffer.h"
#include "xwrappers.h"

/**
 * stavy konecneho automatu
 */
enum automat_states
{
    ST_INIT = 0,
    ST_ID,
    ST_NUMBER_INT,
    ST_NUMBER_REAL,
    ST_NUMBER_REAL_L,
    ST_NUMBER_EXP,
    ST_NUMBER_EXP_L,
    ST_NUMBER_SIGN_EXP,
    ST_STRING,
    ST_STRING_ESC,
    ST_STRING_ESC_NUM_1L,
    ST_STRING_ESC_NUM_2L,
    ST_STRING_ESC_NUM_1G,
    ST_STRING_ESC_NUM_2G,
    ST_ASSIGN_OR_EQUAL,
    ST_CONC,
    ST_LESS,
    ST_GREATER,
    ST_NOT_EQUAL,
    ST_SUB,
    ST_COMMENT,
    ST_COMMENT_S,
    ST_COMMENT_BLOCK,
    ST_COMMENT_BLOCK_S,
    ST_COMMENT_BLOCK_E,
};

/**
 * klucove a rezervovane slova - tabulka
 */
const char *SPECIFIC_WORDS[] =
{
    /* Klucove slova */
    "do",
    "if",
    "then",
    "else",
    "local",
    "true",
    "end",
    "nil",
    "while",
    "false",
    "read",
    "write",
    "function",
    "return",

    /* Rezervovane slova */
    "and",
    "break",
    "elseif",
    "for",
    "in",
    "not",
    "or",
    "repeat",
    "until",

    /* Vstavane funkcie */
    "type",
    "substr",
    "find",
    "sort",
};


/**
 * vrati dalsi token
 * @param - argument odkaz na token
 * @return - navratova hodnota; true pri spravnom vytvoreni tokenu;
 *           false pri lexikalnej chybe
 */
bool scanner_next_token(Ttoken *token)
{
    enum automat_states state = ST_INIT;
    int c = 0;
    Tstring tmp;
    static int esc_seq = 0;

    /**
     * Nekonecny cyklus prechadzania suboru.
     */
    for (;;)
    {
        /* s pouzitim fbuffer pouzije spravny znak */
        c = fbuffer(FBUF_GET);

        /* konecny automat lexikalneho analyzatora */
        switch (state)
        {
            case ST_INIT:
                /* zahadzovanie bielych znakov */
                if (isspace(c) || (c == '\n'))
                {
                    state = ST_INIT;
                }
                /* TKN_NUMBER */
                if (isdigit(c))
                {
                    token->id = TKN_NUMBER;
                    if (! str_init(&tmp)) exit(RET_ERR_MY);
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_INT;
                }
                /* TKN_ID */
                else if (isalpha(c))
                {
                    if (! str_init(&tmp)) exit(RET_ERR_MY);
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_ID;
                }
                /* TKN_STRING */
                else if (c == '"')
                {
                    if (! str_init(&tmp)) exit(RET_ERR_MY);
                    token->id = TKN_STRING;
                    state = ST_STRING;
                }
                /* TKN_OP_CONC */
                else if (c == '.')
                {
                    token->id = TKN_OP_CONC;
                    state = ST_CONC;
                }
                /* TKN_OP_NOT_EQUAL */
                else if (c == '~')
                {
                    token->id = TKN_OP_NOT_EQUAL;
                    state = ST_NOT_EQUAL;
                }
                /* TKN_OP_ASSIGNMENT || TKN_OP_EQUAL */
                else if (c == '=')
                {
                    state = ST_ASSIGN_OR_EQUAL;
                }
                /* TKN_OP_LESS || TKN_OP_LESS_EQUAL */
                else if (c == '<')
                {
                    state = ST_LESS;
                }
                /* TKN_OP_GREATER || TKN_OP_GREATER_EQUAL */
                else if (c == '>')
                {
                    state = ST_GREATER;
                }
                /* TKN_OP_ADD */
                else if (c == '+')
                {
                    token->id = TKN_OP_ADD;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                /* TKN_OP_SUB */
                else if (c == '-')
                {
                    state = ST_SUB;
                }
                /* TKN_OP_MUL */
                else if (c == '*')
                {
                    token->id = TKN_OP_MUL;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                /* TKN_OP_DIV */
                else if (c == '/')
                {
                    token->id = TKN_OP_DIV;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                /* TKN_OP_POWER */
                else if (c == '^')
                {
                    token->id = TKN_OP_POWER;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                /* TKN_BRACKET_L */
                else if (c == '(')
                {
                    token->id = TKN_BRACKET_L;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                /* TKN_BRACKET_R */
                else if (c == ')')
                {
                    token->id = TKN_BRACKET_R;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                /* TKN_COMMA */
                else if (c == ',')
                {
                    token->id = TKN_COMMA;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                /* TKN_SEMICOL */
                else if (c == ';')
                {
                    token->id = TKN_SEMICOL;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                /* TKN_OP_MOD */
                else if (c == '%')
                {
                    token->id = TKN_OP_MOD;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                /* TKN_OP_STRLEN */
                else if (c == '#')
                {
                    token->id = TKN_OP_STRLEN;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }

                /* TKN_EOF */
                else if (c == EOF)
                {
                    token->id = TKN_EOF;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }

                else
                {
                    /* odchytenie "zabludenych" znakov (neplatnych lexem) */
                    if (!isspace(c))
                    {
                        xerror_msg(ERR_LEX_STRAY_CHR);
                        return set_error(RET_ERR_LEX);
                    }
                }
            break;

            /* TKN_NUMBER */
            case ST_NUMBER_INT:
                if (isdigit(c))
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_INT;
                }
                else if ((c == 'e') || (c == 'E'))
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_EXP;
                }
                else if (c == '.')
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_REAL;
                }
                else
                {
                    sscanf(tmp.str, "%lf", &token->val.num);
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
            break;

            case ST_NUMBER_REAL:
                if (isdigit(c))
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_REAL_L;
                }
                else
                {
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_MALFORMED_NUM);
                    return set_error(RET_ERR_LEX);
                }
            break;

            case ST_NUMBER_REAL_L:
                if (isdigit(c))
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_REAL_L;
                }
                else if ((c == 'e') || (c == 'E'))
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_EXP;
                }
                else
                {
                    sscanf(tmp.str, "%lf", &token->val.num);
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
            break;

            case ST_NUMBER_EXP:
                if (isdigit(c))
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_EXP_L;
                }
                else if ((c == '+') || (c == '-'))
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_SIGN_EXP;
                }
                else
                {
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_MALFORMED_NUM);
                    return set_error(RET_ERR_LEX);
                }
            break;

            case ST_NUMBER_EXP_L:
                if (isdigit(c))
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_EXP_L;
                }
                else
                {
                    sscanf(tmp.str, "%lf", &token->val.num);
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
            break;

            case ST_NUMBER_SIGN_EXP:
                if (isdigit(c))
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_NUMBER_EXP_L;
                }
                else
                {
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_MALFORMED_NUM);
                    return set_error(RET_ERR_LEX);
                }
            break;

            /* TKN_ID */
            case ST_ID:
                if (isalnum(c) || (c == '_'))
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_ID;
                }
                else
                {
                    /* klucove a rezervovane slova */
                    scanner_special_words(token, &tmp);
                    fbuffer(FBUF_UNGET);
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
            break;

            /* TKN_STRING */
            case ST_STRING:
                if (c == '"')
                {
                    token->val.str = str_get_str(&tmp);
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                else if (c == '\\')
                {
                    state = ST_STRING_ESC;
                }
                /* osetrenie chyby pri EOF */
                else if (c == EOF)
                {
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_UNTERMINATED_STR);
                    str_free(&tmp);
                    return set_error(RET_ERR_LEX);
                }
                else
                {
                    if (! str_add_char(&tmp, c)) exit(RET_ERR_MY);
                    state = ST_STRING;
                }
            break;

            case ST_STRING_ESC:
                if (c == 'n')
                {
                    if (! str_add_char(&tmp, '\n')) exit(RET_ERR_MY);
                    state = ST_STRING;
                }
                else if (c == 't')
                {
                    if (! str_add_char(&tmp, '\t')) exit(RET_ERR_MY);
                    state = ST_STRING;
                }
                else if (c == '\\')
                {
                    if (! str_add_char(&tmp, '\\')) exit(RET_ERR_MY);
                    state = ST_STRING;
                }
                else if (c == '"')
                {
                    if (! str_add_char(&tmp, '"')) exit(RET_ERR_MY);
                    state = ST_STRING;
                }
                else if ((c == '0') || (c == '1'))
                {
                    esc_seq = c - '0';
                    state = ST_STRING_ESC_NUM_1L;
                }
                else if (c == '2')
                {
                    esc_seq = c - '0';
                    state = ST_STRING_ESC_NUM_1G;
                }
                else
                {
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_BAD_ESC_SEQ);
                    return set_error(RET_ERR_LEX);
                }
            break;

            /**
             * NOTE: Kompilator spracovava escape sekvencie
             * v \ddd formate, pricom konecny automat osetruje
             * velkost cisla ddd.
             */

            /* prva cifra je <0;1> */
            case ST_STRING_ESC_NUM_1L:
                if (isdigit(c)) /* na druhej cifre nezalezi */
                {
                    esc_seq = esc_seq * 10 + (c - '0');
                    state = ST_STRING_ESC_NUM_2L;
                }
                else
                {
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_BAD_ESC_SEQ);
                    return set_error(RET_ERR_LEX);
                }
            break;

            /* prva cifra je {2} */
            case ST_STRING_ESC_NUM_1G:
                if ((c >= '0') && (c <= '4')) /* druha cifra je <0;4> */
                {
                    esc_seq = esc_seq * 10 + (c - '0');
                    state = ST_STRING_ESC_NUM_2L;
                }
                else if (c == '5') /* druha cifra je {5} */
                {
                    esc_seq = esc_seq * 10 + (c - '0');
                    state = ST_STRING_ESC_NUM_2G;
                }
                else
                {
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    return set_error(RET_ERR_LEX);
                }
            break;

            /* prva cifra je <0;1> a druha cifra je <0;9> */
            /* alebo */
            /* prva cifra je {2} a druha cifra je <0;4> */
            case ST_STRING_ESC_NUM_2L:
                if (isdigit(c)) /* na tretej cifre nezalezi */
                {
                    esc_seq = esc_seq * 10 + (c - '0');
                    if (! str_add_char(&tmp, esc_seq)) exit(RET_ERR_MY);
                    state = ST_STRING;
                }
                else
                {
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_BAD_ESC_SEQ);
                    return set_error(RET_ERR_LEX);
                }
            break;

            /* prva cifra je {2} a druha cifra je {5} */
            case ST_STRING_ESC_NUM_2G:
                if ((c >= '0') && (c <= '5')) /* tretia cifra je <0;5> */
                {
                    esc_seq = esc_seq * 10 + (c - '0');
                    if (! str_add_char(&tmp, esc_seq)) exit(RET_ERR_MY);
                    state = ST_STRING;
                }
                else
                {
                    str_free(&tmp);
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_BAD_ESC_SEQ);
                    return set_error(RET_ERR_LEX);
                }
            break;


            /* TKN_OP_CONC */
            case ST_CONC:
                if (c == '.')
                {
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                else
                {
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_BAD_OPERATOR);
                    return set_error(RET_ERR_LEX);
                }
            break;

            /* TKN_OP_NOT_EQUAL  */
            case ST_NOT_EQUAL:
                if (c == '=')
                {
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                else
                {
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_BAD_OPERATOR);
                    return set_error(RET_ERR_LEX);
                }
            break;

            /* TKN_OP_ASSIGNMENT || TKN_OP_EQUAL */
            case ST_ASSIGN_OR_EQUAL:
                if (c == '=')
                {
                    token->id = TKN_OP_EQUAL;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                else
                {
                    token->id = TKN_OP_ASSIGNMENT;
                    fbuffer(FBUF_UNGET);
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
            break;

            /* TKN_OP_GREATER || TKN_OP_GREATER_EQUAL */
            case ST_GREATER:
                if (c == '=')
                {
                    token->id = TKN_OP_GREATER_EQUAL;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                else
                {
                    token->id = TKN_OP_GREATER;
                    fbuffer(FBUF_UNGET);
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
            break;

            /* TKN_OP_LESS || TKN_OP_LESS_EQUAL */
            case ST_LESS:
                if (c == '=')
                {
                    token->id = TKN_OP_LESS_EQUAL;
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
                else
                {
                    token->id = TKN_OP_LESS;
                    fbuffer(FBUF_UNGET);
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
            break;

            /* TKN_OP_SUB */
            case ST_SUB:
                if (c == '-')
                {
                    state = ST_COMMENT_S;
                }
                else
                {
                    token->id = TKN_OP_SUB;
                    fbuffer(FBUF_UNGET);
                    DEBUG_PRINT_TOKEN(token);
                    return true;
                }
            break;

            /* komentar */
            case ST_COMMENT_S:
                if (c == '\n')
                {
                    state = ST_INIT;
                }
                else if (c == '[')
                {
                    state = ST_COMMENT_BLOCK_S;
                }
                /* osetrenie zacyklenia pri EOF */
                else if (c == EOF)
                {
                    state = ST_INIT;
                    c = fbuffer(FBUF_UNGET);
                }
                else
                {
                    state = ST_COMMENT;
                }
            break;

            case ST_COMMENT:
                if (c == '\n')
                {
                    state = ST_INIT;
                }
                /* osetrenie zacyklenia pri EOF */
                else if (c == EOF)
                {
                    state = ST_INIT;
                    c = fbuffer(FBUF_UNGET);
                }
            break;

            case ST_COMMENT_BLOCK_S:
                if (c == '[')
                {
                    state = ST_COMMENT_BLOCK;
                }
                else
                {
                    state = ST_COMMENT;
                }
            break;

            case ST_COMMENT_BLOCK:
                if (c == ']')
                {
                    state = ST_COMMENT_BLOCK_E;
                }
                /* osetrenie chyby pri EOF - neukonceny komentar */
                else if (c == EOF)
                {
                    fbuffer(FBUF_UNGET);
                    xerror_msg(ERR_LEX_UNTERMINATED_COM);
                    return set_error(RET_ERR_LEX);
                }
            break;

            case ST_COMMENT_BLOCK_E:
                if (c == ']')
                {
                    state = ST_INIT;
                }
                else
                {
                    state = ST_COMMENT_BLOCK;
                }
            break;

            default:
            break;
        }
    }
}

/**
 * Funkcia prejde tabulku klucovych, rezervovanych slov, nazov vst. funkcii
 * a ak je identifikator klucove/rezervovane slovo/nazov vst. funkcie
 * nastavi prislusny individualny token.
 * @param - argument ukazatel na token
 * @param - argument identifikator
 * @return - void
 */
void scanner_special_words(Ttoken *token, Tstring *id)
{
    for (int i = TKN_KW_DO; i <= TKN_BI_SORT; i++)
    {
        if (strcmp(SPECIFIC_WORDS[i - TKN_KW_DO], str_get_str(id)) == 0)
        { /* polozka sa nasla - nastavujem typ tokenu */
            token->id = i;
            str_free(id);
            return;
        }
    }

    /* polozka sa nenasla - nastavujem TKN_ID a nazov id */
    token->id = TKN_ID;
    token->val.str = str_get_str(id);
    return;
}

