/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       pparser.c
 * Autor(i):    Pavol Loffay
 *
 * Implementacia precedencnej analyzy na spracovavanie vyrazov.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "pparser.h"
#include "pstack.h"
#include "instruction.h"
#include "ial.h"
#include "variable.h"
#include "scanner.h"
#include "ifj11.h"
#include "debug.h"
#include "xwrappers.h"

typedef struct Tpparser_data
{
    Tpstack_item item_a;
    Tpstack_item item_b;
} Tpparser_data;

/**
 * podla typu instrukcie vytvory vytvory novu instrukciu, vrari ukazatel kde bude
 * ulozeny vysledok
 * @param - ukazatel na funkciu v ktorej spracovavam vyraz
 * @param - typ instrukcie
 * @param - typ novej premennej kde bude ulozeny vysledok instrukcie
 * @param - ukazatel na zoznam instrukcii
 * @param - operand1 instrukcie
 * @param - operand2 instrukcie
 * @return - ukazatel na novu premennu
 */
static Tvariable * pparser_create_opp_inst(Thtable_item * func_ptr,
                                           enum inst_type instruction_type,
                                           enum var_type new_var_type,
                                           Tinstruction_list * inst_list,
                                           Tvariable * var_src1, Tvariable * var_src2)
{
    assert(func_ptr);
    assert(inst_list);
    assert(var_src1);

    union variable_val var_union;

    if (instruction_type == INST_CONC)
        var_union.str = NULL;

    Tvariable * new_var = variable_create(NULL, new_var_type, var_union);
    ial_htable_insert_var(new_var, func_ptr);

    union inst_operand dst = { .var = new_var };
    union inst_operand src1 = { .var = var_src1 };
    union inst_operand src2 = { .var = var_src2 };

    Tinstruction * new_instruction = inst_create(instruction_type, dst, src1, src2);
    inst_append(inst_list, new_instruction);

    return new_var;
}

/**
 * zisti ci su obe polozky EXPR
 * @param - polozka pstack
 * @param - polozka pstack
 * @return - ak su obe polozky EXPR true, inak false
 */
static bool pparser_check_expr(Tpstack_item item_a, Tpstack_item item_b)
{
    if (item_a.type == EXPR && item_b.type == EXPR)
        return true;

    xerror_msg(ERR_SYN_EXPR);
    return false;
}

/**
 * ============================================================================
 * Redukcia aritmetickych operacii, +, -, *, /, %
 * typ operandov moze byt iba number, vysledok je number
 * ============================================================================
 */

/**
 * zisti ci je operand vhodny pre aritmeticku operaciu
 * ak nie vypise chyb hlasku
 * @param - polozka pstacku
 * @return - ak je polozka typu VAR_NUM vrati true, inak false
 */
static bool pparser_check_aritmetic_operand(Tpstack_item item)
{
    if (item.type == EXPR && item.variable != NULL)
    {
        if (item.variable->type != VAR_NUM)
        {
            xerror_msg(ERR_SEM_BAD_ARITH_LIT);
            return false;
        }
        else
            return true;
    }
    else
        return false;
}

/**
 * vybere z pstack 3 polozky EXPR OPERACIA EXPR
 * a skontroluje ci su v poriadku
 * @param - 2 polozky Tpstack_item
 * @param - operacia ADD,SUB,..
 * @return true, ak kontroly dopadli ok, inak false
 */
static bool pparser_check_arithmetic(Tpparser_data * data,
                                    enum pstack_item_type type_need)
{
    assert(data);

    data->item_a = pstack_pop_item();
    /* ak je tam literal musi to byt EXPR a musi byt cislo */
     if (data->item_a.literal == true)
        if (pparser_check_aritmetic_operand(data->item_a) == false)
            return set_error(RET_ERR_SEM);

    enum pstack_item_type type = pstack_pop_type();
    if (type != type_need)
    {
        xerror_msg(ERR_MY_PSTACK_BAD_DATA);
        return set_error(RET_ERR_MY);
    }

    data->item_b = pstack_pop_item();
    if (data->item_b.literal == true)
        if (pparser_check_aritmetic_operand(data->item_b) == false)
            return set_error(RET_ERR_SEM);

    //na zasobniku museli byt 2 EXPR
    if (pparser_check_expr(data->item_a, data->item_b) == false)
        return set_error(RET_ERR_SYN);

    return true;
}

/**
 * vytvori instrukciu pre pravidlo E->E+E, E->E-E, E->E*E, E->E/E, E->E%E,
 * E->E^E
 * POZNAMKY: na vrhole pstack musi byt EXPR <> EXPR
 * @param - typ operacie na redukovanie
 * @param - typ instrukcie ktora sa vygeneruje
 * @param - ukazatel na funkciu v ktorej spracovavam vyraz
 * @param - instruction list
 * @return - true ak prebehlo vsetko v poriadku,
 * inak false - pri PSTACK_OVERFLOW, alebo chybe zjednodusenia
 */
