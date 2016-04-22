/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       interpret.c
 * Autor(i):    Fridolin Pokorny
 *
 * Implementacia interpretru.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stddef.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <assert.h>

#include "interpret.h"
#include "ifj11.h"
#include "xwrappers.h"
#include "instruction.h"
#include "variable.h"
#include "ial.h"
#include "buildin.h"
#include "debug.h"

#define DEQUAL(D1, D2)        (fabs(D1 - D2) < DBL_EPSILON)

enum interpret_cmp_le {
    INT_CMP_CMPL,
    INT_CMP_CMPG,
    INT_CMP_CMPLE,
    INT_CMP_CMPGE,
};

/**
 * Funkcia skontroluje operandy aritmetickych instrukcii.
 * @param - prvy operand aritmetickej operacie
 * @param - druhy operand aritmetickej operacie
 * @return - v pripade, ze aritmeticku operaciu nad operandmi je mozne vykonat
 *           true
 */
static bool interpret_check_arithmetic(const Tvariable * src1,
                                              const Tvariable * src2)
{
    if (src1->type != VAR_NUM || src2->type != VAR_NUM)
    {
        xerror_msg(ERR_INT_BAD_ARITH_OP);
        return set_error(RET_ERR_INT);
    }
    else
        return true;
}

/**
 * Funkcia skontroluje operandy pri porovnavacich operaciach rovnost a
 * nerovnost.
 * @param - prvy operand porovnavacej operacie
 * @param - druhy operand porovnavacej operacie
 */
static bool interpret_equal(Tvariable * dst,
                            const Tvariable * src1,
                            const Tvariable * src2)
{
    /* Pri porovnavani retazovom si musim pamatat vysledok, aby som mohol pamat
     * korektne uvolnit. Pamata sa z dovodu, aby jeden zo src1 a src2 (pripadne
     * oba) mohli byt zaroven cielovym (dst).
     * */
    bool diff;


    if (src1->type == src2->type)
    {
        switch (src1->type)
        {
            case VAR_NUM:
                dst->type = VAR_BOOL;
                dst->val.boolean = DEQUAL(src1->val.num, src2->val.num);

                break;
            case VAR_BOOL:
                dst->type = VAR_BOOL;
                dst->val.boolean = src1->val.boolean
                                   == src2->val.boolean;

                break;
            case VAR_NIL:
                dst->type = VAR_BOOL;
                dst->val.boolean = true;

                break;
            case VAR_STR:
                diff = strcmp(src1->val.str, src2->val.str);
                dst->type = VAR_BOOL;
                dst->val.boolean = ! diff;

                break;
            default:
                assert(0);
                break;
        }
    }
    else
    {
        dst->type = VAR_BOOL;
        dst->val.boolean = false;
    }

    return true;
}

/**
 * Skontroluje, aby ciselne operandy neboli nepovolene hodnoty NAN a INFINITY.
 * @param - ciselny operand k skontrolovaniu
 * @return - true, ak cislo nadobuda spravne hodnoty
 */
static inline bool interpret_check_number(const Tvariable * var)
{
    if (var->type == VAR_NUM
        && ! isnan(var->val.num) && ! isinf(var->val.num))
        return true;
    else
        xerror_msg(ERR_INT_NUM_RANGE);
        return set_error(RET_ERR_INT);
}

/**
 * Vyhodnoti vyraz aby odpovedala semantika jazyku Lua.
 * @param - premenna nad ktorou sa vyraz vyhodnocuje
 * @return - true, ak premenna nadobuda boolovksu hodnotu true
 */
static bool interpret_ifj11_bool(const Tvariable * var)
{
    if (var->type == VAR_NIL
        || (var->type == VAR_BOOL && var->val.boolean == false))
        return false;
    else
        return true;
}

/**
 * Implementacia modula podla jazyka Lua.
 * @param - premenna do ktorej sa ma vysledok ulozit
 * @param - prvy operand operacie modulo
 * @param - druhy operand operacie modulo
 */
static bool interpret_mod(Tvariable * dst,
                           const Tvariable * src1,
                           const Tvariable * src2)
{
    if (! interpret_check_arithmetic(src1, src2))
        return set_error(RET_ERR_INT);

    if (! interpret_check_number(src1) || ! interpret_check_number(src2))
        return set_error(RET_ERR_INT);

    dst->type = VAR_NUM;
    dst->val.num = src1->val.num
                   - floor(src1->val.num / src2->val.num) * src2->val.num;
    return true;
}

