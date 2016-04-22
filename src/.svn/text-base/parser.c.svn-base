/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       parser.c
 * Autor(i):    Miroslav Lisik
 *
 * Implementacia syntaxou riadeneho prekladu.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "ifj11.h"
#include "debug.h"
#include "ial.h"
#include "variable.h"
#include "scanner.h"
#include "parser.h"
#include "pparser.h"
#include "instruction.h"
#include "interpret.h"
#include "xwrappers.h"

/**
 * Konstanta udava velkost pola pre hash tabulku.
 * Dohodli sme sa na velkosti 16
 */
#define HASH_SIZE 16

/* typ generovanej instrukcie */
enum gen_inst_type
{
    NONE = 0,
    WRITE,
    READ,
    ASSIGN,
    RETURN,
    WHILE,
    IF,
    FUN,
};

/* static premenna pre typ generovanej instrukcie */
static enum gen_inst_type gen_inst = NONE;

/**
 * static premenna pre ulozenie ukazatela do tabulky
 */
static Thtable * symbol_table = NULL;

/**
 * static premenna s ukazatelom na akutalnu polozku s funkciou v tabulke
 * symolov
 */
static Thtable_item * current_func = NULL;

/**
 * premenna pre pocet spracovanych argumentov funkcie
 */
static int arg_count;

/**
 * static premenna s nazvom premennej, ktora sa definuje
 */
static char * current_var = NULL;

/**
 * premenna s ukazatelom do tabulky symbolov na premennu do ktorej sa bude
 * priradzovat hodnota
 */
static Tvariable * assign_var = NULL;

/**
 * premenna s ukazatelom na volanu funkciu
 */
static Thtable_item * calling_func = NULL;

/** deklaracia funkcii ktore sa budu volat skor nez su definovane */
static bool df_list(Ttoken * token);
static bool stat(Ttoken * token);

/**
 * =============================================================================
 * BEGIN pomocne funkcie pre pracu s datovymi strukturami
 * =============================================================================
 */

/**
 * Kontroluje, ci aktualna funkcia je main.
 * @param - ukazatel na aktualnu funkciu
 * @return - true ak je aktualna funkcia main.
 */
static bool current_main(Thtable_item * current_func)
{
    if (strcmp("main", current_func->key) == 0)
        return true;
    else
        return false;
}

/**
 * Funkcia vrati hodnotu identifikatora zo struktury token.
 * @param - token
 * @return - hodnota tokenu
 */
static void set_variable_val(Ttoken * token, union variable_val * val)
{
    switch (token->id)
    {
        case TKN_NUMBER:
            val->num = token->val.num;
            break;
        case TKN_STRING:
            val->str = token->val.str;
            break;
        case TKN_KW_TRUE:
            val->boolean = true;
            break;
        case TKN_KW_FALSE:
            val->boolean = false;
            break;
        default:
            /* NIL */
            val->num = 0.0; /* nieco, kvoli poctu parametrov funkcie */
            break;
    }
}


/**
 * Nastavi typ premenne podla hodnoty v tokene
 * @param - token
 * @return - hodnota tokenu
 */
static void set_variable_type(Ttoken * token, enum var_type * type)
{
    switch (token->id)
    {
        case TKN_NUMBER:
            *type = VAR_NUM;
            break;
        case TKN_STRING:
            *type = VAR_STR;
            break;
        case TKN_KW_TRUE:
        case TKN_KW_FALSE:
            *type = VAR_BOOL;
            break;
        default:
            /* NIL */
            *type = VAR_NIL;
            break;
    }
}

/**
 * =============================================================================
 * END pomocne funkcie pre pracu s datovymi strukturami
 * =============================================================================
 */

/**
 * Wrapper na volanie precedencnej syntaktickej analyzy
 * @param - premenna na ulozenie vysledku
 * @return - pri uspechu true, inak false
 */
static bool expr(Tvariable ** expr_result, Ttoken * token)
{
    DEBUG_MSG("expr()\n");
    /* deklaracia premennych pre generovanie instruckie */
    union inst_operand dst;
    union inst_operand src1;
    union inst_operand src2 = {.inst = NULL};

    /* navratova hodnota pparser je ukazatel na premennu do tabulky symbolov */
    Tvariable * ppvar;

    /* volanie precedencnej syntaktickej analyzy */
    /* nastavime premennu, aby nenastal problem so zatvorkou ) v pparser */
    bool in_write = (gen_inst == WRITE) ? true : false;
    ppvar = pparser(symbol_table, current_func, token, in_write);
    if (ppvar == NULL)
    {
        DEBUG_MSG("ERROR: chyba v precedencnej analyze\n");
        return false;
    }
    /* kontrola potreby ulozit vysledok, pre inu funkciu */
    if (expr_result)
        *expr_result = ppvar;

    /* generovanie instrickii */
    switch (gen_inst)
    {
        case WRITE:
            /* generovanie write instrukcie */
            /* nastavenie operandov */
            dst.inst = NULL;
            src1.var = ppvar;
            src2.inst = NULL;
            inst_append(&current_func->inst,
                    inst_create(INST_WRITE, dst, src1, src2));
            break;
        case ASSIGN:
            dst.var = assign_var; /* premenna kam sa ma priradzovat */
            assign_var = NULL;
            src1.var = ppvar; /* ukazatel na premennu do tabulky symbolov */
            src2.inst = NULL;
            /* generujeme instrukciu priradenia do premennej */
            inst_append(&current_func->inst,
                    inst_create(INST_MOV, dst, src1, src2));
            break;
        case RETURN:
            dst.var = &current_func->ret_value;
            src1.var = ppvar;
            /* generovanie instrukcie priradenia navratovej hodnoty */
            inst_append(&current_func->inst, inst_create(INST_MOV, dst, src1, src2));
            /* generujeme instrukciu leave pre interpret aby sa vedel vratit */
            inst_append(&current_func->inst, inst_create(INST_LEAVE, dst, src1, src2));
            break;
        default:
            break;
    }

    /* zrusime typ generovanej instrukcie */
    gen_inst = NONE;

    return true;
}


/**
 * Funkcia pre neterminal <end>
 * @param - ukazatel na token
 * @return - bool
 */
static bool end(Ttoken * token)
{
    DEBUG_MSG("end()\n");
    if (token->id == TKN_SEMICOL)
        return true;
    else
    {
        /*
         * generovanie leave instrukcie, treba aj pri funkcii, bez prikazu
         * return
         */
        union inst_operand dst = {.inst = NULL};
        union inst_operand src1 = {.inst = NULL};
        union inst_operand src2 = {.inst = NULL};
        inst_append(&current_func->inst, inst_create(INST_LEAVE, dst, src1, src2));
        return df_list(token);
    }
}

#if 0
keby nahodu ju este trebalo
/**
 * Funkcia pre neterminal <literal>, kontroluje ci je v tokene literal
 * NOTE: funkcia nebude potrebna, ked sa implementuje rozsirenie LOCALEXPR
 * @param - token
 * @return - bool
 */
static bool literal(Ttoken * token)
{
    DEBUG_MSG("literal()\n");
    union variable_val val;
    enum var_type type;

    switch (token->id)
    {
        case TKN_NUMBER:
        case TKN_STRING:
        case TKN_KW_TRUE:
        case TKN_KW_FALSE:
        case TKN_KW_NIL:
            break;
        default:
            if (token->id == TKN_ID)
                xfree(token->val.str);
            xerror_msg(ERR_SYN_EXP_LITERAL);
            return set_error(RET_ERR_SYN);
            break;
    }
    /* vlozenie premennej do zoznamu premennych */
    set_variable_val(token, &val);
    set_variable_type(token, &type);
    variable_list_append(&current_func->var,
                    variable_create(current_var, type, val));
    current_var = NULL;
    return true;
}
#endif

/**
 * Funkcia pre neterminal <lit-or-id>
 * @param - token
 * @return - pri uspechu true, inak false
 */