static bool pparser_reduce_arithmetic(enum pstack_item_type reduce_type,
                                      enum inst_type instr_type,
                                      Thtable_item * ptr_func,
                                      Tinstruction_list * inst_list)
{
    assert(ptr_func);
    assert(inst_list);

    Tpparser_data items;

    if (pparser_check_arithmetic(&items, reduce_type) == false)
        return false;

    Tvariable * new_var = pparser_create_opp_inst(ptr_func, instr_type, VAR_NUM,
                                                  inst_list,
                                                  items.item_b.variable,
                                                  items.item_a.variable);
    // DEBUG_MSG("pparser_reduce_arithmetic\n");
    if (pstack_push_expr(new_var, items.item_a, items.item_b) == false)
        return false;

    return true;
}

/**
 * ============================================================================
 * Redukcia operacii s stringom .., #
 * typ operandov moze byt iba string, vysledok je string
 * ============================================================================
 */

/**
 * zisti ci je operand vhodny pre operaciu s stringom
 * ak nie vypise chyb hlasku
 * @param - polozka pstacku
 * @return - ak je polozka typu VAR_NUM vrati true, inak false
 */
static bool pparser_check_string_operand(Tpstack_item item)
{
    if (item.type == EXPR && item.variable != NULL)
    {
        if (item.variable->type != VAR_STR)
        {
            xerror_msg(ERR_SEM_BAD_STRING_LIT);
            return false;
        }
        else
            return true;
    }
    else
        return false;
}

/**
 * vybere z pstack 3 polozky EXPR OPERACIA EXPR
 * a skontroluje ci su v poriadku
 * @param - 2 polozky Tpstack_item
 * @param - operacia CONCAT
 * @return true, ak kontroly dopadli ok, inak false
 */
static bool pparser_check_string(Tpparser_data * data,
                                    enum pstack_item_type type_need)
{
    assert(data);

    data->item_a = pstack_pop_item();
    /* ak je tam literal musi to byt EXPR a musi byt retazec */
     if (data->item_a.literal == true)
        if (pparser_check_string_operand(data->item_a) == false)
            return set_error(RET_ERR_SEM);

    enum pstack_item_type type = pstack_pop_type();
    if (type != type_need)
    {
        xerror_msg(ERR_MY_PSTACK_BAD_DATA);
        return set_error(RET_ERR_MY);
    }

    data->item_b = pstack_pop_item();
    if (data->item_b.literal == true)
        if (pparser_check_string_operand(data->item_b) == false)
            return set_error(RET_ERR_SEM);

    //na zasobniku museli byt 2 EXPR
    if (pparser_check_expr(data->item_a, data->item_b) == false)
        return set_error(RET_ERR_SYN);

    return true;
}

/**
 * vytvori instrukciu pre pravidlo E->E..E (konkatenacia 2 retazcov)
 * POZNAMKY: na vrhole pstack musi byt EXPR CONCAT EXPR
 * @param - ukazatel na funkciu v ktorej spracovavam vyraz
 * @param - instruction list
 * @return - true ak prebehlo vsetko v poriadku,
 * inak false - pri PSTACK_OVERFLOW, alebo chybe zjednodusenia
 */
static bool pparser_reduce_concat(Thtable_item * ptr_func,
                                  Tinstruction_list * inst_list)
{
    assert(ptr_func);
    assert(inst_list);

    Tpparser_data items;

    if (pparser_check_string(&items, CONCAT) == false)
        return false;

    Tvariable * new_var = pparser_create_opp_inst(ptr_func, INST_CONC, VAR_STR,
                                                  inst_list,
                                                  items.item_b.variable,
                                                  items.item_a.variable);
    //DEBUG_MSG("pparser_CONCAT\n");
    if (pstack_push_expr(new_var, items.item_a, items.item_b) == false)
        return false;

    return true;
}

/**
 * vytvori instrukciu pre pravidlo E->#E (konkatenacia 2 retazcov)
 * POZNAMKY: na vrhole pstack musi byt # EXPR
 * @param - ukazatel na funkciu v ktorej spracovavam vyraz
 * @param - instruction list
 * @return - true ak prebehlo vsetko v poriadku,
 * inak false - pri PSTACK_OVERFLOW, alebo chybe zjednodusenia
 */
static bool pparser_reduce_strlen(Thtable_item * ptr_func,
                                  Tinstruction_list * inst_list)
{
    assert(ptr_func);
    assert(inst_list);

    Tpstack_item item_a = pstack_pop_item();
    /* ak je tam literal musi to byt EXPR a musi to byt retazec*/
    if (item_a.literal == true)
        if (pparser_check_string_operand(item_a) == false)
            return set_error(RET_ERR_SEM);

    enum pstack_item_type type = pstack_pop_type();
    if (type != STRLEN)
    {
        xerror_msg(ERR_MY_PSTACK_BAD_DATA);
        return set_error(RET_ERR_MY);
    }

    Tvariable * new_var = pparser_create_opp_inst(ptr_func, INST_STRLEN, VAR_NUM,
                                                  inst_list,
                                                  item_a.variable,
                                                  NULL);
    //DEBUG_MSG(pparser_CONCAT\n");
    if (pstack_push_expr(new_var, item_a, item_a) == false)
        return false;

    return true;
}

