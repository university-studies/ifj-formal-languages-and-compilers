/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       pstack.h
 * Autor(i):    Pavol Loffay
 *
 * Implementacia zasobnika pre precedencnu analyzu.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_PSTACK_H_
#define IFJ11_PSTACK_H_

#include <stdbool.h>

#include "scanner.h"
#include "variable.h"
#include "ial.h"

#define PSTACK_SIZE  200

/* neinicializovana hodnota na  pstack_item */
#define UNINITIALIZED -1
/* hodnota udava ze na zasobniku nie je lava zatvorka (*/
#define NO_BRACKET -2

/**
 * typy poloziek stacku
 */
enum pstack_item_type
{
    START,          // '$'
    REDUCE,         //vsetko sa ma zredukovat

    EXPR,           // cislo, identifikator, string, nil, boolean, zjednoduseny vyraz,

    ADD,            //  +
    SUB,            //  -
    MUL,            //  *
    DIV,            //  /
    MODULO,         //  %
    CONCAT,         //  ..
    STRLEN,         //  #
    POWER,          //  ^

    AND,            //  and
    OR,             //  or
    NOT,            //  not
    EQUAL,          //  =
    NOT_EQUAL,      //  ~=
    GREATER,        //  >
    LESS,           //  <
    GREATER_EQUAL,  //  >=
    LESS_EQUAL,     //  <=

    BRACKET_L,      //  (
    BRACKET_R,      //  )
};

/**
 * struktura polozky stacku
 */
typedef struct pstack_item
{
    //znaci typ nonterminal,token
    enum pstack_item_type type;
    /*
     * ak je polozna expr a je literal
     * napriklad (TOKEN_NUMBER, TOKEN_STRING, TOKEN_FALSE/TRUE)
     * tak bude true
     */
    bool literal;
    // ukazatel na premennu kde je ulozeny vysledok, ukazuje do htable
    Tvariable * variable;
} Tpstack_item;

/**
 * struktura stacku
 */
typedef struct pstack
{
    //velkost zasobniku
    int size;
    //vrchol zasobniku, vzdy ukazuje na poslednu polozku
    int top;

    //pole poloziek zasobniku
    Tpstack_item pstack_item[PSTACK_SIZE];
} Tpstack;


void pstack_init(void);

void pstack_push_start(void);
bool pstack_push(enum pstack_item_type type, bool literal, Tvariable * variable);
bool pstack_push_expr(Tvariable * variable, Tpstack_item item_a,
                      Tpstack_item item_b);

Tpstack_item pstack_pop_item(void);
enum pstack_item_type pstack_pop_type(void);
int pstack_type_from_index(int index);

int pstack_top_type(void);
int pstack_top_type_operation(void);
int pstack_top_bracket_left_index(void);
int pstack_top_index(void);

void pstack_delete(void);

#endif /* IFJ11_PSTACK_H_ */
