/**
 * Projekt:     IFJ11
 * Rok:         2011
 * Subor:       ial.c
 * Autor(i):    Fridolin Pokorny    - sort()
 *              Pavol Loffay        - substr()
 *              Pavol Loffay        - find_boyer_moore()
 *
 * - implementacia vyhladania podretazca v retazci
 * - implementacia radeni
 * - implementacia tabulky symbolov
 *
 * Tim:
 *   Fridolin Pokorny   xpokor32
 *   Pavol Loffay       xloffa00
 *   Dusan Madarka      xmadar01
 *   Miroslav Lisik     xlisik00
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "ial.h"
#include "debug.h"
#include "xwrappers.h"
#include "stack.h"
#include "variable.h"
#include "instruction.h"

/**
 * Zameni dva znaky na danych adresach.
 * @param - prvy znak k zamene
 * @param - druhy znak k zamene
 * @return - void
 */
static inline void ial_sortw_switch(char * c1, char * c2)
{
    assert(c1);
    assert(c2);

    char tmp = *c1;
    *c1 = *c2;
    *c2 = tmp;
}

/**
 * Implementacia quick-sort algoritmu pre zoradenie pola znakov alfanumericky.
 * @param - pole znakov, ktore je potrebne zoradit
 * @param - dlzka podretazca, ktory bude radeny
 * @return - void
 */
static void ial_sortw(char * str, size_t len)
{
    assert(str);

    /*
     * Uz nezostava nic na radenie.
     */
    if (len == 0 || len == 1)
        return;

    /*
     * Pivot bude vzdy v strede pola.
     */
    char pivot = str[len / 2];
    unsigned idx_left = 0;
    unsigned idx_right = len - 1;

    do
    {
        /*
         * Preskoci utriedene casti.
         */
        while (str[idx_left] < pivot)
            ++idx_left;
        while (str[idx_right] > pivot)
            --idx_right;

        if (idx_left < idx_right)
        {
            ial_sortw_switch(&str[idx_left], &str[idx_right]);
            ++idx_left;
            --idx_right;
        }
    } while (idx_left < idx_right);

    /*
     * Pokial len == 2, tak len / 2 + 1 == 2 a algoritmus skonci zacyklenim.
     */
    if (len == 2)
        return;

    if (idx_right > 0)
        ial_sortw(&str[0], idx_right + 1);
    if (idx_left < len - 1)
        ial_sortw(&str[idx_left], len - idx_left);
}

/**
 * Zoradi pole znakov podla ich alfanumerickych hodnot vyuzitim quick-sort.
 * @param - pole znakov, ktore je potrebne zoradit
 * @return - void
 */
void ial_sort(char * str)
{
    ial_sortw(str, strlen(str));
}

/**
 * upravy zaciatocny index (substr)
 * @param - neupraveny zaciatocny index
 * @param - dlzka retazca v ktorom sa bude hladat
 * @return - upraveny zaciatocny index,
 * ak from > len vrati  -2
 * ak from > 0 && from <= len       vrati from - 1
 * ak from < 0 && from + len < 0    vrati 0
 * ak from < 0 && from + len >= 0   vrati from + len
 */
static long int from_index(long int intpart_from, long int len)
{
    //vypocet dlzky terazca
    //ak je mensie ako 0
    if (intpart_from < 0)
    {
        //ak po prcitani dlzky je v intervale <0,...>
        if (intpart_from + len >= 0)
            intpart_from = intpart_from + len;
        //ak po pricitani dlzky bolo stale zaporne nastavi sa na 0
        else
            intpart_from = 0;
    }
    else
        //ak bolo kladne a je <= ako len odpocita sa -1, koli indexacii
        //inak sa tam da -1
        if (intpart_from > 0)
        {
            if (intpart_from <= len)
                intpart_from = intpart_from - 1;
            else
                intpart_from = -2; // bolo = 0
        }

    return intpart_from;
}

/**
 * upravy koncovy index (substr)
 * @param - neupraveny koncovy index
 * @param - dlzka retazca v ktorom sa bude hladat
 * @return - upraveny koncovy index
 * ak to < 0 && to + len >= 0   vrati to + len + 1
 * ak to < 0 && to + len < 0    vrati 0
 * ak to > 0 && to <= len       vrati to
 * ak to > 0 && to > len        vrati len
 */
static long int to_index(long int intpart_to, long int len)
{
    //vypocet dlzky terazca
    //ak je mensie ako 0
    if (intpart_to < 0)
    {
        //ak po prcitani dlzky je v intervale <0,...>
        if (intpart_to + len >= 0)
            intpart_to = intpart_to + len + 1;
        //ak po pricitani dlzky bolo stale zaporne nastavi sa na 0
        else
            intpart_to = 0;
    }
    else
        //ak bolo kladne a je <= ako len odpocita sa -1, koli indexacii
        //inak sa nastavi na len - 1
        if (intpart_to > 0)
        {
            if (intpart_to <= len)
                intpart_to = intpart_to ;
            else
                intpart_to = len ;
        }

    return intpart_to;
}