static bool lit_or_id(Ttoken * token)
{
    DEBUG_MSG("lit_or_id()\n");
    /* premenne pre tvorbu instrukcie */
    union inst_operand dst = {.tab = current_func};
    union inst_operand src1 = {.inst = NULL};
    union inst_operand src2 = { .inst = NULL};
    union variable_val var_val;
    Tvariable * var = NULL;
    Tvariable * func_arg = NULL; /* ukazatel na predany argument funkcie */

    switch (token->id)
    {
        case TKN_NUMBER:
            if (arg_count < calling_func->param_number)
            {
                /*
                 * pokial je aktualny pocet argumentov nizsi ako pocet
                 * parametrov funkcie, ziskame ukazatel na parameter vo volanej
                 * funkcii
                 */
                dst.var = ial_htable_var_succ(calling_func);
                /* parameter predany hodnotou ulozime do tabulky symbolov */
                var_val.num = token->val.num;
                var = variable_create(NULL, VAR_NUM, var_val);
                variable_list_append(&current_func->var, var);
                src1.var = var;
                /* nakopirujeme premennu z volajucej do volanej funkcie */
                inst_append(&current_func->inst, inst_create(INST_MOV, dst, src1, src2));
            }
            break;

        case TKN_STRING:
            if (arg_count < calling_func->param_number)
            {
                /* pokial je pocet parametrov nizsi ako akutalny pocet zadaneho
                 * argumentu, ziskame ukazatel na parameter vo volanej funkcie */
                dst.var = ial_htable_var_succ(calling_func);
                var_val.str = token->val.str;
                var = variable_create(NULL, VAR_STR, var_val);
                variable_list_append(&current_func->var, var);
                src1.var = var;
                /* nakopirujeme argument do zoznamu premennych volanej funkcie */
                inst_append(&current_func->inst, inst_create(INST_MOV, dst, src1, src2));
            }
            break;

        case TKN_KW_TRUE:
            if (arg_count < calling_func->param_number)
            {
                /* pokial je pocet parametrov nizsi ako akutalny pocet zadaneho
                 * argumentu, ziskame ukazatel na parameter vo volanej funkcie */
                dst.var = ial_htable_var_succ(calling_func);
                var_val.boolean = true;
                var = variable_create(NULL, VAR_BOOL, var_val);
                variable_list_append(&current_func->var, var);
                src1.var = var;
                /* nakopirujeme argument do zoznamu premennych volanej funkcie */
                inst_append(&current_func->inst, inst_create(INST_MOV, dst, src1, src2));
            }
            break;

        case TKN_KW_FALSE:
            if (arg_count < calling_func->param_number)
            {
                /* pokial je pocet parametrov nizsi ako akutalny pocet zadaneho
                 * argumentu, ziskame ukazatel na parameter vo volanej funkcie */
                dst.var = ial_htable_var_succ(calling_func);
                var_val.boolean = false;
                var = variable_create(NULL, VAR_BOOL, var_val);
                variable_list_append(&current_func->var, var);
                src1.var = var;
                /* nakopirujeme argument do zoznamu premennych volanej funkcie */
                inst_append(&current_func->inst, inst_create(INST_MOV, dst, src1, src2));
            }
            break;

        case TKN_ID:
            if (arg_count < calling_func->param_number)
            {
                /*
                 * argumentom je identifikator, treba kontrolovat ci bol
                 * deklarovany
                 */
                func_arg = ial_htable_look_var(current_func, token->val.str);
                xfree(token->val.str);
                if (! func_arg)
                {
                    xerror_msg(ERR_SEM_ID_UNDEFINED);
                    return set_error(RET_ERR_SEM);
                }

                /*
                 * nakopirujeme hodnotu argumentu do zoznamu premennych volanej
                 * funkcie
                 */
                src1.var = func_arg;
                dst.var = ial_htable_var_succ(calling_func);
                inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                            src1, src2));
            }
            else
            {
                /*
                 * parametrov je viac nez mala deklarovana funkcia,
                 * identifikatory zahadzujeme ale kontrolujeme semanticke,
                 * chyby
                 */
                var = ial_htable_look_var(current_func, token->val.str);
                xfree(token->val.str);
                if (! var)
                {

                    xerror_msg(ERR_SEM_ID_UNDEFINED);
                    return set_error(RET_ERR_SEM);
                }
            }
            break;

        case TKN_KW_NIL:
            if (arg_count < calling_func->param_number)
            {
                /* pokial je pocet parametrov nizsi ako akutalny pocet zadaneho
                 * argumentu, ziskame ukazatel na parameter vo volanej funkcie */
                dst.var = ial_htable_var_succ(calling_func);
                var = variable_create(NULL, VAR_NIL, var_val);
                variable_list_append(&current_func->var, var);
                src1.var = var;
                /* nakopirujeme argument do zoznamu premennych volanej funkcie */
                inst_append(&current_func->inst, inst_create(INST_MOV, dst, src1, src2));
                break;
            }
        default:
            xerror_msg(ERR_SYN_UNEXP_TKN);
            return set_error(RET_ERR_SYN);
            break;
    }

    return true;
}

/**
 * Funkcia pre neterminal <arg>
 * @param - token
 * @return - pri uspechu true, inak false
 */
static bool arg(Ttoken * token)
{
    union inst_operand dst = {.inst = NULL};
    union inst_operand src1 = {.inst = NULL};
    union inst_operand src2 = { .inst = NULL};


    switch (token->id)
    {
        case TKN_COMMA:
            /* pravidlo <arg> -> "," "<lit-or-id> <arg> */
            if (! scanner_next_token(token))
                return false;
            if (! lit_or_id(token))
                return false;
            if (! scanner_next_token(token))
                return false;
            /* precital sa dalsi argument, musime zvysit pocet aktualne nacitanych argumentov */
            ++arg_count;
            return arg(token);
            break;

        case TKN_BRACKET_R:
            /* pravidlo <arg> -> ")" */
            /* ak bol zadany pocet argumentov nizsi nez mala deklarovana funkcia
             * zvysne argumenty sa nastavia na nil
             */
            for (; arg_count < calling_func->param_number; arg_count++)
            {
                 dst.var = ial_htable_var_succ(calling_func);
                 inst_append(&current_func->inst, inst_create(INST_CLR, dst, src1, src2));
            }
            /* generovanie instrukcie volania funkcie, skace sa do instrukcneho
             * zoznamu funkcie, ktora sa vola */
            dst.tab = calling_func;
            inst_append(&current_func->inst, inst_create(INST_CALL, dst, src1, src2));
            /* dalsi token ma byt ";", kontroluje sa v inej funkcii */
            if (! scanner_next_token(token))
                return false;
            return true;
            break;

        default:
            xerror_msg(ERR_SYN_UNEXP_TKN);
            return set_error(RET_ERR_SYN);
    }
}

/**
 * Funkcia pre neterminal <args-list>
 * @param - ukazatel na token
 * @return - pri uspechu true, inak fals
 */
static bool args_list(Ttoken * token)
{
    DEBUG_MSG("args_list()\n");
    /* generovanie instrukcie pre zalohu premennych */
    union inst_operand dst = {.tab = current_func};
    union inst_operand src1 = {.inst = NULL};
    union inst_operand src2 = { .inst = NULL};

    inst_append(&current_func->inst, inst_create(INST_ENTER, dst, src1, src2));
    /* aktivita sa nastavuje na 1. parameter volanej funkcie v jej zozname
     * premennych */
    ial_htable_var_first(calling_func);

    /* pravidlo <args-list> -> ")" */
    if (token->id == TKN_BRACKET_R)
    {
        /* ak neboli zadane argumenty, deklarovane parametre sa nastavuju na nil */
        for (int i = 0; i < calling_func->param_number; i++)
        {
             dst.var = ial_htable_var_succ(calling_func);
             inst_append(&current_func->inst, inst_create(INST_CLR, dst, src1, src2));
        }

        /* generuje sa instrukcia volania funkcie, skace sa do instruktion
         * listu, volanej funkcie */
        dst.tab = calling_func;
        inst_append(&current_func->inst, inst_create(INST_CALL, dst, src1, src2));

        /* nacitame token, ktory ma byt ";", ale kontroluje sa inde */
        if (! scanner_next_token(token))
            return false;
        return true;
    }
    else
    {
        /* pravidlo <args-list> -> <lit-or-id> <arg> */
        /* token netreba nacitat */
        arg_count = 0;
        if (! lit_or_id(token))
            return false;
        if (! scanner_next_token(token))
            return false;
        /* spracoval sa 1. argument vo funkcii lit_or_id */
        arg_count = 1;
        return arg(token);
    }
}

