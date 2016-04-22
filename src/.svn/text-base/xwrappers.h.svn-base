/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       xwrappers.h
 * Autor(i):    Fridolin Pokorny
 *
 * Implementacia wrapperov nad kniznicnymi funkciami z libc, ktore osetruju
 * chybove stavy.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_XWRAPPERS_H_
#define IFJ11_XWRAPPERS_H_

#include <string.h>

#include "scanner.h"

enum xerror_error
{
    /*
     * Zoznam lexikalnych chyb.
     */
    ERR_LEX_STRAY_CHR,
    ERR_LEX_MALFORMED_NUM,
    ERR_LEX_BAD_ESC_SEQ,
    ERR_LEX_BAD_OPERATOR,
    ERR_LEX_UNTERMINATED_COM,
    ERR_LEX_UNTERMINATED_STR,
    /*
     * Zoznam syntaktickych chyb.
     */
    ERR_SYN_UNEXP_KEY_EXP,
    ERR_SYN_EXPR,
    ERR_SYN_NO_L_BRACKET,
    ERR_SYN_NO_MAIN,
    ERR_SYN_EXP_EOF,
    ERR_SYN_EXP_FUN,
    ERR_SYN_EXP_ID,
    ERR_SYN_MAIN_LAST,
    ERR_SYN_EXP_L_BRACKET,
    ERR_SYN_EXP_R_BRACKET,
    ERR_SYN_UNEXP_TKN,
    ERR_SYN_EXP_SEMICOLON,
    ERR_SYN_EXP_LITERAL,
    ERR_SYN_EXP_ASSIGNMENT,
    ERR_SYN_EXP_DO,
    ERR_SYN_EXP_THEN,
    ERR_SYN_EXP_ID_VAR,
    ERR_SYN_EXP_COMMA,
    /*
     * Zoznam semantickych chyb.
     */
    ERR_SEM_BAD_ARITH_LIT,
    ERR_SEM_BAD_STRING_LIT,
    ERR_SEM_BAD_CMP_LIT,
    ERR_SEM_UNDEFINED_ID_EXPR,
    ERR_SEM_FUNC_IN_EXPR,
    ERR_SEM_FUN_REDECLARATION,
    ERR_SEM_ID_COLLISION,
    ERR_SEM_ID_UNDEFINED,
    ERR_SEM_MAIN_PARAM,
    ERR_SEM_READ_PARAM,
    ERR_SEM_WRITE_PARAM,

    /*
     * Zoznam chyb interpretru.
     */
    ERR_INT_STRLEN_OP,
    ERR_INT_NUM_RANGE,
    ERR_INT_BAD_CONC,
    ERR_INT_WRITE_OP,
    ERR_INT_SUBSTR_OP,
    ERR_INT_BAD_CMP,
    ERR_INT_DIFF_CMP,
    ERR_INT_BAD_ARITH_OP,
    ERR_INT_READ_OP,
    ERR_INT_READ_NUM,
    /*
     * Zoznam internych chyb.
     */
    ERR_MY_PSTACK_OVERF,
    ERR_MY_PSTACK_BAD_DATA,
    ERR_MY_TOKEN,
    ERR_MY_ALLOC,
};

enum xwarning_warning
{
    WAR_IGNORE_RESULT,
};

void xerror(const char *fmt, ...);
void xerror_msg(enum xerror_error err);
void xwarning_msg(enum xwarning_warning war);
void * xmalloc(size_t size);
void * xcalloc(size_t size, size_t size_type);
void xfree(void * ptr);

#endif /* IFJ11_XWRAPPERS_H_ */

