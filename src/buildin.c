/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       buildin.h
 * Autor(i):    Fridolin
 *
 * Implementacia vstavanych funkcii interpretra.
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "buildin.h"
#include "debug.h"
#include "xwrappers.h"
#include "str.h"
#include "variable.h"
#include "ifj11.h"
#include "ial.h"

#define TYPE_MAX_LEN        9

/*
 * Ak si chces uzit plno zabavy, odkomentuj nasledovny riadok.
 */
//#define if(               if(!

/*
 * Pri operaciach je nutne dbat na memoryleaky - v pripade ze je do cieloveho
 * operandu priradovana hodnota a cielovy operand bol typu VAR_STR, hodnota musi
 * byt korektne dealokovana.
 * @param - premenna ktora bude skontrolovana
 * @return - void
 */
static inline void buildin_freestr(Tvariable * var)
{
    if (var->type == VAR_STR)
        xfree(var->val.str);
}

/**
 * Implementacia vstavanej funkcie type().
 * @param - premenna, do ktorej sa ma vysledok operacie ulozit
 * @param - premenna, ktorej typ sa ma vyhodnotit
 * @return - vzdy true
 */
bool buildin_type(Tvariable * dst, const Tvariable * item)
{
    if (! dst || ! item)
    {
        DEBUG_MSG("find predany NULL");
        return true;
    }

    assert(item);

    static const char * TYPE_STR  = "string";
    static const char * TYPE_NUM  = "number";
    static const char * TYPE_BOOL = "boolean";
    static const char * TYPE_NIL  = "nil";

    buildin_freestr(dst);

    dst->val.str = xmalloc(TYPE_MAX_LEN * sizeof(char));

    switch (item->type)
    {
        case VAR_NUM:
            strcpy(dst->val.str, TYPE_NUM);
            break;
        case VAR_STR:
            strcpy(dst->val.str, TYPE_STR);
            break;
        case VAR_BOOL:
            strcpy(dst->val.str, TYPE_BOOL);
            break;
        case VAR_NIL:
            strcpy(dst->val.str, TYPE_NIL);
            break;
        default:
            assert(0);
            break;
    }

    /*
     * Typ vysledku je priradeny az po operacii, aby bolo mozne volat instrukciu
     * type s rovnakym operandom pre priradenie a typ vysledku. (dst == item)
     */
    dst->type = VAR_STR;

    return true;
}

/**
 * Konkatenacia dvoch retazcov.
 * @param - miesto, kam sa ma vysledok ulozit
 * @param - prvy retazec
 * @param - druhy retazec
 * @return - false v pripade, ze nad danymi typmi nie je mozne previest
 *           konkatenaciu
 */
bool buildin_conc(Tvariable * dst,
                  const Tvariable * var_str1,
                  const Tvariable * var_str2)
{
    if (! dst || ! var_str1 || ! var_str2)
    {
        DEBUG_MSG("find predany NULL");
        return true;
    }

    size_t len1, len2;

    if (var_str1->type != VAR_STR
        || var_str2->type != VAR_STR)
    {
        xerror_msg(ERR_INT_BAD_CONC);
        return false;
    }

    len1 = strlen(var_str1->val.str);
    len2 = strlen(var_str2->val.str);

    dst->type = VAR_STR;
    dst->val.str = xmalloc(len1 + len2 + 1);

    strcpy(dst->val.str, var_str1->val.str);
    strcpy(dst->val.str + len1, var_str2->val.str);

    return true;
}

/**
 * Vstavana funkcia read().
 * @param - miesto pre vysledok operacie.
 * @param - parameter funkcie read()
 * @return - v pripade, ze sa nenacil dany pocet znakov false
 */