/**
 * Funkcia pre neterminal <fun-expr>
 * @param - ukazatel na token
 * @return - pri uspechu trua inak false
 */
static bool fun_expr(Ttoken * token)
{
    DEBUG_MSG("fun_expr()\n");

    /* zistujeme, ci je to identifikator funkcie */
    /* ulozime si ukazatel na volanu funkciu */
    calling_func = ial_htable_look_func(symbol_table, token->val.str);
    if (calling_func)
    {
        xfree(token->val.str);
        /*
         * NOTE: odstranena kontrola rekurzivneho volania main funkcie
         * moze sa volat rekurzivne a vsetky parametre sa zahadzuju
         */
        gen_inst = FUN;
        /* je to funkcia, pravidlo <fun-expr> -> "(" <args-list> */
        if (! scanner_next_token(token))
            return false;
        if (token->id != TKN_BRACKET_L)
        {
            if (token->id == TKN_ID)
                xfree(token->val.str);
            xerror_msg(ERR_SYN_EXP_L_BRACKET);
            return set_error(RET_ERR_SYN);
        }
        if (! scanner_next_token(token))
            return false;
        return args_list(token);
    }
    else
    {
        /* bude sa spracovavat vyraz, predcitame token a zavolame funkciu */
        gen_inst = ASSIGN;
        return expr(NULL, token);
    }
}

/**
 * Funkcia pre neterminal <read-format>
 * @param - ukazatel na tokek
 * @result- bool
 */
static bool read_format(Ttoken * token)
{
    DEBUG_MSG("read_format()\n");
    /*
     * kontrola formatu, ak prisiel spravny token, pokracuje sa dalej, inak
     * nastava syntakticka chyba
     */
    switch (token->id)
    {
        case TKN_STRING:
            break;

        case TKN_NUMBER:
            break;

        default:
            xerror_msg(ERR_SEM_READ_PARAM);
            return set_error(RET_ERR_SEM);
    }
    /* generovanie instrukcie */
    union variable_val val = {.num = 0.42};
    enum var_type type;
    union inst_operand dst = {.var = variable_create(NULL, VAR_NIL, val)};
    set_variable_val(token, &val);
    set_variable_type(token, &type);
    union inst_operand src1 = {.var = variable_create(NULL, type, val)};
    variable_list_append(&current_func->var, dst.var);
    variable_list_append(&current_func->var, src1.var);
    union inst_operand src2 = { .inst = NULL};

    inst_append(&current_func->inst, inst_create(INST_READ, dst, src1, src2));
    src1.var = dst.var;
    dst.var = assign_var;
    inst_append(&current_func->inst, inst_create(INST_MOV, dst, src1, src2));
    assign_var = NULL;
    return true;
}

/**
 * Skontroluje typ argumentu a osetri chybu.
 * @param - ukazatel na token
 * @return - bool
 */
static bool check_arg_type(Ttoken * token, Tvariable ** var)
{
    switch(token->id)
    {
        case TKN_NUMBER:
        case TKN_STRING:
        case TKN_KW_TRUE:
        case TKN_KW_FALSE:
        case TKN_KW_NIL:
            break;
        case TKN_ID:
            *var = ial_htable_look_var(current_func, token->val.str);
            xfree(token->val.str);
            if (! *var)
            {
                xerror_msg(ERR_SEM_ID_UNDEFINED);
                return set_error(RET_ERR_SEM);
            }
            return true;
            break;
        default:
            return set_error(RET_ERR_SYN);
            break;
    }
    /* argument nie je premenna */
    *var = NULL;
    return true;
}

/**
 * Funkcia pre kontrolu vstavanych funkcii a vygenerovanie instrukcii
 * pre vstavane funkcia.
 * @param - ukazatel na aktualny token
 * @return - bool
 */
