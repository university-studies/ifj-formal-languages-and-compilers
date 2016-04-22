/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       stack.h
 * Autor(i):    Fridolin Pokorny
 *
 * Implementacia programoveho stacku - uchovavanie informacii pri volani
 * podprogramu.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#ifndef IFJ11_STACK_H_
#define IFJ11_STACK_H_

#include <stdbool.h>
#include "variable.h"

struct Thtable_item; /* forward */
struct Tinstruction;

/*
 * Polozka uchovavajuca informacie o volanej funkcii.
 */
typedef struct Tstack_item
{
    /*
     * Stack je interne obojstranne viazany linearny zoznam.
     */
    struct Tstack_item * next;
    struct Tstack_item * prev;

    /*
     * Zalohovane premenne funkcie.
     */
    struct Tvariable_list var;
    /*
     * Ukazatel na povodne premenne funkcie (potrebne pri obnove zalohy)
     */
    struct Tvariable_list var_orig;
    /*
     * Odkaz na prvu instrukciu, ktora sa bude vykonavat po navrate z
     * podprogramu.
     */
    struct Tinstruction * ret_inst;
} Tstack_item;


void stack_init(void);
struct Tinstruction * stack_pop(void);
void stack_push(const struct Thtable_item * func);
void stack_set_rv(struct Tinstruction * inst);
void stack_destroy(void);

#endif /* IFJ11_STACK_H_ */
