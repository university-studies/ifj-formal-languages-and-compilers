/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       variable.c
 * Autor(i):    Fridolin Pokorny
 *
 * Implementacia zoznamu premennych a zakladnymi operaciami nad nimi.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "variable.h"
#include "xwrappers.h"

#define VARIABLE_LIST_EMPTY(L)      (! L->first)

/**
 * Vytvori premennu daneho typu, mena a priradi jej hodnotu.
 * @param - nazov premennej
 * @param - typ premennej
 * @param - hodnota premennej
 * @return - alokovana premenna
 */
Tvariable *
variable_create(char * name, enum var_type type, union variable_val val)
{
    Tvariable * ret = xmalloc(sizeof(Tvariable));

    ret->type = type;
    ret->val = val;
    ret->name = name;

    ret->next = NULL;

    return ret;
}

/**
 * Dealokuje pamat vyhradenu pre premennu.
 * @param - premenna k dealokacii
 * @return - void
 */
void variable_delete(Tvariable_list * list, Tvariable * var)
{
    assert(list);
    assert(var);

    if (list->first == var && list->last == var)
    {
        list->first = list->last = NULL;
    }
    else if (list->first == var)
    {
        list->first = var->next;
        list->first->prev = NULL;
    }
    else if (list->last == var)
    {
        list->last = var->prev;
        list->last->next = NULL;
    }
    else
    {
        var->next->prev = var->prev;
        var->prev->next = var->next;
    }

    if (var->type == VAR_STR && ! var->name)
        xfree(var->val.str);

    xfree(var->name);
    xfree(var);
}

/**
 * Inicializacia zoznamu premennych
 * @param - hlavicka zoznamu
 * @return - void
 */
void variable_list_init(Tvariable_list * list)
{
    assert(list);

    list->first = list->last = list->active = NULL;
}

/**
 * Funkcia vytvori hlboku kopiu premennej.
 * @param - cielova premenna
 * @param - zdrojova premenna
 */
void variable_copy(Tvariable * dst, const Tvariable * src)
{
    dst->type = src->type;

    if (dst->type == VAR_STR)
    {
        dst->val.str = xmalloc(strlen(src->val.str) + 1);
        strcpy(dst->val.str, src->val.str);

    }
    else
        dst->val = src->val;
}

/**
 * Vytvori kopiu zoznamu premennych. Pamat pre nazov premenne zdielaju!
 * @param - miesto, kam sa ma kopia ulozit
 * @param - zoznam z ktoreho kopia sa ma urobit
 * @return - void
 */
void variable_list_copy(Tvariable_list * dst, const Tvariable_list * src)
{
    assert(dst);
    assert(src);

    Tvariable * dst_ptr;
    Tvariable * src_ptr = src->first;

    if (src_ptr)
    {
        dst->first = variable_create(src_ptr->name, src_ptr->type, src_ptr->val);
        dst_ptr = dst->first;
        src_ptr = src_ptr->next;

        while (src_ptr)
        {
            dst_ptr->next = variable_create(src_ptr->name,
                                            src_ptr->type,
                                            src_ptr->val);
            if (dst_ptr->prev)
                dst_ptr->prev->next = dst_ptr;
            dst_ptr = dst_ptr->next;
            src_ptr = src_ptr->next;
        }

        dst->last = dst_ptr;
    }

}

/**
 * Priradi hodnoty a typy zoznamu premennych. Funkcia z dovodu rychlosti
 * nekontroluje, ci premenne maju rovnaky nazov a ci su zoznamy rovnako dlhe.
 * Toto si musi zaistit programator.
 * @param - zoznam, kam sa maju hodnoty priradit
 * @param - zdrojovy zoznam, z ktoreho sa hodnoty priraduju
 * @return - void
 */
void variable_list_assign_del(Tvariable_list * dst, const Tvariable_list * src)
{
    assert(dst);
    assert(src);

    Tvariable * dst_ptr = dst->first;
    Tvariable * src_ptr = src->first;

    while (src_ptr)
    {
        /*
         * Zoznamy mysia byt rovnako dlhe!
         */
        assert(dst_ptr);

        dst_ptr->type = src_ptr->type;
        dst_ptr->val = src_ptr->val;
        /* nazov maju rovnaky */

        dst_ptr = dst_ptr->next;
        src_ptr = src_ptr->next;

        if (src_ptr)
            xfree(src_ptr->prev);
    }
}

/**
 * Odstrani vsetky polozky zretezeneho zoznamu premennych.
 * @param - zoznam premennych k odstraneniu
 * @return - void
 */
void variable_list_destroy(Tvariable_list * list)
{
    assert(list);

    while (! VARIABLE_LIST_EMPTY(list))
        variable_delete(list, list->first);


}

/**
 * Prida premennu na koniec zoznamu premennych.
 * @param - list premennych
 * @param - pridavana premenna
 * @return void
 */
void variable_list_append(Tvariable_list * list, Tvariable * var)
{
    assert(list);
    assert(var);

    if (VARIABLE_LIST_EMPTY(list))
    {
        list->first = list->last = var;
        var->prev = var->next = NULL;
    }
    else
    {
        var->next = NULL;
        var->prev = list->last;
        list->last->next = var;
        list->last = var;
    }
}

/**
 * Nastavi aktivitu na prvy prvok zoznamu a vrati na neho ukazatel.
 * @param - ukazatel na zoznam premennych
 * @return - ukazatel na prvu premennu
 */
Tvariable * variable_list_first(Tvariable_list * list)
{
    assert(list);

    return list->active = list->first;
}

/**
 * Posunie aktivitu na nasledujucu premennu a vrati ukazatel na premennu ktora
 * bola aktivna pred volanim funkcie.
 * @param - ukazatel na zoznam premennych
 * @return - aktivna premenna
 */
Tvariable * variable_list_succ(Tvariable_list * list)
{
    assert(list);

    Tvariable * rv = list->active;

    if (list->active)
        list->active = list->active->next;

    return rv;
}
/**
 * Vyhlada v zozname premennych premennu so zadanym menom.
 * @param - ukazatel na list premennych
 * @param - nazov premennej, ktora sa bude vyhladavat
 * @return - najdena premenna alebo NULL
 */
Tvariable * variable_list_look(const Tvariable_list * list, const char * name)
{
    assert(list);
    assert(name);

    for (Tvariable * tmp = list->first; tmp != NULL; tmp = tmp->next)
    {
        if (tmp->name && strcmp(tmp->name, name) == 0)
            return tmp;
    }
    return NULL;
}