static bool build_in(Ttoken * token)
{
    DEBUG_MSG("build_in()\n");

    enum token_id tkn_id = token->id; /* ulozenie typu tokenu */
    /* premenne pre generovanie instrukcie */
    union inst_operand dst = {.inst = NULL};
    union inst_operand src1 = {.inst = NULL};
    union inst_operand src2 = {.inst = NULL};
    union variable_val val;
    enum var_type type;
    Tvariable * var;

    /* premenna, do ktorej sa uklada ukazatel na premennu predanu argumentom */
    Tvariable * var_id_arg;

    /* ocakavame token "(" */
    if (! scanner_next_token(token))
        return false;
    if (token->id != TKN_BRACKET_L)
    {
        if (token->id == TKN_ID)
            xfree(token->val.str);
        xerror_msg(ERR_SYN_EXP_L_BRACKET);
        return set_error(RET_ERR_SYN);
    }
    /* nacitanie dalsieho tokenu, ktory sa bude dalej spracovavat */
    if (! scanner_next_token(token))
        return false;
    /*
     * vytvorime premennu bez mena, do ktorej sa bude priradzovat hodnota, ktoru
     * vracia funkcia
     * neskor instrukciou MOV sa premenna prekopiruje do premennej s menom
     */
    dst.var = variable_create(NULL, VAR_NIL, val);
    variable_list_append(&current_func->var, dst.var);
    //dst.var = assign_var;

    if (token->id == TKN_BRACKET_R)
    {
        /*
         * neboli zadane ziadne argumenty, vytvorime premennu nil a
         * vygenerujeme dany typ instrukcie
         */
        var = variable_create(NULL, VAR_NIL, val);
        variable_list_append(&current_func->var, var);
        src1.var = var;
        src2.var = var;
        switch (tkn_id)
        {
            case TKN_BI_TYPE:
                inst_append(&current_func->inst, inst_create(INST_TYPE, dst,
                            src1, src2));
                /* presunutie vysledku do premennej s nazvom */
                src1.var = dst.var;
                dst.var = assign_var;
                inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                            src1, src2));
                break;
            case TKN_BI_SORT:
                inst_append(&current_func->inst, inst_create(INST_SORT, dst,
                            src1, src2));
                /* presunutie vysledku do premennej s nazvom */
                src1.var = dst.var;
                dst.var = assign_var;
                inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                            src1, src2));
                break;
            case TKN_BI_FIND:
                inst_append(&current_func->inst, inst_create(INST_FIND, dst,
                            src1, src2));
                /* presunutie vysledku do premennej s nazvom */
                src1.var = dst.var;
                dst.var = assign_var;
                inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                            src1, src2));
                break;
            case TKN_BI_SUBSTR:
                inst_append(&current_func->inst, inst_create(INST_SUBSTR, dst,
                            src1, src2));
                if (assign_var != NULL)
                {
                    inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                            src1, src2));
                    src1.var = dst.var;
                    dst.var = assign_var;
                    inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                            src1, src2));
                }
                break;
            default:
                break;
        }

        if (! scanner_next_token(token))
            return false;
        return true;
    }

    switch (tkn_id)
    {
        case TKN_BI_TYPE:
        case TKN_BI_SORT:
            /*
             * funkcia ma jeden parameter, ktorym moze byt premenna alebo
             * literal
             */

            /* konrola argumentu */
            if (! check_arg_type(token, &var_id_arg))
                return false;
            /* ak nebola nastavena premenna var_id_arg, argument nebol premenna,
             * musime vytvarat premennu
             */
            if (! var_id_arg)
            {
                set_variable_val(token, &val);
                set_variable_type(token, &type);
                src1.var = variable_create(NULL, type, val);
                variable_list_append(&current_func->var, src1.var);
            }
            else
                src1.var = var_id_arg;

            inst_append(&current_func->inst,
                    inst_create((tkn_id == TKN_BI_TYPE) ? INST_TYPE : INST_SORT,
                       dst, src1, src2));
            /* presunutie vysledku do premennej s nazvom */
            src1.var = dst.var;
            dst.var = assign_var;
            inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                        src1, src2));
            break;
        case TKN_BI_FIND:
            /* funkcia ma dva parametre */
            /* konrola argumentu */
            if (! check_arg_type(token, &var_id_arg))
                return false;
            /* ak nebola nastavena premenna var_id_arg, argument nebol premenna,
             * musime vytvarat premennu
             */
            if (! var_id_arg)
            {
                set_variable_val(token, &val);
                set_variable_type(token, &type);
                src1.var = variable_create(NULL, type, val);
                variable_list_append(&current_func->var, src1.var);
            }
            else
                src1.var = var_id_arg;

            /* nacitame dalsi token, ktory ma byt "," */
            if (! scanner_next_token(token))
                return false;
            if (token->id == TKN_BRACKET_R)
            {
                var = variable_create(NULL, VAR_NIL, val);
                variable_list_append(&current_func->var, var);
                src2.var = var;
                dst.var = assign_var;
                inst_append(&current_func->inst, inst_create(INST_FIND, dst,
                            src1, src2));
                /* presunutie vysledku do premennej s nazvom */
                src1.var = dst.var;
                dst.var = assign_var;
                inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                            src1, src2));
                if (! scanner_next_token(token))
                    return false;
                return true;
            }
            if (token->id != TKN_COMMA)
            {
                if (token->id == TKN_ID)
                    xfree(token->val.str);
                xerror_msg(ERR_SYN_EXP_COMMA);
                return set_error(RET_ERR_SYN);
            }
            /* nacitame dalsi token, ktory ma byt argument a skontrolujeme ho */
            if (! scanner_next_token(token))
                return false;
            if (! check_arg_type(token, &var_id_arg))
                return false;
            /* ak nebola nastavena premenna var_id_arg, argument nebol premenna,
             * musime vytvarat premennu
             */
            if (! var_id_arg)
            {
                set_variable_val(token, &val);
                set_variable_type(token, &type);
                src2.var = variable_create(NULL, type, val);
                variable_list_append(&current_func->var, src2.var);
            }
            else
                src2.var = var_id_arg;

            /* generujeme instrukciu INST_FIND */
            dst.var = assign_var;
            inst_append(&current_func->inst, inst_create(INST_FIND, dst, src1,
                        src2));
            /* presunutie vysledku do premennej s nazvom */
            src1.var = dst.var;
            dst.var = assign_var;
            inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                        src1, src2));
            break;
        case TKN_BI_SUBSTR:
            /* funkcia ma tri parametre */
            /* konrola argumentu */
            if (! check_arg_type(token, &var_id_arg))
                return false;
            /* ak nebola nastavena premenna var_id_arg, argument nebol premenna,
             * musime vytvarat premennu
             */
            if (! var_id_arg)
            {
                set_variable_val(token, &val);
                set_variable_type(token, &type);
                dst.var = variable_create(NULL, type, val);
                variable_list_append(&current_func->var, dst.var);
            }
            else
                dst.var = var_id_arg;

            /* nacitame dalsi token, ktory ma byt "," */
            if (! scanner_next_token(token))
                return false;
            if (token->id == TKN_BRACKET_R)
            {
                /*
                 * argumentov je menej ako ma byt, parametre nastavujeme na nil
                 */
                var = variable_create(NULL, VAR_NIL, val);
                variable_list_append(&current_func->var, var);
                src1.var = var;
                src2.var = var;
                inst_append(&current_func->inst, inst_create(INST_SUBSTR, dst,
                            src1, src2));
                if (assign_var != NULL)
                {
                    /* presunutie vysledku do premennej s nazvom */
                    src1.var = dst.var;
                    dst.var = assign_var;
                    inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                                src1, src2));
                }
                if (! scanner_next_token(token))
                    return false;
                return true;
            }
            if (token->id != TKN_COMMA)
            {
                if (token->id == TKN_ID)
                    xfree(token->val.str);
                xerror_msg(ERR_SYN_EXP_COMMA);
                return set_error(RET_ERR_SYN);
            }
            /* nacitame dalsi token, ktory ma byt argument a skontrolujeme ho */
            if (! scanner_next_token(token))
                return false;
            if (! check_arg_type(token, &var_id_arg))
                return false;
            /* ak nebola nastavena premenna var_id_arg, argument nebol premenna,
             * musime vytvarat premennu
             */
            if (! var_id_arg)
            {
                set_variable_val(token, &val);
                set_variable_type(token, &type);
                src1.var = variable_create(NULL, type, val);
                variable_list_append(&current_func->var, src1.var);
            }
            else
                src1.var = var_id_arg;


             /* nacitame dalsi token, ktory ma byt "," */
            if (! scanner_next_token(token))
                return false;
            if (token->id == TKN_BRACKET_R)
            {
                /*
                 * argumentov je menej ako ma byt, parametre nastavujeme na nil
                 */
                var = variable_create(NULL, VAR_NIL, val);
                variable_list_append(&current_func->var, var);
                src2.var = var;
                inst_append(&current_func->inst, inst_create(INST_SUBSTR, dst,
                            src1, src2));
                if (assign_var != NULL)
                {
                    /* presunutie vysledku do premennej s nazvom */
                    src1.var = dst.var;
                    dst.var = assign_var;
                    inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                                src1, src2));
                }
                if (! scanner_next_token(token))
                    return false;
                return true;
            }
            if (token->id != TKN_COMMA)
            {
                if (token->id == TKN_ID)
                    xfree(token->val.str);
                xerror_msg(ERR_SYN_EXP_COMMA);
                return set_error(RET_ERR_SYN);
            }
            /* nacitame dalsi token, ktory ma byt argument a skontrolujeme ho */
            if (! scanner_next_token(token))
                return false;
            if (! check_arg_type(token, &var_id_arg))
                return false;
            /* ak nebola nastavena premenna var_id_arg, argument nebol premenna,
             * musime vytvarat premennu
             */
            if (! var_id_arg)
            {
                set_variable_val(token, &val);
                set_variable_type(token, &type);
                src2.var = variable_create(NULL, type, val);
                variable_list_append(&current_func->var, src2.var);
            }
            else
                src2.var = var_id_arg;

            /* generujeme instrukciu INST_SUBSTR */
            inst_append(&current_func->inst, inst_create(INST_SUBSTR, dst, src1,
                        src2));
            /* vysledok je v dst, ktory ukladame do src1.var */
            if (assign_var != NULL)
            {
                /* presunutie vysledku do premennej s nazvom */
                src1.var = dst.var;
                dst.var = assign_var;
                inst_append(&current_func->inst, inst_create(INST_MOV, dst,
                            src1, src2));
            }
            break;
        default:
            break;
    }

    /*
     * preskocime argumenty az po ")", ak bola predana hodnota premennou,
     * kontrolujeme, ci bola deklarovana
     */
    if (! scanner_next_token(token))
        return false;
    while (token->id != TKN_BRACKET_R)
    {
        if (token->id != TKN_COMMA)
        {
            if (token->id == TKN_ID)
                xfree(token->val.str);
            xerror_msg(ERR_SYN_EXP_COMMA);
            return set_error(RET_ERR_SYN);
        }
        if (! scanner_next_token(token))
            return false;
        if (! check_arg_type(token, &var_id_arg))
            return false;
        if (! scanner_next_token(token))
            return false;
    }

    assign_var = NULL;
    /* dalsi token by mal byt ";", kontroluje sa v inej funkcii */
    if (! scanner_next_token(token))
        return false;
    return true;

} /* end of build_in() */


