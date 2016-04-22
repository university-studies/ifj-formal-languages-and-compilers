/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       pstack.c
 * Autor(i):    Pavol Loffay
 *
 * Implementacia zasobnika pre precedencnu analyzu
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "pstack.h"
#include "xwrappers.h"
#include "ifj11.h"

static Tpstack pstack;

/**
 * funkcia inicializuje polozky buffera
 * @param - void
 * @return - void
 */
void pstack_init(void)
{
    pstack.size = PSTACK_SIZE;
    pstack.top = 0;

    for (int i = 0; i < pstack.size; i++)
    {
        pstack.pstack_item[i].type = UNINITIALIZED;
        pstack.pstack_item[i].literal = false;
        pstack.pstack_item[i].variable = NULL;
    }
}

/**
 * funkcia vlozi na zasobnik novu polozku
 * @param - urucuje typ polozky napr. SHIFT,EQUAL, EXPRESSION
 * @param - ukazatel na premennu ulozenu htable/stack
 * @return - ak doslo k preteceniu false, inak true
 */
bool pstack_push(enum pstack_item_type type, bool literal, Tvariable * variable)
{
    if (pstack.top + 1 >= pstack.size)
    {
        set_error(RET_ERR_MY);
        xerror_msg(ERR_MY_PSTACK_OVERF);
        return false;
    }

    pstack.top++;

    pstack.pstack_item[pstack.top].type = type;
    pstack.pstack_item[pstack.top].literal = literal;
    pstack.pstack_item[pstack.top].variable = variable;

    return true;
}

/**
 * na vrchol zasobnika vlozi symbol start,
 * @param - void
 * @return - void
 */
void pstack_push_start(void)
{
    pstack.pstack_item[pstack.top].type = START;
    pstack.pstack_item[pstack.top].variable = NULL;
}

/**
 * ulozi novu polozky EXPR na pstack, zisti ci je literal
 * @param - variable
 * @param - poloka pstack
 * @param - polozka pstack
 * @return - true ak sa ulozila v poriadku, false ak doslo k PSTACK_OVERFLOW
 */
bool pstack_push_expr(Tvariable * new_var, Tpstack_item item_a, Tpstack_item item_b)
{
    bool literal = false;

    if (item_a.literal == true && item_b.literal == true)
        literal = true;

    if (pstack_push(EXPR, literal, new_var) == false)
        return false;

    return true;
}

/**
 * vrati polozku na vrchole zasobnika a nasledne polozku odstrani
 * @param - void
 * @return - polozku Tpstack_item z vrchola zasobnika
 */
Tpstack_item pstack_pop_item(void)
{
    Tpstack_item ret = pstack.pstack_item[pstack.top];

    pstack.pstack_item[pstack.top].type = UNINITIALIZED;
    pstack.pstack_item[pstack.top].literal = false;
    pstack.pstack_item[pstack.top].variable = NULL;

    pstack.top--;

    return ret;
}

/**
 * vrati typ polozky z vrchola zasobnika a popne
 * @param - void
 * @return - typ polozky z vrchola zasobnika
 */
enum pstack_item_type pstack_pop_type(void)
{
    int ret = pstack.pstack_item[pstack.top].type;

    pstack.pstack_item[pstack.top].type = UNINITIALIZED;
    pstack.pstack_item[pstack.top].literal = false;
    pstack.pstack_item[pstack.top].variable = NULL;

    pstack.top--;

    return ret;
}

/**
 * funkcia vrati aktualny typ polozky na vrchole zasobnika
 * @param -void
 * @return - typ polozky napr. SHIFT,EQUAL, EXPRESSION
 */
int pstack_top_type(void)
{
    return pstack.pstack_item[pstack.top].type;
}

/**
 * vrati typ polozky ktory, je najblizsie vrcholu zasobnika a nie je to
 * EXPR,
 * @param - void
 * @return - typ polozky, ak tam nie je ziadna vrati START
 */
int pstack_top_type_operation(void)
{
    for (int temp_top = pstack.top; temp_top >= 0; temp_top--)
    {
        if (pstack.pstack_item[temp_top].type != EXPR)
            return pstack.pstack_item[temp_top].type;
    }

    return START;
}

/**
 * najde prvy vyskit BRACKET_L, na zasobniku
 * @param - void
 * @return - index prvej najdenej BRACKET_L na zasobniku,
 *          ak sa nenajde vrati NO_BRACKET,
 */
int pstack_top_bracket_left_index(void)
{
    for (int temp_top = pstack.top; temp_top >= 0; temp_top--)
    {
        if (pstack.pstack_item[temp_top].type == BRACKET_L)
            return temp_top;
    }

    return NO_BRACKET;
}

/**
 * vrati index vrchola zasobnika
 * @param - void
 * @return - aktualny idex vrchola zasobnika
 */
int pstack_top_index(void)
{
    return pstack.top;
}

/**
 * vrati typ polozky staku na danom indexe
 * @param - index z ktoreho potom vrati typ polozky
 * @return void
 */
int pstack_type_from_index(int index)
{
    return pstack.pstack_item[index].type;
}