/**
 * ============================================================================
 * Redukcia logickych operacii ==, ~= , and, or, not
 * typ operandov moze byt lubovolny, vysledok je boolean
 * ============================================================================
 */

/**
 * vytvori instrukciu pre pravidlo E->E==E, E->E~=E, E->EorE, E->EandE
 * POZNAMKY: na vrhole pstack musi byt EXPR <> EXPR
 * @param - typ operacie na redukovanie
 * @param - typ instrukcie ktora sa vygeneruje
 * @param - ukazatel na funkciu v ktorej spracovavam vyraz
 * @param - instruction list
 * @return - true ak prebehlo vsetko v poriadku,
 * inak false - pri PSTACK_OVERFLOW, alebo pri chybe zjednodusenia
 * POZNAMKA: porovnavat sa mozu lubovone typy napr. nil ~= "avs"
 */
static bool pparser_reduce_logical_no_type(enum pstack_item_type reduce_type,
                                           enum inst_type instr_type,
                                           Thtable_item * ptr_func,
                                           Tinstruction_list * inst_list)
{
    assert(ptr_func);
    assert(inst_list);

    Tpstack_item item_a = pstack_pop_item();

    enum pstack_item_type type = pstack_pop_type();
    if (type != reduce_type)
    {
        xerror_msg(ERR_MY_PSTACK_BAD_DATA);
        return set_error(RET_ERR_MY);
    }

    Tpstack_item item_b = pstack_pop_item();

    //na pstacku neboli 2 EXPR
    if (pparser_check_expr(item_a, item_b) == false)
        return set_error(RET_ERR_SYN);

    Tvariable * new_var = pparser_create_opp_inst(ptr_func, instr_type, VAR_BOOL,
                                                  inst_list,
                                                  item_b.variable,
                                                  item_a.variable);
    //DEBUG_MSG("pparser_NOT_EQUAL\n");
    if (pstack_push_expr(new_var, item_a, item_b) == false)
        return false;

    return true;
}

/**
 * vytvori instrukciu pre pravidlo E->not E (negacia), vysledok je bool
 * POZNAMKY: na vrhole pstack musi byt not EXPR
 * @param - ukazatel na funkciu v ktorej spracovavam vyraz
 * @param - instruction list
 * @return - true ak prebehlo vsetko v poriadku,
 * inak false - pri PSTACK_OVERFLOW, alebo chybe zjednodusenia
 */
static bool pparser_reduce_not(Thtable_item * ptr_func,
                               Tinstruction_list * inst_list)
{
    assert(ptr_func);
    assert(inst_list);

    Tpstack_item item_a = pstack_pop_item();

    enum pstack_item_type type = pstack_pop_type();
    if (type != NOT)
    {
        xerror_msg(ERR_MY_PSTACK_BAD_DATA);
        return set_error(RET_ERR_MY);
    }

    //na pstacku neboli 2 EXPR
    if (pparser_check_expr(item_a, item_a) == false)
        return set_error(RET_ERR_SYN);

    Tvariable * new_var = pparser_create_opp_inst(ptr_func, INST_NOT, VAR_BOOL,
                                                  inst_list,
                                                  item_a.variable,
                                                  NULL);
    //DEBUG_MSG(pparser_NOT\n");
    if (pstack_push_expr(new_var, item_a, item_a) == false)
        return false;

    return true;
}

/**
 * ============================================================================
 * Redukcia logickych operacii <, >, <=, >=
 * typ operandov musi byt sucasne zhodny, cislo alebo string, vysledok je
 * boolean
 * ============================================================================
 */

/**
 * zisti ci je operand vhodny pre logicku operaciu
 * ak nie vypise chybu
 * @param - polozka pstack
 * @return - ak je polozka typu VAR_NUM alebo VAR_STR varati true
 * iank false
 */
static bool pparser_check_logical_operand(Tpstack_item item)
{
    if (item.type == EXPR && item.variable != NULL)
    {
        if (item.variable->type == VAR_NUM || item.variable->type == VAR_STR)
            return true;
        else
        {
            xerror_msg(ERR_SEM_BAD_CMP_LIT);
            return false;
        }
    }
    else
        return false;
}

/**
 * vybere z pstack 3 polozky EXPR OPERACIA EXPR
 * a skontroluje ci su v poriadku
 * @param - 2 polozky Tpstack_item
 * @param - operacia <, >, <=, >=
 * @return true, ak kontroly dopadli ok, inak false
 */
static bool pparser_check_logical_type(Tpparser_data * data,
                                    enum pstack_item_type type_need)
{
    assert(data);