/**
 * vypocita kolko miesta sa ma alokovat pre vrateny retazec (substr)
 * @param - zaciatocny upraveny index
 * @param - koncovy upraveny index
 * @param - dlzka retazca v ktorom sa bude hladat
 * @return - dlzka podretazca + 1(znak  '\0')
 *
 */
static long int alloc_size(long int intpart_from, long int intpart_to)
{
    if (intpart_from == -2)
        return 1;
    return (intpart_to - intpart_from < 0) ? 1 : (intpart_to - intpart_from + 1);
}

/**
 * vrati podretazec v zadanom retazci
 * @param - retazec z ktoreho sa vybere podretazec
 * @param - index zaciatku podretazca, zacina od 1, pricom 0 = 1, a napr.
 * "abc" index -1 = 3, -2 = 2
 * @param - index konca podretazca, plati to iste co pri indexe zaciatku
 * @return - podretazec, pozor vzdy vrati podretazec viz. zadanie
 */
char * ial_substr(char * str, double from, double to)
{
    assert(str);

    //dlzka retazca str
    size_t len = strlen(str);
    //cela cast
    long int intpart_from = from;
    long int intpart_to = to;
    char * ret = NULL;

    //ak sa parametre from, to rovnaju 0, vrattim ""
    if (intpart_from == 0 && intpart_to == 0)
    {
        ret = xmalloc(sizeof(char));
        ret[0] = '\0';
        return ret;
    }

    //upravenie indexov
    intpart_from = from_index(intpart_from, len);
    intpart_to = to_index(intpart_to, len);
    //kolko miesta sa ma alokovat
    long int size = alloc_size(intpart_from, intpart_to);

    //DEBUG_MSG("alloc_size %ld ,  from  %ld,   to '%ld'\n", size, intpart_from, intpart_to );
    ret = xmalloc(size * sizeof(char));

    if (size == 1)
    {
        ret[0] = '\0';
        return ret;
    }

    long int index = 0;
    while (str[intpart_from] != '\0' && intpart_from < intpart_to)
        ret[index++] = str[intpart_from++];

    ret[intpart_from] = '\0';

    return ret;
}

/**
 * upravi pole kde sa nachadzaju posuvy retazca (find_boyer_moore)
 * @param - pole
 * @param - retazec ktory sa hlada
 * @param - jeho dlzka
 * @return - void
 */
static void char_jump_init(long int *arr, char *needle, long int len_needle)
{
    assert(arr);
    assert(needle);

    for (long int i = 0; i < ASCII; i++)
        arr[i] = len_needle;

    for (long int j = 0,i = len_needle - 1; i >= 0; i--)
        arr[(long int)needle[j++]] = i;
}

/**
 * najde needle v source
 * @param - retazec v ktorom sa bude hladat
 * @param - retazec ktory sa bude hladat
 * @return - -1 ak nenajde needle, ak je needle "" vrati 0, inak vrati poziciu
 * kde sa needle nachadza - pocita sa od 1(viz. zadanie),
 */
long int ial_find_boyer_moore(char * source, char * needle)
{
    assert(source);
    assert(needle);

    //ak je prvy retazec prazdny vrati index 0
    if (strlen(needle) == 0 && needle[0] == '\0')
        return 0;

    long int len_source = strlen(source);
    long int len_needle = strlen(needle);

    long int index_source = len_needle - 1;
    long int index_needle = len_needle - 1;

    //vytvorenie a inicializacia char_jump
    long int * char_jump = xmalloc(ASCII * sizeof(long int));

    char_jump_init(char_jump, needle, len_needle);

    while (index_source <= len_source && index_needle >= 0)
    {
        //DEBUG_MSG("%c\n", source[index_source]);
        if (source[index_source] == needle[index_needle])
        {
            index_source--;
            index_needle--;
        }
        else
        {
            index_source = index_source + char_jump[(long int)source[index_source]];
            index_needle = len_needle - 1;
        }
    }

    xfree(char_jump);

    //ihla sa nasla
    if (index_needle == -1)
        return (index_source + 1 + 1);

    return -1;
}

/*******************************************************************************/

/**
 * Vhodna rozptylovacia funkcia podla literatury.
 * @param - kluc, z ktoreho sa bude generovat index
 * @param - velkost tabulky
 * @return - index do hash tabulky
 */
unsigned int ial_hash_function(const char * key, unsigned int htable_size)
{
    unsigned int h = 0;
    unsigned char *p;

    for (p = (unsigned char*)key; *p != '\0'; p++)
        h = 31 * h + *p;

    return h % htable_size;
}

/**
 * Vytvori a inicializuje hash tabulku pred prvym pouzitim.
 * @param - velkost hash tabulky
 * @return - ukazatel na tabulku, v pripade, ze zlyhala alokacia tabulky NULL
 */