static bool interpret_ifj11_le(enum interpret_cmp_le operation,
                               Tvariable * dst,
                               const Tvariable * src1,
                               const Tvariable * src2)
{
    /* Pri porovnavani retazovom si musim pamatat vysledok, aby som mohol pamat
     * korektne uvolnit. Pamata sa z dovodu, aby jeden zo src1 a src2 (pripadne
     * oba) mohli byt zaroven cielovym (dst).
     * */
    int diff;

    if (src1->type == src2->type)
    {
        switch (operation)
        {
            case INT_CMP_CMPL:
                switch (src1->type)
                {
                    case VAR_NUM:
                        dst->type = VAR_BOOL;
                        dst->val.boolean = src1->val.num < src2->val.num;
                        break;
                    case VAR_STR:
                        diff = strcmp(src1->val.str, src2->val.str);
                        dst->type = VAR_BOOL;
                        dst->val.boolean = diff < 0;

                        break;
                    case VAR_NIL:
                    case VAR_BOOL:
                        xerror_msg(ERR_INT_BAD_CMP);
                        return set_error(RET_ERR_INT);
                    default:
                            assert(0);
                            break;
                }

                break;
            case INT_CMP_CMPG:
                switch (src1->type)
                {
                    case VAR_NUM:
                        dst->type = VAR_BOOL;
                        dst->val.boolean = src1->val.num > src2->val.num;
                        break;
                    case VAR_STR:
                        diff = strcmp(src1->val.str, src2->val.str);
                        dst->type = VAR_BOOL;
                        dst->val.boolean = diff > 0;

                        break;
                    case VAR_NIL:
                    case VAR_BOOL:
                        xerror_msg(ERR_INT_BAD_CMP);
                        return set_error(RET_ERR_INT);

                        break;
                    default:
                            assert(0);
                            break;
                }

                break;
            case INT_CMP_CMPLE:
                switch (src1->type)
                {
                    case VAR_NUM:
                        dst->type = VAR_BOOL;
                        dst->val.boolean = src1->val.num <= src2->val.num;
                        break;
                    case VAR_STR:
                        diff = strcmp(src1->val.str, src2->val.str);
                        dst->type = VAR_BOOL;
                        dst->val.boolean = diff <= 0;
                        break;
                    case VAR_NIL:
                    case VAR_BOOL:
                        xerror_msg(ERR_INT_BAD_CMP);
                        return set_error(RET_ERR_INT);
                    default:
                            assert(0);
                            break;
                }

                break;
            case INT_CMP_CMPGE:
                switch (src1->type)
                {
                    case VAR_NUM:
                        dst->type = VAR_BOOL;
                        dst->val.boolean = src1->val.num >= src2->val.num;
                        break;
                    case VAR_STR:
                        diff = strcmp(src1->val.str, src2->val.str);
                        dst->type = VAR_BOOL;
                        dst->val.boolean = diff >= 0;
                        break;
                    case VAR_NIL:
                    case VAR_BOOL:
                        xerror_msg(ERR_INT_BAD_CMP);
                        return set_error(RET_ERR_INT);
                    default:
                            assert(0);
                            break;
                }

                break;
            default:
                assert(0);
                break;
        }
    }
    else
    {
        xerror_msg(ERR_INT_DIFF_CMP);
        return set_error(RET_ERR_INT);
    }

    return true;
}

/**
 * Interpret spusti vykonavanie instrukcii od zadanej instrukcie.
 * @param - prva instrukcia pre interpretaciu
 * @return - true v pripade spravnej interpretacii
 * Funkcia nastavuje hodnotu globalnej premennej ifj11_error.
 */