/**
 * Funkcia pre neterminal <read-function-expr>
 * @param - ukazatel na token
 * @return - pri uspechu true inak false
 */
static bool read_fun_expr(Ttoken * token)
{
    DEBUG_MSG("read_fun_expr()\n");
    switch (token->id)
    {
        case TKN_KW_READ:
            /* pravidlo <read-fun-expr> -> "READ" "(" <read-format> ")" */
            if (! scanner_next_token(token))
                return false;
            if (token->id != TKN_BRACKET_L)
            {
                if (token->id == TKN_ID)
                    xfree(token->val.str);
                xerror_msg(ERR_SYN_EXP_L_BRACKET);
                return set_error(RET_ERR_SYN);
            }
            if (! scanner_next_token(token))
                return false;
            if (! read_format(token))
                return false;
            if (! scanner_next_token(token))
                return false;
            if (token->id != TKN_BRACKET_R)
            {
                if (token->id == TKN_ID)
                    xfree(token->val.str);
                xerror_msg(ERR_SYN_EXP_R_BRACKET);
                return set_error(RET_ERR_SYN);
            }
            /* dalsi token ma byt ";", kontrolovat sa bude inde */
            if (! scanner_next_token(token))
                return false;
            /* pravidlo je dodrzane */
            return true;
            break;

        case TKN_ID:
            /* pravidlo <read-fun-expr> -> "ID" <fun-expr> */
            /* nevolame dalsi token lebo sa musi zistit, ci ide o funkciu alebo
             * o vyraz
             */
            return fun_expr(token);
            break;

        case TKN_BI_TYPE:
        case TKN_BI_SORT:
        case TKN_BI_FIND:
        case TKN_BI_SUBSTR:
            return build_in(token);
            break;

        default:
            /* zavola sa funkcia pre vyhodnotenie vyrazu */
            gen_inst = ASSIGN;
            return expr(NULL, token);
    }
}

/**
 * Funkcia pre neterminal <w-arg>
 * @param - ukazatel na token
 * @return - pri uspechu true, inak false
 */
static bool w_arg(Ttoken * token)
{
    switch (token->id)
    {
        case TKN_COMMA:
            /* pravidlo <w-arg> -> , <expr> <w-arg> */
            /* nacitavame token pre precedencnu analyzu */
            if (! scanner_next_token(token))
                return false;
            gen_inst = WRITE;
            if (! expr(NULL, token))
                return false;
            /* token je nacitany z precedencnej analyzy */
            return w_arg(token);
            break;

        case TKN_BRACKET_R:
            /* pravidlo <w-arg> -> ")" */
            /* nacitame token mal by byt ";" ale kontrolovat sa bude inde */
            if (! scanner_next_token(token))
                return false;
            return true;
            break;

        default:
            /* terminal mimo pravidla */
            xerror_msg(ERR_SYN_UNEXP_TKN);
            return set_error(RET_ERR_SYN);
    }
}

/**
 * Funkcia pre neterminal <w-args-list>
 * @param - ukazatel na token
 * @return - pri uspechu true, inak false
 */
static bool w_args_list(Ttoken * token)
{
    DEBUG_MSG("w_args_list()\n");
    /* write musi mat aspon jeden parameter */
    if (token->id == TKN_BRACKET_R)
    {
        xerror_msg(ERR_SEM_WRITE_PARAM);
        return set_error(RET_ERR_SEM);
    }
    else {
        /* pravidlo w_args_list -> <expr> <w-arg> */
        /* token je uz nacitany a nie je to ")" volame precedencnu */
        if (! expr(NULL, token))
            return false;
        /* token je uz nacitany z precedencnej analyzy */
        return w_arg(token);
    }
}

/**
 * Funkcia pre neterminal <stat-list>
 * @param - ukazatel na token
 * @return - bool
 */
static bool stat_list(Ttoken * token)
{
    DEBUG_MSG("stat_list()\n");
    /* pravidlo <stat-list> -> END <end> */
    if (token->id == TKN_KW_END)
    {
        if (! scanner_next_token(token))
                return false;
        return end(token);
    }

    /* pravidlo <stat-list> -> <stat> ; <stat-list> */
    if (! stat(token))
        return false;
    /*
     * if (! scanner_next_token(token) return false;
     * token by uz mal byt nacitany, len sa provna
     */
    if (token->id != TKN_SEMICOL)
    {
        if (token->id == TKN_ID)
            xfree(token->val.str);
        xerror_msg(ERR_SYN_EXP_SEMICOLON);
        return set_error(RET_ERR_SYN);
    }
    if (! scanner_next_token(token))
        return false;
    return stat_list(token);
}

/**
 * Funkcia pre neterminal <stat-repeat-list>
 * funkcia <stat-list> sa nemoze pouzit lebo token UNTIL moze prist jedine
 * az po tokene REPEAT
 * @param - ukazatel na token
 * @return - bool
 */
static bool stat_repeat_list(Ttoken * token)
{
    DEBUG_MSG("stat_repeat_list()\n");
    if (token->id == TKN_RW_UNTIL)
    {
        return true;
    }

    /* pravidlo <stat-list> -> <stat> ; <stat-list> */
    if (! stat(token))
        return false;
    /*
     * if (! scanner_next_token(token) return false;
     * token by uz mal byt nacitany, len sa provna
     */
    if (token->id != TKN_SEMICOL)
    {
        if (token->id == TKN_ID)
            xfree(token->val.str);
        xerror_msg(ERR_SYN_EXP_SEMICOLON);
        return set_error(RET_ERR_SYN);
    }
    if (! scanner_next_token(token))
        return false;
    return stat_repeat_list(token);
}

/**
 * Funkcia pre neterminal <stat-if-list>
 * @param - ukazatel na token
 * @return - bool
 */
static bool stat_if_list(union inst_operand if_end,
        union inst_operand if_block_end, Ttoken * token)
{
    DEBUG_MSG("stat_if_list()\n");
    union inst_operand src1;
    if (token->id == TKN_KW_ELSE)
    {
         /*
          * generujeme instrukcui NOP na uplny koniec bloku, kde sa bude
          * skakat ak bola podmienka predosleho bloku true
          */
        inst_append(&current_func->inst, inst_create(INST_JMP, if_end, if_end, if_end));
        /* vkladame instrukciu NOP, na ktoru sa skace ak bola predosla podmienka
         * false
         */
        inst_append(&current_func->inst, if_block_end.inst);

        if (! scanner_next_token(token))
                return false;
        return stat_list(token);
    }

    if (token->id == TKN_RW_ELSEIF)
    {
        /*
         * generujeme instrukcui NOP na uplny koniec bloku, kde sa bude
         * skakat ak bola podmienka predosleho bloku true
         */
        inst_append(&current_func->inst, inst_create(INST_JMP, if_end,
                    if_end, if_end));
        /* vkladame NOP, na ktory sa ma skocit, ak predosla podmienka bola
         * false
         */
        inst_append(&current_func->inst, if_block_end.inst);

        if (! scanner_next_token(token))
            return false;
        if (! expr(&src1.var, token))
            return false;
        /*
         * vytvorime instrukciu pre skok, predosleho bloku, ak bola podmienka
         * false
         */
        if_block_end.inst = inst_create(INST_NOP, if_end, if_end, if_end);
        inst_append(&current_func->inst, inst_create(INST_JNIF, if_block_end,
                    src1, src1));

        if (token->id != TKN_KW_THEN)
        {
            if (token->id == TKN_ID)
                xfree(token->val.str);
            xerror_msg(ERR_SYN_EXP_THEN);
            return set_error(RET_ERR_SYN);
        }
        if (! scanner_next_token(token))
            return false;
        return stat_if_list(if_end, if_block_end, token);
    }

    /* rozsirenie if <exp> then <stat-if-list> end; */
    if (token->id == TKN_KW_END)
    {
        if (! scanner_next_token(token))
                return false;
        return end(token);
    }