    data->item_a = pstack_pop_item();
    /* ak je tam literal musi to byt EXPR a musi byt cislo */
     if (data->item_a.literal == true)
        if (pparser_check_logical_operand(data->item_a) == false)
            return set_error(RET_ERR_SEM);

    enum pstack_item_type type = pstack_pop_type();
    if (type != type_need)
    {
        xerror_msg(ERR_MY_PSTACK_BAD_DATA);
        return set_error(RET_ERR_MY);
    }

    data->item_b = pstack_pop_item();
    if (data->item_b.literal == true)
        if (pparser_check_logical_operand(data->item_b) == false)
            return set_error(RET_ERR_SEM);

    //na zasobniku museli byt 2 EXPR
    if (pparser_check_expr(data->item_a, data->item_b) == false)
        return set_error(RET_ERR_SYN);

    return true;
}

/**
 * vytvori instrukciu pre pravidlo E->E>E, E->E<E, E->E<=E, E->E>=E
 * POZNAMKY: na vrhole pstack musi byt EXPR GREATER EXPR
 * @param - typ operacie na redukovanie
 * @param - tym instrukcie ktora sa vygeneruje
 * @param - ukazatel na funkciu v ktorej spracovavam vyraz
 * @param - instruction list
 * @return - true ak prebehlo vsetko v poriadku,
 * inak false - pri PSTACK_OVERFLOW, alebo pri chybe zjednodusenia
 */
static bool pparser_reduce_logical_type(enum pstack_item_type reduce_type,
                                        enum inst_type instr_type,
                                        Thtable_item * ptr_func,
                                        Tinstruction_list * inst_list)
{
    assert(ptr_func);
    assert(inst_list);

    Tpparser_data items;

    if (pparser_check_logical_type(&items, reduce_type) == false)
        return false;

    Tvariable * new_var = pparser_create_opp_inst(ptr_func, instr_type, VAR_BOOL,
                                                  inst_list,
                                                  items.item_b.variable,
                                                  items.item_a.variable);

    if (pstack_push_expr(new_var, items.item_a, items.item_b) == false)
        return false;

    return true;
}

/**
 * vytvori instrukciu pre pravidlo E->(E)
 * POZNAMKY: na vrhole pstack musi byt BRACKET_L EXPR
 * @param - ukazatel na funkciu v ktorej spracovavam vyraz
 * @param - instruction list
 * @return - true ak prebehlo vsetko v poriadku,
 * inak false - pri PSTACK_OVERFLOW, alebo pri chybe zjednodusenia
 */
static bool pparser_reduce_bracket(void)
{

    Tpstack_item item_a = pstack_pop_item();

    enum pstack_item_type type = pstack_pop_type();
    if (type != BRACKET_L)
    {
        xerror_msg(ERR_MY_PSTACK_BAD_DATA);
        return set_error(RET_ERR_MY);
    }

    //na pstacku nebol EXPR
    if (item_a.type != EXPR)
    {
        xerror_msg(ERR_SYN_EXPR);
        return set_error(RET_ERR_SYN);
    }

    //DEBUG_MSG("pparser_BRACKET_(E)\n");
    if (pstack_push(EXPR, item_a.literal, item_a.variable) == false)
        return false;

    return true;
}

/**
 * z pravidiel na pstacku vytvara instrukcie, REDUKUJE pstack
 * @param - ukazatel na funkciu v ktorej spracovavam vyraz
 * @param - bool premenna, ak ju nastavi na true znamena ze doslo k chybe
 * @param - ukazatel na list instrukcii
 * @param - typ polozky ktora je momentalne na vstupe EXPR, ADD, SUB..
 * @return - ak sa nepodari zredukovat vrati false, inak true
 * *error - ked sa nastavi na true musim nastavit set_error,
 * a viem ze bol error a musi sa ukoncit precedencna analyza
 */
static bool pparser_reduce(Thtable_item * ptr_func, bool * error,
                           Tinstruction_list * inst_list,
                            enum pstack_item_type actual_type)
{
    assert(ptr_func);
    assert(error);
    assert(inst_list);

    /* nastala chyba? ak nastane nastavi sa aj ina navratova hodnota interpr.*/
    *error = false;

    /* operacia na vrchole zasobnika */
    enum pstack_item_type top_operation = pstack_top_type_operation();

    /* navratova hodnota tejto funkcie */
    bool ret = false;

