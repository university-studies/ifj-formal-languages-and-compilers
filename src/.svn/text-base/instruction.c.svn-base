/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       instruction.c
 * Autor(i):    Fridolin Pokorny
 *
 * Implementacia zoznamu trojadresnych instrukcii.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stddef.h>
#include <assert.h>

#include "instruction.h"
#include "stack.h"
#include "xwrappers.h"

#define INST_LIST_EMPTY(L)     (! L->first)
#define INST_LIST_ACTIVE(L)    (L->active)
/**
 * Inicializuje hlavicku linearneho zoznamu instrukcii.
 * @param - zoznam, ktory sa ma inicializovat
 * @return - none
 */
void inst_list_init(Tinstruction_list * list)
{
    assert(list);

    list->first = list->last = list->active = NULL;
}

/**
 * Preda aktivitu na prvy prvok zoznamu.
 * @param - zoznam instrukcii
 * @return - prva instrukcia
 */
Tinstruction * inst_list_first(Tinstruction_list * list)
{
    assert(list);

    list->active = list->first;

    return list->active;
}

/**
 * Preda aktivitu na dalsi prvok, ak je zoznam aktivny.
 * @param - zoznam instrukcii
 * @return - aktivna instrukcia
 */
Tinstruction * inst_list_succ(Tinstruction_list * list)
{
    assert(list);
    Tinstruction * rv = list->active;

    if (list->active)
        list->active = list->active->next;

    return rv;
}

/**
 * Vytvori instrukciu daneho typu a operandmi.
 * @param - typ instrukcie
 * @param - src1 instrukcie
 * @param - src2 instrukcie
 * @param - dst instrukcie
 * @return - vytvorena instrukcia
 */
Tinstruction * inst_create(enum inst_type opcode,
                           union inst_operand dst,
                           union inst_operand src1,
                           union inst_operand src2)
{
    Tinstruction * new = xmalloc(sizeof(Tinstruction));

    new->opcode = opcode;
    new->src1 = src1;
    new->src2 = src2;
    new->dst = dst;

    new->next = new->prev = NULL;

    return new;
}

/**
 * Funkcia vytvori prazdnu instrukciu.
 * @param - void
 * @return - vytvorena instrukcia
 */
Tinstruction * inst_alloc(void)
{
    Tinstruction * inst = xmalloc(sizeof(Tinstruction));
    union inst_operand op = {.var = NULL};

    inst->dst = inst->src1 = inst->src2 = op;

    return inst;
}

/**
 * Funkcia naplni predom vytvorenu instrukciu danymi hodnotami.
 * @param - instrukcia, ktora bude inicializovana
 * @param - typ instrukcie
 * @param - cielovy operand
 * @param - zdrojovy operand 1
 * @param - zdrojovy operand 2
 */
void inst_fill(Tinstruction * inst,
               enum inst_type opcode,
               union inst_operand dst,
               union inst_operand src1,
               union inst_operand src2)
{
    inst->opcode = opcode;
    inst->src1 = src1;
    inst->src2 = src2;
    inst->dst = dst;

    inst->next = inst->prev = NULL;
}

/**
 * Prida instrukciu inst na koniec zoznamu `to'.
 * @param - zoznam, kam sa ma instrukcia pridat
 * @param - pridavana instrukcia
 * @return - none
 */
void inst_append(Tinstruction_list * to, Tinstruction * inst)
{
    assert(to);
    assert(inst);

    inst->next = NULL;
    inst->prev = to->last;

    if (INST_LIST_EMPTY(to))
    {
        to->first = inst;
        to->last = inst;
    }
    else
    {
        to->last->next = inst;
        to->last = inst;
    }
}

/**
 * Nastavi aktivitu na posledny prvok v zozname.
 * @param - zoznam instrukcii, kde sa ma aktivita nastavit
 * @return - none
 */
void inst_last(Tinstruction_list * list)
{
    list->active = list->last;

}

/**
 * Vlozi instrukciu za aktivny prvok.
 * @param - zoznam instrukcii, do ktorej sa ma instrukcia vlozit
 * @param - instrukcia, ktora sa ma vlozit
 * @return - none
 */
void inst_list_postinsert(Tinstruction_list * list, Tinstruction * inst)
{
    if (INST_LIST_ACTIVE(list))
    {
        inst->prev = list->active;
        inst->next = list->active->next;

        list->active->next = inst;
        if (inst->next)
            inst->next->prev = inst;
    }
}

/**
 * Zmaze zoznam instrukcii.
 * @param - zoznam instrukcii, ktore budu zmazane
 * @return - void
 */
void inst_list_destroy(Tinstruction_list * list)
{
    assert(list);
    while (! INST_LIST_EMPTY(list))
        inst_delete(list, list->last);
}

/**
 * Odstrani zadanu instrukciu zo zoznamu.
 * @param - zoznam obsahujuci danu instrukciu
 * @param - instrukcia, ktora ma byt zmazana
 * @return - none
 */
void inst_delete(Tinstruction_list * list, Tinstruction * inst)
{
    assert(list);
    assert(inst);

    if (inst == list->last && inst == list->first)
    {
        list->first = list->last = NULL;
    }
    else if (inst == list->last)
    {
        inst->prev->next = NULL;
        list->last = inst->prev;
    }
    else if (inst == list->first)
    {
        list->first = inst->prev;
        list->first->next  = NULL;

    }
    else
    {
        inst->next->prev = inst->prev;
        inst->prev->next = inst->next;
    }

    xfree(inst);
}