    /* pravidlo <stat-list> -> <stat> ; <stat-list> */
    if (! stat(token))
        return false;

    if (token->id != TKN_SEMICOL)
    {
        if (token->id == TKN_ID)
            xfree(token->val.str);
        xerror_msg(ERR_SYN_EXP_SEMICOLON);
        return set_error(RET_ERR_SYN);
    }

    if (! scanner_next_token(token))
        return false;
    return stat_if_list(if_end, if_block_end, token);
}

#if 0
/**
 * Funkcia pre neterminal <for-expr>
 * @param - token
 * @return - bool
 */
static bool for_expr(Ttoken * token)
{
    DEBUG_MSG("for_expr()\n");
    /* definicia premennych pre tvorbu instrukcii */
    union inst_operand dst;
    union inst_operand src1;
    union inst_operand src2;
    Tvariable * init_var;

    if (token->id != TKN_ID)
    {
        xerror_msg(ERR_SYN_EXP_ID);
        return set_error(RET_ERR_SYN);
    }

    /* kontrolujeme, ci to nie je identifikator funkcie */
    if (ial_htable_look_func(symbol_table, token->val.str))
    {
        xerror_msg(ERR_SYN_EXP_ID_VAR);
        xfree(token->val.str);
        return set_error(RET_ERR_SYN);
    }

    if ((init_var = ial_htable_look_var(current_func,
                    token->val.str)) == NULL)
    {
        xerror_msg(ERR_SEM_ID_UNDEFINED);
        xfree(token->val.str);
        return set_error(RET_ERR_SEM);
    }

    if (! scanner_next_token(token))
        return false;
    if (token->id != TKN_OP_ASSIGNMENT)
    {
        xerror_msg(ERR_SYN_EXP_ASSIGNMENT);
        if (token->id == TKN_ID)
            xfree(token->val.str);
        return set_error(RET_ERR_SYN);
    }

    if (! scanner_next_token(token))
        return false;
    if (! expr(&src1.var, token))
        return false;

    /* generujeme instrukciu priradenia do inicializacnej premennej */
    dst.var = init_var;
    inst_append(&current_func->inst, inst_create(INST_MOV, dst, src1, src2));

    if (! scanner_next_token(token))
        return false;
    if (token->id != TKN_COMMA)
    {
        xerror_msg(ERR_SYN_EXP_COMMA);
        if (token->id == TKN_ID)
            xfree(token->val.str);
        return set_error(RET_ERR_SYN);
    }

    inst_last(current_func->inst);
    start_for = inst_create(NOP, NULL, NULL, NULL);
    inst_append(current_func->inst, start_for);

    /* nacitame vyraz pre podmienku vykonania vyrazu */
    if (! scanner_next_token(token))
        return false;
    if (! expr(&src1.var, token))
        return false;
}
#endif

/**
 * Funkcia pre neterminal <stat>
 * @param - ukazatel na token
 * @return - pri uspechu true inak false
 */
static bool stat(Ttoken * token)
{
    DEBUG_MSG("stat()\n");
    /* premenna pre navratovu hodnotu z funkcie neterminalu */
    bool rv;
    /* premenne pre generovanie instrukcie */
    union inst_operand dst = {.var = assign_var};
    union inst_operand src1 = {.var = &calling_func->ret_value};
    union inst_operand src2 = { .inst = NULL};
    union inst_operand start_loop;
    union inst_operand if_end;
    union inst_operand if_block_end;

    switch (token->id)
    {
        case TKN_ID:
            /* pravidlo <stat> -> "ID" "=" <read-fun-expr> */

            if ((calling_func = ial_htable_look_func(symbol_table,
                            token->val.str)))
            {
                xfree(token->val.str);
                /*
                 * NOTE: odstranena kontrola rekurzivneho volania main funkcie
                 * moze sa volat rekurzivne a vsetky parametre sa zahadzuju
                 */
                if (! scanner_next_token(token))
                    return false;
                if (token->id != TKN_BRACKET_L)
                {
                    if (token->id == TKN_ID)
                        xfree(token->val.str);
                    xerror_msg(ERR_SYN_EXP_L_BRACKET);
                    return set_error(RET_ERR_SYN);
                }
                if (! scanner_next_token(token))
                    return false;
                return args_list(token);
            }
            else
            {
                if ((assign_var = ial_htable_look_var(current_func,
                        token->val.str)) == NULL)
                {
                    /* semanticka kontrola, ci bola premenna deklarovana */
                    xerror_msg(ERR_SEM_ID_UNDEFINED);
                    xfree(token->val.str);
                    return set_error(RET_ERR_SEM);
                }
                xfree(token->val.str);

                if (! scanner_next_token(token))
                    return false;
                if (token->id != TKN_OP_ASSIGNMENT)
                {
                    xerror_msg(ERR_SYN_EXP_ASSIGNMENT);
                    if (token->id == TKN_ID)
                        xfree(token->val.str);
                    return set_error(RET_ERR_SYN);
                }
                if (! scanner_next_token(token))
                    return false;

                /*
                 * ak je navratova hodnota true a generovala sa instrukcia volania
                 * funkcie, tak vygenerujeme instrukcie priradenia navratovej hodnoty
                 * funkcie do premennej */
                if ((rv = read_fun_expr(token)) && gen_inst == FUN)
                {
                    /* funkcia sa syntakticky skontrolovala a je vydgenerovany
                     * trojadresny kod. bude sa generovat instrukcia pre priradenie
                     * navratovej hodnoty funkcie
                     */
                    dst.var = assign_var;
                    src1.var = &calling_func->ret_value;
                    src2.inst = NULL;

                    inst_append(&current_func->inst, inst_create(INST_MOV, dst, src1, src2));
                    gen_inst = NONE;
                    return true;
                }
                else
                    return rv; /* vraciame vysledok funkcie neterminalu */
            }

            break;

        case TKN_BI_FIND:
        case TKN_BI_SORT:
        case TKN_BI_SUBSTR:
        case TKN_BI_TYPE:
            xwarning_msg(WAR_IGNORE_RESULT);
            return build_in(token);
            break;

        case TKN_KW_WRITE:
            /* pravidlo <stat> -> "WRITE" "(" <w-args-list> */
            /* poznacime si ze generovana instrukcia bude write */
            gen_inst = WRITE;
            if (! scanner_next_token(token))
                return false;
            if (token->id != TKN_BRACKET_L)
            {
                if (token->id == TKN_ID)
                    xfree(token->val.str);
                xerror_msg(ERR_SYN_EXP_L_BRACKET);
                return set_error(RET_ERR_SYN);
            }
            if (! scanner_next_token(token))
                return false;
            return w_args_list(token);
            break;

        case TKN_KW_RETURN:
            /* pravidlo <stat> -> "RETURN" <expr> */
            if (! scanner_next_token(token))
                return false;
            gen_inst = RETURN;
            return expr(NULL, token);
            break;

        case TKN_RW_REPEAT:
            start_loop.inst = inst_create(INST_NOP, dst, src1, src2);
            inst_append(&current_func->inst, start_loop.inst);
            if (! scanner_next_token(token))
                return false;
            if (! stat_repeat_list(token))
                return false;
            /* prisiel token UNTIL, ideme kontrolovat podmienku */
            if (! scanner_next_token(token))
                return false;
            if (! expr(&src1.var, token))
                return false;
            inst_append(&current_func->inst, inst_create(INST_JNIF, start_loop, src1, src1));
            return true;
            break;

        case TKN_KW_WHILE:
            /* pravidlo <stat> -> "WHILE" <expr> "DO" <stat-list> */
            if (! scanner_next_token(token))
                return false;
            /* generovanie prazdnej instrukcie, pre potrebu skoku
             * pred vyhodnocovanie vyrazu pre podmienku.
             * na operandoch INST_NOP nezalezi
             */
            start_loop.inst = inst_create(INST_NOP, dst, src1, src2);
            inst_append(&current_func->inst, start_loop.inst);

            if (! expr(&src1.var, token))
                return false;
            /* generovanie prazdnej instrukcie pre skok na koniec cyklu.
             * vlozi sa az na konci tela cyklu
             * na INST_NOP operandoch nezalezi
             */
            dst.inst = inst_create(INST_NOP, dst, src1, src2);
            /* generovanie INST_JNIF, skace ak je podmienka nepravdiva */
            src2.inst = inst_create(INST_JNIF, dst, src1, src2);
            inst_append(&current_func->inst, src2.inst);

            if (token->id != TKN_KW_DO)
            {
                if (token->id == TKN_ID)
                    xfree(token->val.str);
                xerror_msg(ERR_SYN_EXP_DO);
                return set_error(RET_ERR_SYN);
            }
            if (! scanner_next_token(token))
                return false;
            if ( (rv = stat_list(token)) )
            {
                /* vkladame instrukciu skoku na instrukciu pred vyhodnocovanim
                 * podmienky
                 */
                inst_append(&current_func->inst, inst_create(INST_JMP,
                            start_loop, src1, src1));
                /* vkladame instrukciu na ktoru sa ma skocit ak bude podmienka
                 * nepravdiva
                 */
                inst_append(&current_func->inst, dst.inst);
            }
            return rv;
            break;

        case TKN_KW_IF:
            /* pravidlo <stat> -> "IF" <expr> THEN <stat-if-list> */
            if (! scanner_next_token(token))
                return false;
            /* do src1 si ulozime vysledok podmienky pre if */
            if (! expr(&src1.var, token))
                return false;
            /* instrukcia NOP pre skoky na koniec bloku IFTHEN a bloku
             * ELSE alebo ELSEIF bloku, na parametroch INST_NOP nezalezi
             */
            if_end.inst = inst_create(INST_NOP, dst, src1, src2);
            if_block_end.inst = inst_create(INST_NOP, dst, src1, src2);

            inst_append(&current_func->inst, inst_create(INST_JNIF,
                        if_block_end, src1, src2));

            if (token->id != TKN_KW_THEN)
            {
                if (token->id == TKN_ID)
                    xfree(token->val.str);
                xerror_msg(ERR_SYN_EXP_THEN);
                return set_error(RET_ERR_SYN);
            }
            if (! scanner_next_token(token))
                return false;

            if ((rv = stat_if_list(if_end, if_block_end, token)))
            {
                /*
                 * Pokial ide o vetvenie bez else vetvy, je nutne pridat
                 * block_end na ktory sa z podmienky odkazuje.
                 */
                if (! if_block_end.inst->prev)
                    inst_append(&current_func->inst, if_block_end.inst);
                /* vlozime instrukciu nop pre skok na koniec bloku */
                inst_append(&current_func->inst, if_end.inst);
            }
            return rv;
            break;
        /*
        case TKN_RW_FOR:
            if (! scanner_next_token(token))
                return false;
            return for_expr(token);
            break; */

        default:
            /* na vstupe je terminal mimo pravidla */
            xerror_msg(ERR_SYN_UNEXP_TKN);
            return set_error(RET_ERR_SYN);
    }
}