    /*
     * podla operacie ktora sa nachadza na vrchole zasobnika
     * a podla toho aky token prisiel
     * sa zjednodusi, alebo nie
     */
    switch (top_operation)
    {
        //operacia na vrchole zasobnika (riadok v prec. tabulke)
        case ADD:
        case SUB:
        {
            //aktualny token (stlpec v prec. tabulke)
            if (actual_type == MUL || actual_type == DIV
                || actual_type == MODULO
                || actual_type == POWER)
                return false;

            if (top_operation == ADD)
                ret = pparser_reduce_arithmetic(ADD , INST_ADD, ptr_func,
                                                inst_list);
            else
                ret = pparser_reduce_arithmetic(SUB, INST_SUB, ptr_func,
                                         inst_list);
            if (ret != true)
                *error = true;

            return true;
            break;
        }

        case MUL:
        case DIV:
        case MODULO:
        {
            if (actual_type == POWER)
                return false;

            if (top_operation == MUL)
                ret = pparser_reduce_arithmetic(MUL, INST_MUL, ptr_func,
                                                inst_list);
            else if (top_operation == DIV)
                ret = pparser_reduce_arithmetic(DIV, INST_DIV, ptr_func,
                                         inst_list);
            else
                ret = pparser_reduce_arithmetic(MODULO, INST_MOD, ptr_func,
                                                inst_list);
            if (ret != true)
                *error = true;

            return true;
            break;
        }

        case CONCAT:
        {
            if (actual_type == ADD || actual_type == SUB
                || actual_type == MUL || actual_type == DIV
                || actual_type == MODULO
                || actual_type == POWER)
                return false;

            ret = pparser_reduce_concat(ptr_func, inst_list);
            if (ret != true)
                *error = true;

            return true;
            break;
        }

        case STRLEN:
        {
            if (actual_type == POWER)
                return false;

            ret = pparser_reduce_strlen(ptr_func, inst_list);
            if (ret != true)
                *error = true;

            return true;
            break;
        }

        case NOT:
        {
            if (actual_type == POWER)
                return false;

            ret = pparser_reduce_not(ptr_func, inst_list);
            if(ret != true)
                *error = true;

            return true;
            break;
        }

        case POWER:
        {
            if (actual_type == BRACKET_L)
                return false;

            ret = pparser_reduce_arithmetic(POWER, INST_POW, ptr_func,
                                            inst_list);
            if (ret != true)
                *error = true;

            return true;
            break;
        }

        case EQUAL:
        case NOT_EQUAL:
        case GREATER:
        case LESS:
        case LESS_EQUAL:
        case GREATER_EQUAL:
        {
            if (actual_type == CONCAT || actual_type == ADD
                || actual_type == SUB || actual_type == MUL
                || actual_type == DIV || actual_type == MODULO
                || actual_type == POWER)
                return false;

            if (top_operation == EQUAL)
                ret = pparser_reduce_logical_no_type(EQUAL, INST_CMPE,
                                                     ptr_func, inst_list);
            else if (top_operation == NOT_EQUAL)
                ret = pparser_reduce_logical_no_type(NOT_EQUAL, INST_CMPNE,
                                               ptr_func, inst_list);
            else if (top_operation == GREATER)
                ret = pparser_reduce_logical_type(GREATER, INST_CMPG,
                                                  ptr_func, inst_list);
            else if (top_operation == LESS)
                ret = pparser_reduce_logical_type(LESS,INST_CMPL,
                                                  ptr_func, inst_list);
            else if (top_operation == LESS_EQUAL)
                ret = pparser_reduce_logical_type(LESS_EQUAL, INST_CMPLE,
                                                  ptr_func, inst_list);
            else
                ret = pparser_reduce_logical_type(GREATER_EQUAL, INST_CMPGE,
                                                  ptr_func, inst_list);

            if (ret != true)
                *error = true;

            return true;
            break;
        }

        case AND:
        {
            if (actual_type == CONCAT || actual_type == ADD
                || actual_type == SUB || actual_type == MUL
                || actual_type == DIV || actual_type == MODULO
                || actual_type == POWER || actual_type == EQUAL
                || actual_type == NOT_EQUAL || actual_type == GREATER
                || actual_type == LESS || actual_type == LESS_EQUAL
                || actual_type == GREATER_EQUAL)
                return false;

            ret = pparser_reduce_logical_no_type(AND, INST_AND, ptr_func,
                                                 inst_list);
            if (ret != true)
                *error = true;

            return true;
            break;
        }

        case OR:
        {
            if (actual_type == CONCAT || actual_type == ADD
                || actual_type == SUB || actual_type == MUL
                || actual_type == DIV || actual_type == MODULO
                || actual_type == POWER || actual_type == EQUAL
                || actual_type == NOT_EQUAL || actual_type == GREATER
                || actual_type == LESS || actual_type == LESS_EQUAL
                || actual_type == GREATER_EQUAL || actual_type == AND)
                return false;

            ret = pparser_reduce_logical_no_type(OR, INST_OR, ptr_func, inst_list);
            if (ret != true)
                *error = true;

            return true;
            break;
        }
        case BRACKET_L:
        {
            if (actual_type != BRACKET_R)
                return false;

            ret = pparser_reduce_bracket();
            if (ret != true)
                *error = true;

            /*
             * ked som zredukoval E->(E)
             * tak vratim false aby som dalej neredukoval
             */
            return false;
            break;
        }

        /*
         * na vrchole moze byt iba START, EXPR,
         * BRACKET_R a REDUCE sa nikdy nevklada
         */
        case START:
        case EXPR:
        case BRACKET_R:
        case REDUCE:
            break;

        /*
         * interna chyba, na vrchole zasobnika
         * nemoze byt ina hodnota
         */
        default:
        {
            xerror_msg(ERR_MY_PSTACK_BAD_DATA);
            set_error(RET_ERR_MY);
            *error = true;
            return false;
            break;
        }
    }