bool interpret_run(Tinstruction * inst)
{
    Tvariable * dst;
    Tvariable * src1;
    Tvariable * src2;
    /*
     * Pri instrukcii INST_STRLEN je nutne si zapamatat dlzku retazca.
     */
    size_t size;

    while (inst)
    {
        DEBUG_PRINT_INSTRUCTION(inst);
        switch (inst->opcode)
        {
            case INST_ADD:
                dst = inst->dst.var;
                src1 = inst->src1.var;
                src2 = inst->src2.var;

                if (interpret_check_arithmetic(src1, src2)
                    &&
                    interpret_check_number(src1)
                    &&
                    interpret_check_number(src2))
                {
                    dst->type = VAR_NUM;
                    dst->val.num = src1->val.num + src2->val.num;
                    /* Skontroluj pretecenie vysledku */
                    if (! interpret_check_number(dst))
                        return set_error(RET_ERR_INT);
                }
                else
                    return set_error(RET_ERR_INT);

                break;
            case INST_MUL:
                dst = inst->dst.var;
                src1 = inst->src1.var;
                src2 = inst->src2.var;

                if (interpret_check_arithmetic(src1, src2)
                    &&
                    interpret_check_number(src1)
                    &&
                    interpret_check_number(src2))
                {
                    dst->type = VAR_NUM;
                    dst->val.num = src1->val.num * src2->val.num;
                    /* Skontroluj pretecenie vysledku */
                    if (! interpret_check_number(dst))
                        return set_error(RET_ERR_INT);
                }
                else
                    return set_error(RET_ERR_INT);

                break;
            case INST_POW:
                dst = inst->dst.var;
                src1 = inst->src1.var;
                src2 = inst->src2.var;

                if (interpret_check_arithmetic(src1, src2)
                    &&
                    interpret_check_number(src1)
                    &&
                    interpret_check_number(src2))
                {
                    dst->type = VAR_NUM;
                    dst->val.num = pow(src1->val.num, src2->val.num);
                    /* Skontroluj pretecenie vysledku */
                    if (! interpret_check_number(dst))
                        return set_error(RET_ERR_INT);
                }
                else
                    return set_error(RET_ERR_INT);

                break;
            case INST_DIV:
                dst = inst->dst.var;
                src1 = inst->src1.var;
                src2 = inst->src2.var;

                if (interpret_check_arithmetic(src1, src2)
                    &&
                    interpret_check_number(src1)
                    &&
                    interpret_check_number(src2))
                {
                    dst->type = VAR_NUM;
                    dst->val.num = src1->val.num / src2->val.num;
                    /* Skontroluj pretecenie vysledku */
                    if (! interpret_check_number(dst))
                        return set_error(RET_ERR_INT);
                }
                else
                    return set_error(RET_ERR_INT);

                break;

            case INST_MOD:
                if (! interpret_mod(inst->dst.var,
                                   inst->src1.var, inst->src2.var))
                    return set_error(RET_ERR_INT);
                break;

            case INST_SUB:
                dst = inst->dst.var;
                src1 = inst->src1.var;
                src2 = inst->src2.var;

                if (interpret_check_arithmetic(src1, src2)
                    &&
                    interpret_check_number(src1)
                    &&
                    interpret_check_number(src2))
                {
                    dst->type = VAR_NUM;
                    dst->val.num = src1->val.num - src2->val.num;
                    /* Skontroluj pretecenie vysledku */
                    if (! interpret_check_number(dst))
                        return set_error(RET_ERR_INT);
                }
                else
                    return set_error(RET_ERR_INT);

                break;

            case INST_AND:
                dst = inst->dst.var;
                src1 = inst->src1.var;
                src2 = inst->src2.var;

                dst->type = VAR_BOOL;
                if (interpret_ifj11_bool(inst->src1.var)
                    && interpret_ifj11_bool(inst->src2.var))
                {
                    dst->val.boolean = true;
                }
                else
                {
                    dst->val.boolean = false;
                }

                break;
            case INST_OR:
                dst = inst->dst.var;
                src1 = inst->src1.var;
                src2 = inst->src2.var;


                dst->type = VAR_BOOL;
                if (interpret_ifj11_bool(inst->src1.var)
                    || interpret_ifj11_bool(inst->src2.var))
                {
                    dst->val.boolean = true;
                }
                else
                {
                    dst->val.boolean = false;
                }

                break;
            case INST_NOT:
                dst = inst->dst.var;
                src1 = inst->src1.var;

                dst->type = VAR_BOOL;
                if (interpret_ifj11_bool(inst->src1.var))
                {
                    dst->val.boolean = false;
                }
                else
                {
                    dst->val.boolean = true;
                }

                break;
            case INST_CMPL:
                if (! interpret_ifj11_le(INT_CMP_CMPL,
                                         inst->dst.var,
                                         inst->src1.var,
                                         inst->src2.var))
                    return set_error(RET_ERR_INT);

                break;
            case INST_CMPG:
                 if (! interpret_ifj11_le(INT_CMP_CMPG,
                                          inst->dst.var,
                                          inst->src1.var,
                                          inst->src2.var))
                    return set_error(RET_ERR_INT);


               break;
            case INST_CMPLE:
                 if (! interpret_ifj11_le(INT_CMP_CMPLE,
                                          inst->dst.var,
                                          inst->src1.var,
                                          inst->src2.var))
                    return set_error(RET_ERR_INT);

                break;
           case INST_CMPGE:
                  if (! interpret_ifj11_le(INT_CMP_CMPGE,
                                           inst->dst.var,
                                           inst->src1.var,
                                           inst->src2.var))
                    return set_error(RET_ERR_INT);

                break;
            case INST_CMPE:
                if (! interpret_equal(inst->dst.var,
                                      inst->src1.var,
                                      inst->src2.var))
                    return set_error(RET_ERR_INT);

                break;
            case INST_CMPNE:
                if (! interpret_equal(inst->dst.var,
                                      inst->src1.var,
                                      inst->src2.var))
                    return set_error(RET_ERR_INT);

               /*
                 * Operator ~= sa prevedie na negovany operator ==.
                 */
                inst->dst.var->val.boolean = ! inst->dst.var->val.boolean;

                break;
            case INST_CONC:
                if (! buildin_conc(inst->dst.var, inst->src1.var, inst->src2.var))
                {
                    return set_error(RET_ERR_INT);
                }

                break;
            case INST_CLR:
                inst->dst.var->type = VAR_NIL;

                break;
            case INST_INC:
                dst = inst->dst.var;
                if (interpret_check_arithmetic(dst, dst)
                    && interpret_check_number(dst))
                {
                    dst->type = VAR_NUM;
                    dst->val.num = dst->val.num + 1;
                    /* Skontroluj pretecenie vysledku */
                    if (! interpret_check_number(dst))
                        return set_error(RET_ERR_INT);
                }
                else
                    return set_error(RET_ERR_INT);

                break;
            case INST_JMP:
                /*
                 * Vykonavanie nebude pokracovan linearne v zozname, ale nastavi
                 * sa adresa z instrukcie a pokracuje sa zaciatocnou instrukciu.
                 */
                inst = inst->dst.inst;
                continue;

                break;
            case INST_JNIF:
                /*
                 * Skokova instrukcia sa prevedie len ak src1 ~= true.
                 * Ak je true, pokracuje instrukciou dst.
                 */
                if (! interpret_ifj11_bool(inst->src1.var))
                {
                    inst = inst->dst.inst;
                    continue;
                }

                break;
            case INST_JIF:
                /*
                 * Skokova instrukcia sa prevedie len ak src1 == true.
                 * Ak je true, pokracuje instrukciou dst.
                 */
                if (interpret_ifj11_bool(inst->src1.var))
                {
                    inst = inst->dst.inst;
                    continue;
                }

                break;
            case INST_MOV:
                if (inst->dst.var)
                {
                    inst->dst.var->val = inst->src1.var->val;
                    inst->dst.var->type = inst->src1.var->type;
                }

                break;
            case INST_CPY:
                if (inst->dst.var)
                {
                    inst->dst.var->type = inst->src1.var->type;
                    if (inst->src1.var->type == VAR_STR)
                    {
                        inst->dst.var->val.str = xmalloc(strlen(inst->src1.var->val.str)+1);
                        strcpy(inst->dst.var->val.str, inst->src1.var->val.str);
                    }
                    else
                        inst->dst.var->val = inst->src1.var->val;
                }

                break;
            case INST_LEAVE:
                /*
                 * Obnovi zalohu a pokracuje sa vratenou instrukciou -
                 * instrukcia, ktora bola ulozena pri volani INST_JMP.
                 */
                inst = stack_pop();
                continue;

                break;
            case INST_ENTER:
                stack_push(inst->dst.tab);

                break;
            case INST_NOP:

                break;
            case INST_CALL:
                /*
                 * Instrukcia ulozi navratovu adresu z podprogramu (prva
                 * vykonavana instrukcia pri navrate z podpogramu) a vygeneruje
                 * skok do podpogramu.
                 */
                stack_set_rv(inst->next);
                /*
                 * Je nunte vynulovat navratovu adresu podprogramu, do ktoreho
                 * sa skace.
                 */
                if (inst->dst.tab->ret_value.type == VAR_STR)
                    xfree(inst->dst.tab->ret_value.val.str);

                inst->dst.tab->ret_value.type = VAR_NIL;
                /*
                 * Dalsia vykonavana instrukcia bude prva instrukcia volanej
                 * funkcie.
                 */
                inst = inst->dst.tab->inst.first;
                continue;

                break;

            case INST_READ:
                if (! buildin_read(inst->dst.var, inst->src1.var))
                    return set_error(RET_ERR_INT);

                break;
            case INST_WRITE:
                if (! buildin_write(inst->src1.var))
                    return set_error(RET_ERR_INT);

                break;
            case INST_TYPE:
                if (! buildin_type(inst->dst.var, inst->src1.var))
                    return set_error(RET_ERR_INT);

                break;
            case INST_SUBSTR:
                if (! buildin_substr(inst->dst.var,
                                     inst->src1.var,
                                     inst->src2.var))
                    return set_error(RET_ERR_INT);

                break;
            case INST_STRLEN:
                if (inst->src1.var->type == VAR_STR)
                {
                    size = strlen(inst->src1.var->val.str);

                    inst->dst.var->type = VAR_NUM;
                    inst->dst.var->val.num = size;
                }
                else
                {
                    xerror_msg(ERR_INT_STRLEN_OP);
                    return set_error(RET_ERR_INT);
                }
                break;

            case INST_FIND:
                if (! buildin_find(inst->dst.var,
                                   inst->src1.var,
                                   inst->src2.var))
                    return set_error(RET_ERR_INT);

                break;
            case INST_SORT:
                if (! buildin_sort(inst->dst.var, inst->src1.var))
                    return set_error(RET_ERR_INT);

                break;
            default:
                assert(0);
                break;
        }

        inst = inst->next;
    }

    return true;
}