/**
 * Funkcia pre neterminal <param>
 * @param - ukazatel na token
 * @return - bool
 */
static bool param(Ttoken * token)
{
    DEBUG_MSG("param()\n");
    switch (token->id)
    {
        case TKN_BRACKET_R:
            /* pravidlo <param> -> ")" */
            return true;
            break;
        case TKN_COMMA:
            /* pravidlo <param> -> "," "ID" <param> */
            if (! scanner_next_token(token))
                return false;

            /* dalsi token musi byt "ID" */
            if (token->id != TKN_ID)
            {
                xerror_msg(ERR_SYN_EXP_ID);
                return set_error(RET_ERR_SYN);
            }

            /* semanticka kontrola kolizie s nazvom funkcie alebo premenej */
            if (ial_htable_look_func(symbol_table, token->val.str)
                    || ial_htable_look_var(current_func, token->val.str))
            {
                xerror_msg(ERR_SEM_ID_COLLISION);
                return set_error(RET_ERR_SEM);
            }

            /* vkladame parameter do zoznamu premennych a inkrementujeme pocet
             * parametrov */
            union variable_val tmp_val; /* make some shit */
            tmp_val.num = 0.42;

            variable_list_append(&current_func->var,
                    variable_create(token->val.str, VAR_NIL, tmp_val));

            ial_htable_inc_param_count(current_func);

            /* dalsi token a rekurzivne volanie param() */
            if (! scanner_next_token(token))
                return false;
            return param(token);
        default:
            if (token->id == TKN_ID)
                xfree(token->val.str);
            xerror_msg(ERR_SYN_UNEXP_TKN);
            return set_error(RET_ERR_SYN);
    }
}

/**
 * Funkcia pre neterminal <dclr-type>
 * @param - ukazatel na token
 * @return - bool
 */
static bool dclr_type(Ttoken * token)
{
    DEBUG_MSG("dclr_type()\n");

    union inst_operand dst;
    union inst_operand src1;
    union inst_operand src2;

    union variable_val tmp_val = {.num = 0.42};
    /* premenna pre ulozenie hodnoty vratenej z pparser */
    Tvariable * var;
    /* aby nepisalo warning unused variable */
    var = NULL;

    switch (token->id)
    {
        case TKN_SEMICOL:
            /* pravidlo <dclr-type> -> ";" */
            /* premennej sa nepriradzuje hodnota, vlozime do variable listu
               implicitne nastavenie hodnoty na NIL */
            tmp_val.num = 0.42;
            variable_list_append(&current_func->var,
                    variable_create(current_var, VAR_NIL, tmp_val));
            current_var = NULL;
            return true;
            break;
        case TKN_OP_ASSIGNMENT:
            /* pravidlo <dclr-type> -> "=" <literal> ";" */
            if (! scanner_next_token(token))
                return false;

            /*
             * rozirenie LOCALEXPR - pri deklaracii premennej je mozne priradit
             * do premennej vyraz
             */

            /* NOTE: toto tu bolo pred opravou bugu local a = a; */
#if 0
            /* vytvorime a vlozime premennu do var listu, pre INST_MOV */
             assign_var = variable_create(current_var, VAR_NIL, tmp_val);
             variable_list_append(&current_func->var, assign_var);
             gen_inst = ASSIGN;
             if (! expr(NULL, token))
                 return false;
#endif

#if 0
            if (! expr(&var, token))
                return false;
            /* v pparser sa vytvorila pomocna premenna v tabulke symbolov, ktoru
             * mozeme premenovat a tym definujeme premennu
             */
            var->name = current_var;
            current_var = NULL;
#endif
#if 1
            /* LOCALEXPR po 3 */
            if (! expr(&var, token))
                return false;
            /* teraz mozeme vytvorit premennu a vlozit do tabulky symbolov */
            assign_var = variable_create(current_var, VAR_NIL, tmp_val);
            variable_list_append(&current_func->var, assign_var);
            /* generujeme instrukciu MOV */
            dst.var = assign_var; /* premenna kam sa ma priradzovat */
            assign_var = NULL;
            src1.var = var; /* ukazatel na premennu do tabulky symbolov */
            src2.inst = NULL;
            /* generujeme instrukciu priradenia do premennej */
            inst_append(&current_func->inst,
                    inst_create(INST_MOV, dst, src1, src2));
#endif


            /* deklaracia bez rozsirenia LOCALEXPR
            if (! literal(token))
                return false;

            if (! scanner_next_token(token))
                return false; */

            if (token->id != TKN_SEMICOL)
            {
                if (token->id == TKN_ID)
                    xfree(token->val.str);
                xerror_msg(ERR_SYN_EXP_SEMICOLON);
                return set_error(RET_ERR_SYN);
            }
            return true;
            break;
        default:
            /* zabrani memory leaku */
            if (token->id == TKN_ID)
                xfree(token->val.str);
            xerror_msg(ERR_SYN_UNEXP_TKN);
            return set_error(RET_ERR_SYN);
    }
}

