/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       stack.c
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

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string.h>

#include "stack.h"
#include "xwrappers.h"
#include "instruction.h"
#include "variable.h"
#include "ial.h"

static struct {
    Tstack_item * top;
} stack;

/**
 * Inicializuje stack pred prvym pouzitim.
 * @param - none
 * @return - none
 */
void stack_init(void)
{
    stack.top = NULL;
}

/**
 * Odstrani z vrcholu zasobnika polozku a vrati instrukciu, ktorou sa bude
 * pokracovat po navrate z podprogramu.
 * @param - mone
 * @return - prva vykonavana instrukcia po navrate z podprogramu
 */
Tinstruction * stack_pop(void)
{
    Tstack_item * tmp;
    Tinstruction * ret = NULL;

    if (stack.top)
    {
        tmp = stack.top;
        ret = stack.top->ret_inst;

        /*
         * Obnovenie zalohy premennych.
         */
        variable_list_assign_del(&stack.top->var_orig, &stack.top->var);
        stack.top = stack.top->prev;

        xfree(tmp);

        if (stack.top)
            stack.top->next = NULL;
    }

    return ret;
}

/**
 * Na vrchol zasobnika ulozi informacie spojene s volanim podprogramu -
 * instrukcia po navrate, zalohu premennych a odkaz na interpretacne premenne.
 * @param - polozka z hashovacej tabulky
 * @return - none
 */
void stack_push(const Thtable_item * item)
{
    assert(item);

    Tstack_item * new = xmalloc(sizeof(Tstack_item));
    new->prev = stack.top;
    new->next = NULL;
    stack.top = new;

    /*
     * Poznamenanie ukazatelov na premenne odkazovane z tabulky symbolov.
     */
    new->var_orig = item->var;
    /*
     * Vytvorenie kopie premennych
     */
    variable_list_copy(&new->var, &item->var);
    /*
     * Ulozenie navratovej adresy sa prevedie volanim operacie stack_set_rv().
     */
    new->ret_inst = NULL;

}

/**
 * Nastavi navratovu hodnotu zaznamu aktivacnemu zaznamu na vrchole zasobnika.
 * @param - prva instrukcia, ktora sa bude vykonavat po navrate z podprogramu
 * @return - void
 */
void stack_set_rv(Tinstruction * inst)
{
    if (stack.top)
        stack.top->ret_inst = inst;
}

/**
 * Vymaze vsetky polozky nad zasobnikom a vrati zasobnik do stavu po
 * inicializacii.
 * @param - none
 * @return - none
 */
void stack_destroy(void)
{
    Tstack_item * tmp = stack.top;

    while (tmp)
    {
        variable_list_destroy(&tmp->var);
        stack.top = tmp->prev;
        xfree(tmp);
        tmp = stack.top;
    }

    stack.top = NULL;
}