    return false;
}

/**
 * ulozi cislo do htable pre danu funkciu pod nejakym nazvom
 * (vytvori a naplni premennu)
 * @param - ukazatel na funkciu
 * @param - cislo ktore bude v premennej
 * @return - ukazatel na novu premennu (ulozenu v htable k danej funkcii)
 */
static Tvariable * pparser_reduce_number(Thtable_item * ptr_func, double number)
{
    assert(ptr_func);

    union variable_val val_union = { .num = number };
    Tvariable * variable = variable_create(NULL, VAR_NUM, val_union);
    ial_htable_insert_var(variable, ptr_func);

    return variable;
}

/**
 * ulozi premennu typu boolean do htable (prisiel token FALSE/TRUE)
 * @param - ukazatel na funkciu
 * @param - hodnota premennej true/false
 * @return - ukazatel na novo vytvoreny premennu
 */
static Tvariable * pparser_reduce_bool(Thtable_item * ptr_func, bool boolean)
{
    assert(ptr_func);

    union variable_val val_union = { .boolean = boolean };
    Tvariable * variable = variable_create(NULL, VAR_BOOL, val_union);
    ial_htable_insert_var(variable, ptr_func);

    return variable;
}

/**
 * ulozi premennu typu nil do htable (prisiel token nil)
 * @param - ukazatel na funkciu
 * @return - ukazatel na novo vytvorenu premennu
 */
static Tvariable * pparser_reduce_nil(Thtable_item * ptr_func)
{
    assert(ptr_func);

    union variable_val val_union;
    Tvariable * variable = variable_create(NULL, VAR_NIL, val_union);
    ial_htable_insert_var(variable, ptr_func);

    return variable;
}

/**
 * ulozi string ako premennu do htable k danej funkcii (prisiel tokek STRING)
 * @param - ukazatel na funkciu
 * @param - string
 * @return - ukazatel na novo vytvorenu premennu, kde ju ulozeny string
 */
static Tvariable * pparser_reduce_string(Thtable_item * ptr_func,
                                         char * string)
{
    assert(ptr_func);
    assert(string);

    union variable_val val_union = { .str = string};
    Tvariable * variable = variable_create(NULL, VAR_STR, val_union);
    ial_htable_insert_var(variable, ptr_func);

    return variable;
}

/**
 * vrati ukazatel na premennu ak sa podarilo vsetko uspesne
 * zredukovat a na zasobniku ostalo iba START EXPR,
 * @param - void
 * @return - ukazatel na premennu z EXPR,
 * ak sa nepodarilo zredukovat NULL
 */
static Tvariable * pparser_reduce_all(Thtable_item * ptr_func)
{
    bool reduce_error = false;

    while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        REDUCE) != false)
        if (reduce_error == true)
            return NULL;

    if (pstack_top_type() == EXPR)
    {
        Tpstack_item item = pstack_pop_item();
        if (pstack_top_type() != START)
        {
            //na zasobniku nie je START
            set_error(RET_ERR_SYN);
            xerror_msg(ERR_SYN_EXPR);
            return NULL;
        }

        return item.variable;
    }
    else
    {
        //nepodarilo sa zredukovat
        set_error(RET_ERR_SYN);
        xerror_msg(ERR_SYN_EXPR);
        return NULL;
    }
}

/**
 * funkcia spracovava vyrazi pomocou precedencnej analyzi
 * @param - hash tabulka
 * @param - ukazatel na funkciiu (do htable) z ktorej ma volas
 * @param - token, (ked ma volas tak mas precitany moj prvy token -
 *          chem jeho obsah)
 *        - nasledne ked koncim pracitam tvoj token, ulozim ti jeho obsah
 * @param - ak idem spracovat vyraz v write, volaj s true
 * @return - premenna s vysledkom
 * POZNAMKY:
 * https://wis.fit.vutbr.cz/FIT/st/phorum-msg-show.php?id=23631&mode=mthr
 */