/**
 * Funkcia pre neterminal <var-list>
 * @param - ukazatel na token
 * @return - bool
 */
static bool var_list(Ttoken * token)
{
    DEBUG_MSG("var_list()\n");
    if (token->id == TKN_KW_LOCAL)
    {
        /* pravidlo <var-list> "LOCAL" "ID" <dclr-type> <var-list> */

        /* dalsi token musi byt "ID" */
        if (! scanner_next_token(token))
            return false;
        if (token->id != TKN_ID)
        {
            xerror_msg(ERR_SYN_EXP_ID);
            return set_error(RET_ERR_SYN);
        }

        /* semanticka kontrola kolizie s nazvom funkcie alebo premenej */
        if (ial_htable_look_func(symbol_table, token->val.str)
                || ial_htable_look_var(current_func, token->val.str))
        {
            xfree(token->val.str);
            xerror_msg(ERR_SEM_ID_COLLISION);
            return set_error(RET_ERR_SEM);
        }

        /* ulozime si nazov pre pripad, ze by premennej trebalo priradovat
           hodnotu */
        current_var = token->val.str;

        /* volame dalsi token a funkciu dclr-type() */
        if (! scanner_next_token(token))
            return false;
        if (! dclr_type(token))
            return false;

        if (! scanner_next_token(token))
            return false;
        return var_list(token);
    }
    else
        /* pravidlo <var-list> -> <stat-list> */
        return stat_list(token);
}

/**
 * Funkcia pre neterminal <param-list>
 * @param - ukazatel na token
 * @return - bool
 */
static bool param_list(Ttoken * token)
{
    DEBUG_MSG("param_list()\n");
    switch (token->id)
    {
        case TKN_BRACKET_R:
            /* pravidlo <param-list> -> ")" */
            return true;
            break;

        case TKN_ID:
            /* pravidlo <param-list> -> "ID" <param> */
            /* funkcia main by nemala mat parametre */
            if (current_main(current_func))
            {
                xfree(token->val.str);
                xerror_msg(ERR_SEM_MAIN_PARAM);
                return set_error(RET_ERR_SYN);
            }

            /* semanticka kontrola kolizie s nazvom funkcie */
            if (ial_htable_look_func(symbol_table, token->val.str))
            {
                xfree(token->val.str);
                xerror_msg(ERR_SEM_ID_COLLISION);
                return set_error(RET_ERR_SEM);
            }

            /* vkladame parameter do zoznamu premennych a inkrementujeme pocet
             * parametrov */
            union variable_val tmp_val; /* make some shit */
            tmp_val.num = 0.42;

            variable_list_append(&current_func->var,
                    variable_create(token->val.str, VAR_NIL, tmp_val));

            ial_htable_inc_param_count(current_func);

            /* volame dalsi token a funkciu param() */
            if (! scanner_next_token(token))
                return false;
            return param(token);
            break;
        default:
            xerror_msg(ERR_SYN_UNEXP_TKN);
            return set_error(RET_ERR_SYN);
    }
}

/**
 * Funkcia pre neterminal <df-list>
 * @param - ukazatel na token
 * @return - bool
 */
static bool df_list(Ttoken * token)
{
    DEBUG_MSG("df_list()\n");
    if (token->id == TKN_KW_FUNCTION)
    {
        /* pravidlo <df-list> -> "FUNCTION" "ID" "(" <param-list> <var-list> */
        /* poziadame o dalsi token, ktory musi byt TKN_ID */
        if (! scanner_next_token(token))
            return false;
        if (token->id != TKN_ID)
        {
            xerror_msg(ERR_SYN_EXP_ID);
            return set_error(RET_ERR_SYN);
        }

        /* main musi byt posledna */
        if (ial_htable_look_func(symbol_table, "main"))
        {
            xerror_msg(ERR_SYN_MAIN_LAST);
            return set_error(RET_ERR_SYN);
        }

        /*
         * semanticka kontrola, ci sa nenachadza identifikator s rovnakym nazvom
         * funkcie v tabulke symbolov
         */
        if (ial_htable_look_func(symbol_table, token->val.str) != NULL)
        {
            xfree(token->val.str);
            xerror_msg(ERR_SEM_FUN_REDECLARATION);
            return set_error(RET_ERR_SEM);
        }

        /* vlozenie funkcie do tabulky symbolov a ulozenie ukazatela aktualnej
         * funkcie pre dalsie vkladanie parametrou a premennych
         */
        current_func = ial_htable_insert(symbol_table, token->val.str);

        /* poziadame o dalsi token, ktory musi byt TKN_BRACKET_L */
        if (! scanner_next_token(token))
            return false;
        if (token->id != TKN_BRACKET_L)
        {
            if (token->id == TKN_ID)
                xfree(token->val.str);
            xerror_msg(ERR_SYN_EXP_L_BRACKET);
            return set_error(RET_ERR_SYN);
        }

        /* zavolame dalsi token a funkciu param_list */
        if (! scanner_next_token(token))
            return false;
        if (! param_list(token))
            return false;
        /* dalsi token a funkcia var_list() */
        if (! scanner_next_token(token))
            return false;
        if (! var_list(token))
            return false;

        return true;
    }
    else
    {
        /* token nie je TKN_KW_FUNCTION, syntakticka chyba */
        if (token->id == TKN_ID)
            xfree(token->val.str);
        xerror_msg(ERR_SYN_EXP_FUN);
        return set_error(RET_ERR_SYN);
    }
}

/**
 * Funkcia pre pociatocny neterminal <prog>
 * @param - ukazatel na token
 * @result - bool
 */
static bool program(Ttoken * token)
{
    DEBUG_MSG("program()\n");
    /* pravidlo <prog> -> <df-list> <eof> */
    if (! df_list(token))
        return false;
    /* nacitame dasli token, ktory musi byt 'EOF' */
    if (! scanner_next_token(token))
        return false;
    /* pravidlo <eof> -> TKN_EOF */
    if (token->id != TKN_EOF)
    {
        xerror_msg(ERR_SYN_EXP_EOF);
        return set_error(RET_ERR_SYN);
    }

    return true;
}

/**
 * Funkcia sa stara o syntaxou riadeny preklad.
 * Vola lexikalny analyzator, syntakticky analyzator zhora dolu
 * implementovany rekurzivnym zostupom, ktory vola syntakticky analyzator
 * zdola hore implementovany precedencnou analyzou, semanticky analyzator
 * a interpret.
 * @param - none
 * @result - bool
 */
bool syntax_directed_translation()
{
    DEBUG_MSG("ZACIATOK SYNTAXOU RIADENEHO PREKLADU\n");
    /* definicia potrebnych premennych */
    Ttoken token;
    bool result = true;
    Thtable_item * main_item = NULL; /* ukazatel na polozku s mainom */

    /* alokacia a inicializacia tabulky symbolov */
    symbol_table = ial_htable_init(HASH_SIZE);

    if (! scanner_next_token(&token))
        result = false;
    else
        /* volanie funkcie, ktora reprezentuje 1. neterminal */
        result = program(&token);

    /* kontrola definicie main */
    main_item = ial_htable_look_func(symbol_table, "main");
    if (! get_error() && ! main_item)
    {
        xerror_msg(ERR_SYN_NO_MAIN);
        ial_htable_delete(symbol_table);
        return set_error(RET_ERR_SYN);
    }

    if (result)
    {
        DEBUG_MSG("ZAVOLA SA INTERPRET\n");
        interpret_run(ial_htable_inst_first(main_item));
    }

    /* zrusenie tabulky symbolov */
    ial_htable_delete(symbol_table);

    DEBUG_MSG("KONIEC SYNTAXOU RIADENEHO PREKLADU\n");

    return result;
}

/* end of parser.c */