Thtable * ial_htable_init(unsigned int htable_size)
{
    Thtable * htptr = NULL;

    htptr = xmalloc(sizeof(Thtable)
                    + htable_size * sizeof(Thtable_item));

    /* inicializacia hash tabulky (velkost, pole ukazatelov na polozky) */
    htptr->htable_size = htable_size;
    for (unsigned int i = 0; i < htable_size; i++)
        htptr->uk[i] = NULL;

    return htptr;
}

/**
 * Vrati pamet obsadenu polozkami hash tabulky.
 * @param - ukazatel na hash tabulku
 * @return - none
 */
void ial_htable_delete(Thtable * htptr)
{
    assert(htptr);

    Thtable_item * tmp;
    Thtable_item * del;

    for (unsigned int i = 0; i < htptr->htable_size; i++)
    {
        tmp = htptr->uk[i];
        while (tmp)
        {
            del = tmp;
            xfree(tmp->key);
            inst_list_destroy(&tmp->inst);
            variable_list_destroy(&tmp->var);
            tmp = tmp->next;
            xfree(del);
        }

    }

    xfree(htptr);
}

/**
 * Vlozi polozku do hash tabulky.
 * @param - ukazatel na hash tabulku
 * @param - nazov vkladanej polozky (funkcie)
 * @return - ukazatel na prave vlozenu polozku
 */
Thtable_item * ial_htable_insert(Thtable * htptr, char * name)
{
    assert(htptr);

    unsigned int idx = ial_hash_function(name, htptr->htable_size);

    Thtable_item * new = xmalloc(sizeof(Thtable_item));

    /* inicializacia struktury Thtable_item */
    new->next = htptr->uk[idx];
    new->key = name;
    new->param_number = 0;
    new->ret_value.type = VAR_NIL;

    variable_list_init(&new->var);
    inst_list_init(&new->inst);

    /* vlozenie novej polozky do hash tabulky */
    htptr->uk[idx] = new;

    /* vratime ukazatel na vlozenu polozku, aby sme mohli pristupit k variabile
     * listu, ked budeme vkladat parametre a premenne
     */
    return new;
}

/**
 * Nastavi pocet parametrov danej funkcii v hash table. Ak funkcia s danym menom
 * neexistuje, funckia je bez efektu.
 * @param - ukazatel na hash tabulku
 * @param - nazov funkcie
 * @return - none
 */
void ial_htable_inc_param_count(Thtable_item * func)
{
    assert(func);

    func->param_number += 1;
}

/**
 * Nastavi aktivitu prveho v zozname premennych a vrati ukazatel na prvy.
 * @param - ukazatel na hash tabulku
 * @return - ukazatel na aktivny prvok
 */
Tvariable * ial_htable_var_first(Thtable_item * func)
{
    assert(func);

    return variable_list_first(&func->var);
}

/**
 * Vrati ukazatel na aktivny prvok a posunie aktivitu o jedna dopredu.
 * @param - ukazatel na hash tabulku
 * @return - ukazatel na predosly aktivny prvok
 */
Tvariable * ial_htable_var_succ(Thtable_item * func)
{
    assert(func);

    return variable_list_succ(&func->var);
}
/**
 * Nastavi aktivitu na prvu instrukciu v zozname instrukcii.
 * @param - polozka hashtabulky so zoznamom instrukcii
 * @return - aktivna instrukcia
 */
Tinstruction * ial_htable_inst_first(Thtable_item * func)
{
    assert(func);

    return inst_list_first(&(func->inst));
}

/**
 * Nastavi aktivitu na nasledujucu instrukciu v zozname instrukcii.
 * @param - polozka hashtabulky so zoznamom instrukcii
 * @return - aktivna instrukcia
 */
Tinstruction * ial_htable_inst_succ(Thtable_item * func)
{
    assert(func);

    return inst_list_succ(&(func->inst));
}

/**
 * zisti ci sa dana funkcia nachadza
 * @param - ukazatel na htable
 * @param - kluc podla ktoreho sa vyhlada v htable
 * @return - ak najde dany zaznam v htable, vrati ukazatel na neho, inak NULL
 */
Thtable_item * ial_htable_look_func(const Thtable * htable, const char * name)
{
    unsigned int index = ial_hash_function(name, htable->htable_size);

    for (Thtable_item * item = htable->uk[index]; item != NULL; item = item->next)
    {
        if (strcmp(name, item->key) == 0)
            return item;
    }

    return NULL;
}

/**
 * vyhlada ci je v danej funkcii deklarovana premenna
 * @param - ukazatel na zaznam v htable - uk na zaznam o funkcii
 * @param - nazov premennej
 * @return - ukazatel na premennu, inak neexistuje NULL,
 */
Tvariable * ial_htable_look_var(const Thtable_item * func, const char * name)
{
    return variable_list_look(&func->var, name);
}

/**
 * prida premennu do f_var_list, vzdy na zaciatok
 * @param - ukazatel na premennu
 * @param - ukazatel na polozku htable
 * @return - void
 */
void ial_htable_insert_var(Tvariable * new_var, Thtable_item * htable_item)
{
    variable_list_append(&(htable_item->var), new_var);
}