Tvariable * pparser(Thtable * htable, Thtable_item * ptr_func,
                    Ttoken * token, bool write)
{
    assert(htable);
    assert(ptr_func);
    assert(token);

    //inicializcia pbuffera a vlozenie START = '$' na vrchol
    pstack_init();
    pstack_push_start();

    //aky symbol je na vrchole zasobnika
    int top = pstack_top_type();

    //uchovava navratovu hodnotu funkcie scanner_next_token;
    bool ret_next_token = true;
    bool reduce_error = false;

    /*
     * v cykle sa zisti novy token, a aka polozka
     * je na vrchole pstacku
     * a podla pravidiel sa urci ci sa bude redukovat alebo shiftovat
     * novy token sa prebera na konci
     */
    while (ret_next_token != false)
    {
        top = pstack_top_type();

        switch (token->id)
        {
            case TKN_NUMBER:
            {
                if (top == EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                //vytvory sa nova premenna v htable pre danu funkciu
                //a nakopiruje sa tam hodnota - negenerujem instrukciu
                Tvariable * variable = pparser_reduce_number(ptr_func,
                                                             token->val.num);

                //vlozim cislo na vrcho zasobnika pod EXPR
                if (pstack_push(EXPR, true, variable) == false)
                    return NULL;

                break;
            }

            case TKN_KW_TRUE:
            case TKN_KW_FALSE:
            {
                if (top == EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                bool boolean;
                if (token->id == TKN_KW_TRUE)
                    boolean = true;
                else
                    boolean = false;

                Tvariable * variable = pparser_reduce_bool(ptr_func, boolean);

                if (pstack_push(EXPR, true, variable) == false)
                    return NULL;

                break;
            }

            case TKN_KW_NIL:
            {
                if (top == EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                Tvariable * variable = pparser_reduce_nil(ptr_func);

                if (pstack_push(EXPR, true, variable) == false)
                    return NULL;

                break;
            }

            case TKN_STRING:
            {
                DEBUG_MSG("prisiel string\n");
                if (top == EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    xfree(token->val.str);
                    return NULL;
                }

                Tvariable * variable = pparser_reduce_string(ptr_func, token->val.str);
                if (pstack_push(EXPR, true, variable) == false)
                    return NULL;

                break;
            }
            case TKN_ID:
            {
                //prisiel identifikator
                if (top == EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                Thtable_item * func = ial_htable_look_func(htable, token->val.str);

                /* ak je nie je null znamena ze je to id funkcie co je syn chyba */
                if (func != NULL)
                {
                    set_error(RET_ERR_SEM);
                    xerror_msg(ERR_SEM_FUNC_IN_EXPR);
                    xfree(token->val.str);
                    return NULL;
                }

                Tvariable * variable = ial_htable_look_var(ptr_func, token->val.str);
                if (variable == NULL)
                {
                    set_error(RET_ERR_SEM);
                    xerror_msg(ERR_SEM_UNDEFINED_ID_EXPR);
                    xfree(token->val.str);
                    return NULL;
                }

                /* identifikator je ok existuje ale treba vymazat jeho "meno" :D*/
                xfree(token->val.str);
                
                /*
                 * preco sa musi vytvarat nova premenna?
                 * aby bola zachovana integrita
                 * s  = "saasa"; - "saasa" je ulozene pod menom NULL
                 * a = s;
                 * lenze teraz ked sa zmeni s tak sa zmeni aj a
                 * preto treba vytvorit novu premennu
                 */

                union variable_val val_union;
                Tvariable * new_var = variable_create(NULL, VAR_NIL, val_union);
                union inst_operand operand1 = { .var = new_var};
                union inst_operand operand2 = { .var = variable};
                union inst_operand operand3 = { .var = NULL};
                inst_append(&ptr_func->inst, inst_create(INST_CPY, operand1, operand2, operand3));

                /*
                if (variable->type == VAR_STR)
                {
                    size_t s_len = strlen(variable->val.str);
                    char * s = xmalloc((s_len + 1) * sizeof(char));
                    s = strcpy(s, variable->val.str);
                    
                    union variable_val val = { .str = s};

                    printf("toto %s\n", val.str);
                    new_var = variable_create(NULL, VAR_STR, val);
                }
                else
                    new_var = variable_create(NULL, variable->type,  variable->val);
                */

                ial_htable_insert_var(new_var, ptr_func);
                if (pstack_push(EXPR, false, new_var) == false)
                    return NULL;
                break;
            }

            case TKN_OP_LESS:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                //redukuje az pokial nie je co redukovat
                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        LESS) != false)
                    if (reduce_error == true)
                    {
                        return NULL;
                    }

                if (pstack_push(LESS, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_GREATER:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                //redukuje az pokial nie je co redukovat
                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        GREATER) != false)
                    if (reduce_error == true)
                    {
                        return NULL;
                    }

                if (pstack_push(GREATER, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_LESS_EQUAL:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                //redukuje az pokial nie je co redukovat
                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        LESS_EQUAL) != false)
                    if (reduce_error == true)
                    {
                        return NULL;
                    }

                if (pstack_push(LESS_EQUAL, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_GREATER_EQUAL:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                //redukuje az pokial nie je co redukovat
                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        GREATER_EQUAL) != false)
                    if (reduce_error == true)
                    {
                        return NULL;
                    }

                if (pstack_push(GREATER_EQUAL, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_NOT_EQUAL:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                //redukuje az pokial nie je co redukovat
                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        NOT_EQUAL) != false)
                    if (reduce_error == true)
                    {
                        return NULL;
                    }

                if (pstack_push(NOT_EQUAL, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_EQUAL:
            {
                 if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                //redukuje az pokial nie je co redukovat
                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        EQUAL) != false)
                    if (reduce_error == true)
                    {
                        return NULL;
                    }

                if (pstack_push(EQUAL, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_CONC:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                while(pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        CONCAT) != false)
                    if (reduce_error == true)
                        return NULL;

                if (pstack_push(CONCAT, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_ADD:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        ADD) != false)
                    if (reduce_error == true)
                        return NULL;

                if (pstack_push(ADD, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_SUB:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        SUB) != false)
                    if (reduce_error == true)
                        return NULL;

                if (pstack_push(SUB, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_MUL:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

              while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                    MUL) != false)
                    if (reduce_error == true)
                        return NULL;

                if (pstack_push(MUL, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_DIV:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        DIV) != false)
                    if (reduce_error == true)
                        return NULL;

                if (pstack_push(DIV, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_MOD:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        MODULO) != false)
                    if (reduce_error == true)
                        return NULL;

                if (pstack_push(MODULO, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_OP_STRLEN:
            {
                if (top == EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                if (pstack_push(STRLEN, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_RW_NOT:
            {
                if (top == EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                if (pstack_push(NOT, false, NULL) == false)
                    return NULL;

                break;
            }
            case TKN_OP_POWER:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                if (pstack_push(POWER, false, NULL) == false)
                    return NULL;

                break;
            }

            /*
             * nic sa neredukuje zatvorka sa da iba na zasobnik (
             */
            case TKN_BRACKET_L:
            {
                /*ak je EXPR( tak je to chyba */
                if (top == EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                if (pstack_push(BRACKET_L, false, NULL) == false)
                    return NULL;

                break;
            }

            case TKN_BRACKET_R:
            {
                /* ak je START), () alebo <+-* /..>) tak je chyba */
                if (top == START || top == BRACKET_L || top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                /* index prvej lavej zatvorky */
                int index_bracket_left = pstack_top_bracket_left_index();

                if (index_bracket_left == NO_BRACKET && write == false)
                {
                    /* nie som v prikaze write a nemam lavu zatvorku na zasobniku
                     * znamena chybu
                     */
                    set_error(RET_ERR_SYN);
                    xerror_msg(ERR_SYN_NO_L_BRACKET);
                    return NULL;
                }

                else if (index_bracket_left == NO_BRACKET && write == true)
                {
                    /*
                    * na zasobniku nie je lava zatvorka ale som v prikaze write
                    * tak musim vsetko zredukovat
                    */
                    Tvariable * variable = pparser_reduce_all(ptr_func);

                    return variable;
                }

                /*
                 * redukujem az kym sa index lavej zatvorky
                 * nenahradi za EXPR - zredukovane
                 */
                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst,
                        BRACKET_R) != false && pstack_top_index() != index_bracket_left)
                {
                    if (reduce_error == true)
                        return NULL;
                }

                //pravu zatvorku na stack nedavam
                break;
            }

            case TKN_RW_AND:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst, AND)
                        != false)
                    if (reduce_error == true)
                    return NULL;

                if (pstack_push(AND, false, NULL) == false)
                    return NULL;
                break;
            }

            case TKN_RW_OR:
            {
                if (top != EXPR)
                {
                    xerror_msg(ERR_SYN_EXPR);
                    set_error(RET_ERR_SYN);
                    return NULL;
                }

                while (pparser_reduce(ptr_func, &reduce_error, &ptr_func->inst, OR)
                       != false)
                    if (reduce_error == true)
                        return NULL;

                if (pstack_push(OR, false, NULL) == false)
                    return NULL;
                break;
            }
            case TKN_KW_THEN:
            case TKN_KW_DO:
            case TKN_SEMICOL:
            case TKN_COMMA:
            {
                Tvariable * variable = pparser_reduce_all(ptr_func);
                return variable;

                break;
            }

            /* tokeny pri ktorych je syntakticka chyba */
            case TKN_OP_ASSIGNMENT:
            case TKN_KW_IF:
            case TKN_KW_ELSE:
            case TKN_KW_LOCAL:
            case TKN_KW_END:
            case TKN_KW_WHILE:
            case TKN_KW_READ:
            case TKN_KW_WRITE:
            case TKN_KW_FUNCTION:
            case TKN_KW_RETURN:
            case TKN_RW_BREAK:
            case TKN_RW_ELSEIF:
            case TKN_RW_FOR:
            case TKN_RW_IN:
            case TKN_RW_REPEAT:
            case TKN_RW_UNTIL:
            case TKN_BI_TYPE:
            case TKN_BI_SUBSTR:
            case TKN_BI_FIND:
            case TKN_BI_SORT:
            case TKN_EOF:
            default:
            {
                //syntakticka chyba
                xerror_msg(ERR_SYN_UNEXP_KEY_EXP);
                set_error(RET_ERR_SYN);
                return NULL;
                break;
            }
        }

        ret_next_token = scanner_next_token(token);
    }

    return NULL;
}