bool buildin_read(Tvariable * dst, const Tvariable * item)
{
    assert(dst);
    assert(item);

    static const char * READ_NUM     = "*n";
    static const char * READ_STR_LF  = "*l";
    static const char * READ_STR_EOF = "*a";

    Tstring str;
    int c;

    buildin_freestr(dst);
    if (item->type == VAR_NUM)
    {
        if (item->val.num < 0)
        {
            xerror_msg(ERR_INT_READ_OP);
            return set_error(RET_ERR_INT);
        }

        str_init(&str);
        for (unsigned i = 0; i < (unsigned) item->val.num; ++i)
        {
            c = getchar();
            if (c != EOF)
                str_add_char(&str, c);
        }

        dst->type = VAR_STR;
        dst->val.str = str_get_str(&str);
    }
    else if (item->type == VAR_STR)
    {
        if (! strncmp(item->val.str, READ_NUM, 2))
        {
            int i = scanf("%lf", &dst->val.num);
            if (i != 1 || isnan(dst->val.num) || isinf(dst->val.num))
            {
                dst->type = VAR_NIL;
            }
            else
                dst->type = VAR_NUM;
        }
        else if (! strncmp(item->val.str, READ_STR_LF, 2))
        {
            str_init(&str);
            while ((c = getchar()) != '\n' && c != EOF)
                str_add_char(&str, c);
            if (str.length == 0)
            {
                dst->type = VAR_NIL;
            }
            else
            {
                dst->type = VAR_STR;
                dst->val.str = str.str;
            }
        }
        else if (! strncmp(item->val.str, READ_STR_EOF, 2))
        {
            str_init(&str);
            while ((c = getchar()) != EOF)
                str_add_char(&str, c);
            dst->type = VAR_STR;
            dst->val.str = str.str;
        }
        else
        {
            /*
             * Chybny parameter read je semanticka chyba
             */
            xerror_msg(ERR_INT_READ_OP);
            return set_error(RET_ERR_INT);
        }
    }
    else
    {
        /*
         * V pripade inych parametrov je vrateny nil.
         */
        dst->type = VAR_NIL;
    }

    return true;
}

/**
 * Vstavana funkcia write().
 * @param - hodnotu pre vypis
 * @return - v pripade chybnych parametrov (nil, bool) false.
 */
bool buildin_write(const Tvariable * item)
{
    assert(item);

    switch (item->type)
    {
        case VAR_NUM:
            printf("%g", item->val.num);
            break;
        case VAR_STR:
            fputs(item->val.str, stdout);
            break;
        case VAR_BOOL:
        case VAR_NIL:
            xerror_msg(ERR_INT_WRITE_OP);
            return false;
        default:
            assert(0);
            break;
    }

    return true;
}

/**
 * Vyhlada vrati podretazec v retazci. Pozor, funkcia prepise prvy operand.
 * @param - retazec v ktorom sa ma vyhladat podretacec
 * @param - startovacia pozicia
 * @param - ukoncovacia pozicia
 */
bool buildin_substr(Tvariable * src1,
                    const Tvariable * src2,
                    const Tvariable * src3)
{
    if (! src1 || ! src2 || ! src3)
    {
        DEBUG_MSG("substr predany NULL");
        return true;
    }

    if (src1->type != VAR_STR
        || src2->type != VAR_NUM
        || src3->type != VAR_NUM)
    {
        xerror_msg(ERR_INT_SUBSTR_OP);
        return false;
    }

    src1->type = VAR_STR;
    src1->val.str = ial_substr(src1->val.str, src2->val.num, src3->val.num);

    return true;
}

/**
 * Vyhlada vyskyt zadaneho retazca v retazci a vrati jeho poziciu.
 * @param - miesto, kam sa ma ulozit vysledok
 * @param - retazec v ktorom sa ma hladat
 * @param - podretazec, ktory sa ma vyhladat
 * @return - true
 */
bool buildin_find(Tvariable * dst,
                  const Tvariable * str,
                  const Tvariable * pattern)
{
    if (! dst || ! str || ! pattern)
    {
        DEBUG_MSG("find predany NULL");
        return true;
    }

    int result;

    if (str->type != VAR_STR || pattern->type != VAR_STR)
    {
        dst->type = VAR_NIL;
        return true;
    }

    result = ial_find_boyer_moore(str->val.str, pattern->val.str);

    if (result == -1)
    {
        dst->type = VAR_BOOL;
        dst->val.boolean = false;
    }
    else
    {
        dst->type = VAR_NUM;
        dst->val.num = result;
    }

    return true;
}

/**
 * Zoradi znaky v danom retazci podla ordinalnej hodnoty.
 * @param - miesto, kam sa ma ulozit vysledok
 * @param - znaky, ktore sa maju zoradit
 * @return - true
 */
bool buildin_sort(Tvariable * dst, const Tvariable * str)
{
    if (! dst || ! str)
    {
        DEBUG_MSG("sort predany NULL ukazatel");
        return true;
    }

    if (str->type != VAR_STR)
    {
        dst->type = VAR_NIL;
    }
    else
    {
        if (dst != str)
        {
            dst->val.str = xmalloc((strlen(str->val.str) + 1) * sizeof(char));
            strcpy(dst->val.str, str->val.str);
            ial_sort(dst->val.str);
        }
        else
            ial_sort(dst->val.str);

        dst->type = VAR_STR;
    }

    return true;
}
